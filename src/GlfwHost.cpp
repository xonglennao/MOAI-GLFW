//
//  GlfwHost.cpp
//  MOAI-GLFW
//
//  Created by Luke Perkin on 19/08/2012.
//  Copyright (c) 2012 Locofilm. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glfw.h>
#include <aku/AKU.h>
#include <aku/AKU-debugger.h>
#include <aku/AKU-luaext.h>
#include <aku/AKU-untz.h>
#include <aku/AKU-audiosampler.h>
#include <lua-headers/moai_lua.h>
#include "GlfwHost.h"
#include <CoreFoundation/CFBundle.h>
#include "MOAIHostMgr.h"

namespace GLFWInputDeviceID {
    enum {
        DEVICE,
        TOTAL,
    };
}

namespace GLFWInputDeviceSensorID {
    enum {
        KEYBOARD,
        POINTER,
        MOUSE_LEFT,
        MOUSE_MIDDLE,
        MOUSE_RIGHT,
        MOUSE_WHEEL,
        JOYSTICK,
        TOTAL,
    };
}

int windowWidth;
int windowHeight;
bool isFullscreen = FALSE;

void _AKUOpenWindowFunc( const char* title, int width, int height )
{
    if( !glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW) )
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    } else {
        glfwSetWindowTitle(title);
        AKUDetectGfxContext();
        AKUSetScreenSize( width, height );
        AKUSetViewSize( width, height );
        SetupInputCallbacks();
    }
}

void _AKUEnterFullscreenFunc()
{
    if (!isFullscreen) {
        isFullscreen = TRUE;
        glfwGetWindowSize(&windowWidth, &windowHeight);
        GLFWvidmode mode;
        glfwGetDesktopMode( &mode );
        glfwCloseWindow();
        glfwOpenWindow(mode.Width, mode.Height, 0, 0, 0, 0, 0, 0, GLFW_FULLSCREEN);
        glfwEnable( GLFW_MOUSE_CURSOR );
        AKUDetectGfxContext();
        AKUSetScreenSize( mode.Width, mode.Height );
        AKUSetViewSize( mode.Width, mode.Height );
        SetupInputCallbacks();
    }
}

void _AKUExitFullscreenFunc()
{
    if (isFullscreen) {
        isFullscreen = FALSE;
        glfwCloseWindow();
        glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
        AKUDetectGfxContext();
        AKUSetScreenSize( windowWidth, windowHeight );
        AKUSetViewSize( windowWidth, windowHeight );
        SetupInputCallbacks();
    }
}

void _AKUErrorTracebackFunc ( const char* message, lua_State* L, int level );
void _AKUErrorTracebackFunc ( const char* message, lua_State* L, int level )
{
    //AKUDebugHarnessHandleError(message, L, level);
}

void GLFWCALL onMouseButton( int button, int action )
{
    printf("onMouseButton");
    bool isPress = (action == GLFW_PRESS);
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            AKUEnqueueButtonEvent(GLFWInputDeviceID::DEVICE, GLFWInputDeviceSensorID::MOUSE_LEFT, isPress);
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            AKUEnqueueButtonEvent(GLFWInputDeviceID::DEVICE, GLFWInputDeviceSensorID::MOUSE_MIDDLE, isPress);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            AKUEnqueueButtonEvent(GLFWInputDeviceID::DEVICE, GLFWInputDeviceSensorID::MOUSE_RIGHT, isPress);
            break;
        default:
            break;
    }
}

void GLFWCALL onKeyboardKey( int key, int action )
{
    bool isDown = (action == GLFW_PRESS);
    AKUEnqueueKeyboardEvent(GLFWInputDeviceID::DEVICE, GLFWInputDeviceSensorID::KEYBOARD, key, isDown);
}

void GLFWCALL onKeyboardChar( int unicode, int action )
{
    
}

void GLFWCALL onMouseMove( int x, int y )
{
    AKUEnqueuePointerEvent(GLFWInputDeviceID::DEVICE, GLFWInputDeviceSensorID::POINTER, x, y);
}

void GLFWCALL onMouseWheel( int pos )
{
    AKUEnqueueWheelEvent(GLFWInputDeviceID::DEVICE, GLFWInputDeviceSensorID::MOUSE_WHEEL, pos);
}

void GLFWCALL onWindowSize( int width, int height )
{
    AKUSetViewSize( width, height );
}

int GLFWCALL onWindowClose()
{
    int result = GL_TRUE;
    
	if ( MOAIHostMgr::Get().GetOnWindowClosedLuaFunction() ) {
		MOAILuaStateHandle state = MOAIHostMgr::Get().GetOnWindowClosedLuaFunction().GetSelf ();
		state.DebugCall ( 0, 1 );
        result = state.GetValue < int >( 0, GL_FALSE );
	}
    
    return result;
}

static void _cleanup()
{
    AKUFinalize();
}

int GlfwHost( int argc, const char * argv[] )
{
    atexit(_cleanup);
    RefreshAKUContext();
    
    REGISTER_LUA_CLASS ( MOAIHostMgr )
	printf("Registered MOAIHostMgr\n");
    
    // Load lua script from command-line argument.
    
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    
    CFDictionaryRef infoDict = CFBundleGetInfoDictionary(mainBundle);
    CFTypeRef scriptName = CFDictionaryGetValue(infoDict, CFSTR("MTMainScript"));
    CFURLRef scriptURL = CFBundleCopyResourceURL(mainBundle, (CFStringRef)scriptName, CFSTR("lua"), NULL);
    
    if (scriptURL) {
        CFStringRef scriptPath = CFURLCopyFileSystemPath(scriptURL, kCFURLPOSIXPathStyle);
        CFStringEncoding encodingMethod = CFStringGetSystemEncoding();
        
        const char *path = CFStringGetCStringPtr(scriptPath, encodingMethod);
        
        AKURunScript ( path );

    } else if (argc < 2) {
    
        AKURunScript("main.lua");
    
    } else {
    
        for (int i = 1; i < argc; ++i) {
            const char* arg = argv[i];
            AKURunScript(arg);
        }
        
    }
        
    GlfwEventLoop();
    
    return 0;
}

void RefreshAKUContext()
{
    AKUContextID context = AKUGetContext();
    if (context) {
        AKUDeleteContext(context);
    }
    AKUCreateContext();
    
    LoadAKUModules();
    SetupInputMapping();
    SetupWindowMapping();
    //SetupErrorMapping();
    
    AKURunBytecode( moai_lua, moai_lua_SIZE );
}

void LoadAKUModules()
{
    AKUExtLoadLuacrypto ();
    AKUExtLoadLuacurl ();
    AKUExtLoadLuafilesystem ();
    AKUExtLoadLuasocket ();
    AKUExtLoadLuasql ();
    AKUUntzInit ();
    AKUAudioSamplerInit();
}

void SetupInputCallbacks()
{
    glfwSetWindowSizeCallback(onWindowSize);
    glfwSetWindowCloseCallback(onWindowClose);
    glfwSetKeyCallback(onKeyboardKey);
    glfwSetMouseButtonCallback(onMouseButton);
    glfwSetMousePosCallback(onMouseMove);
    glfwSetMouseWheelCallback(onMouseWheel);
}

void SetupInputMapping()
{
    AKUSetInputConfigurationName ( "AKU_GLFW" );
    AKUReserveInputDevices( GLFWInputDeviceID::TOTAL );
    AKUSetInputDevice(GLFWInputDeviceID::DEVICE, "device");
    AKUReserveInputDeviceSensors(GLFWInputDeviceID::DEVICE, GLFWInputDeviceSensorID::TOTAL);
    AKUSetInputDeviceKeyboard(GLFWInputDeviceID::DEVICE,    GLFWInputDeviceSensorID::KEYBOARD,      "keyboard");
    AKUSetInputDevicePointer(GLFWInputDeviceID::DEVICE,     GLFWInputDeviceSensorID::POINTER,       "pointer");
    AKUSetInputDeviceButton(GLFWInputDeviceID::DEVICE,      GLFWInputDeviceSensorID::MOUSE_LEFT,    "mouseLeft");
    AKUSetInputDeviceButton(GLFWInputDeviceID::DEVICE,      GLFWInputDeviceSensorID::MOUSE_MIDDLE,  "mouseMiddle");
    AKUSetInputDeviceButton(GLFWInputDeviceID::DEVICE,      GLFWInputDeviceSensorID::MOUSE_RIGHT,   "mouseRight");
    AKUSetInputDeviceWheel(GLFWInputDeviceID::DEVICE,       GLFWInputDeviceSensorID::MOUSE_WHEEL,   "mouseWheel");
}

void SetupWindowMapping()
{
    AKUSetFunc_OpenWindow(_AKUOpenWindowFunc);
    AKUSetFunc_EnterFullscreenMode(_AKUEnterFullscreenFunc);
    AKUSetFunc_ExitFullscreenMode(_AKUExitFullscreenFunc);
}

void SetupErrorMapping()
{
    AKUSetFunc_ErrorTraceback(_AKUErrorTracebackFunc);
}

void GlfwEventLoop()
{
    int running = GL_TRUE;
    while(running)
    {
        AKUUpdate();
        if (glfwGetWindowParam(GLFW_OPENED)) {
            glClear(GL_COLOR_BUFFER_BIT);
            AKURender();
            glfwSwapBuffers();
            running = glfwGetWindowParam(GLFW_OPENED);
        }
    }
}