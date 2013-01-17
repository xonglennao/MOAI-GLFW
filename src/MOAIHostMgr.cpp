// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include "MOAIHostMgr.h"
#include <GL/glfw.h>

//================================================================//
// lua
//================================================================//

int MOAIHostMgr::_setWindowSize ( lua_State* L )
{
	int width = lua_tointeger(L, 1);
	int height = lua_tointeger(L, 2);
	glfwSetWindowSize( width, height );
	return 0;
}

int MOAIHostMgr::_getTime( lua_State* L )
{
	double time = glfwGetTime();
	lua_pushnumber(L, time);
	return 1;
}

static void pushLuaKeyNumberValue( lua_State* L, const char* s, int value )
{
	lua_pushstring(L, s);
	lua_pushnumber(L, value);
	lua_settable(L, -3);
}

int MOAIHostMgr::_getVideoModes( lua_State* L )
{
	int maxModes = lua_tointeger(L, 1);
	int numModes;
	GLFWvidmode videoModes[ maxModes ];
	numModes = glfwGetVideoModes(videoModes, maxModes);
	
	lua_newtable(L);
	int luaVideoModesTableIndex = lua_gettop(L);
	
	for (int i = 0; i < numModes; i++) {
		GLFWvidmode videoMode = videoModes[i];
		lua_pushnumber(L, i + 1);
		lua_newtable(L);
		pushLuaKeyNumberValue(L, "width",		videoMode.Width);
		pushLuaKeyNumberValue(L, "height",		videoMode.Height);
		pushLuaKeyNumberValue(L, "redBits",		videoMode.RedBits);
		pushLuaKeyNumberValue(L, "greenBits",	videoMode.GreenBits);
		pushLuaKeyNumberValue(L, "blueBits",	videoMode.BlueBits);
		lua_settable(L, luaVideoModesTableIndex);
	}
	
	return 1;
}

int MOAIHostMgr::_getDesktopVideoMode( lua_State* L )
{
	GLFWvidmode videoMode;
	glfwGetDesktopMode(&videoMode);
	lua_newtable(L);
	pushLuaKeyNumberValue(L, "width", videoMode.Width);
	pushLuaKeyNumberValue(L, "height", videoMode.Height);
	pushLuaKeyNumberValue(L, "redBits", videoMode.RedBits);
	pushLuaKeyNumberValue(L, "greenBits", videoMode.GreenBits);
	pushLuaKeyNumberValue(L, "blueBits", videoMode.BlueBits);
	return 1;
}

int MOAIHostMgr::_setWindowPos( lua_State *L )
{
	int x = lua_tointeger(L, 1);
	int y = lua_tointeger(L, 2);
	glfwSetWindowPos(x, y);
	return 0;
}

int MOAIHostMgr::_minimiseWindow( lua_State* L )
{
	UNUSED(L);
	glfwIconifyWindow();
	return 0;
}

int MOAIHostMgr::_restoreWindow( lua_State* L )
{
	UNUSED(L);
	glfwRestoreWindow();
	return 0;
}

int MOAIHostMgr::_getNumberOfProcessors( lua_State* L )
{
	int processors = glfwGetNumberOfProcessors();
	lua_pushinteger(L, processors);
	return 1;
}

int MOAIHostMgr::_enable( lua_State* L )
{
	int token = lua_tointeger(L, 1);
	glfwEnable(token);
	return 0;
}

int MOAIHostMgr::_disable( lua_State* L )
{
	int token = lua_tointeger(L, 1);
	glfwDisable(token);
	return 0;
}

int MOAIHostMgr::_setOnWindowCloseCalback ( lua_State* L ) {
	MOAILuaState state ( L );
	if ( state.CheckParams ( 1 , "F" ) ) {
		MOAIHostMgr::Get().mOnWindowClosed.SetStrongRef ( state, 1 );
	}
	
	return 0;
}

MOAILuaRef MOAIHostMgr::GetOnWindowClosedLuaFunction()
{
	return mOnWindowClosed;
}

//================================================================//
// MOAIHostMgr
//================================================================//

//----------------------------------------------------------------//
MOAIHostMgr::MOAIHostMgr () {
	// register all classes MOAIHostMgr derives from
	// we need this for custom RTTI implementation
	RTTI_BEGIN
		RTTI_EXTEND ( MOAILuaObject )
		// and any other objects from multiple inheritance...
		// RTTI_EXTEND ( MOAIFooMgrBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIHostMgr::~MOAIHostMgr () {
}

//----------------------------------------------------------------//
void MOAIHostMgr::RegisterLuaClass ( MOAILuaState& state ) {
	// call any initializers for base classes here:
	// MOAIFooBase::RegisterLuaClass ( state );
	// also register constants:
	// state.SetField ( -1, "FOO_CONST", ( u32 )FOO_CONST );
	state.SetField(-1, "HOST_MOUSE_CURSOR", (u32)GLFW_MOUSE_CURSOR);
	state.SetField(-1, "HOST_KEY_REPEAT", (u32)GLFW_KEY_REPEAT);
	state.SetField(-1, "HOST_STICKY_KEYS", (u32)GLFW_STICKY_KEYS);
	state.SetField(-1, "HOST_STICKY_MOUSE_BUTTONS", (u32)GLFW_STICKY_MOUSE_BUTTONS);
	state.SetField(-1, "HOST_SYSTEM_KEYS", (u32)GLFW_SYSTEM_KEYS);
    
    // Map key constants
    state.SetField(-1, "HOST_KEY_SPACE", (u32) GLFW_KEY_SPACE);
    state.SetField(-1, "HOST_KEY_ESC", (u32) GLFW_KEY_ESC);
    state.SetField(-1, "HOST_KEY_F1", (u32) GLFW_KEY_F1);
    state.SetField(-1, "HOST_KEY_F2", (u32) GLFW_KEY_F2);
    state.SetField(-1, "HOST_KEY_F3", (u32) GLFW_KEY_F3);
    state.SetField(-1, "HOST_KEY_F4", (u32) GLFW_KEY_F4);
    state.SetField(-1, "HOST_KEY_F5", (u32) GLFW_KEY_F5);
    state.SetField(-1, "HOST_KEY_F6", (u32) GLFW_KEY_F6);
    state.SetField(-1, "HOST_KEY_F7", (u32) GLFW_KEY_F7);
    state.SetField(-1, "HOST_KEY_F8", (u32) GLFW_KEY_F8);
    state.SetField(-1, "HOST_KEY_F9", (u32) GLFW_KEY_F9);
    state.SetField(-1, "HOST_KEY_F10", (u32) GLFW_KEY_F10);
    state.SetField(-1, "HOST_KEY_F11", (u32) GLFW_KEY_F11);
    state.SetField(-1, "HOST_KEY_F12", (u32) GLFW_KEY_F12);
    state.SetField(-1, "HOST_KEY_F13", (u32) GLFW_KEY_F13);
    state.SetField(-1, "HOST_KEY_F14", (u32) GLFW_KEY_F14);
    state.SetField(-1, "HOST_KEY_F15", (u32) GLFW_KEY_F15);
    state.SetField(-1, "HOST_KEY_F16", (u32) GLFW_KEY_F16);
    state.SetField(-1, "HOST_KEY_F17", (u32) GLFW_KEY_F17);
    state.SetField(-1, "HOST_KEY_F18", (u32) GLFW_KEY_F18);
    state.SetField(-1, "HOST_KEY_F19", (u32) GLFW_KEY_F19);
    state.SetField(-1, "HOST_KEY_F20", (u32) GLFW_KEY_F20);
    state.SetField(-1, "HOST_KEY_F21", (u32) GLFW_KEY_F21);
    state.SetField(-1, "HOST_KEY_F22", (u32) GLFW_KEY_F22);
    state.SetField(-1, "HOST_KEY_F23", (u32) GLFW_KEY_F23);
    state.SetField(-1, "HOST_KEY_F24", (u32) GLFW_KEY_F24);
    state.SetField(-1, "HOST_KEY_F25", (u32) GLFW_KEY_F25);
    state.SetField(-1, "HOST_KEY_UP", (u32) GLFW_KEY_UP);
    state.SetField(-1, "HOST_KEY_DOWN", (u32) GLFW_KEY_DOWN);
    state.SetField(-1, "HOST_KEY_LEFT", (u32) GLFW_KEY_LEFT);
    state.SetField(-1, "HOST_KEY_RIGHT", (u32) GLFW_KEY_RIGHT);
    state.SetField(-1, "HOST_KEY_LSHIFT", (u32) GLFW_KEY_LSHIFT);
    state.SetField(-1, "HOST_KEY_RSHIFT", (u32) GLFW_KEY_RSHIFT);
    state.SetField(-1, "HOST_KEY_LCTRL", (u32) GLFW_KEY_LCTRL);
    state.SetField(-1, "HOST_KEY_RCTRL", (u32) GLFW_KEY_RCTRL);
    state.SetField(-1, "HOST_KEY_LALT", (u32) GLFW_KEY_LALT);
    state.SetField(-1, "HOST_KEY_RALT", (u32) GLFW_KEY_RALT);
    state.SetField(-1, "HOST_KEY_LSUPER", (u32) GLFW_KEY_LSUPER);
    state.SetField(-1, "HOST_KEY_RSUPER", (u32) GLFW_KEY_RSUPER);
    state.SetField(-1, "HOST_KEY_TAB", (u32) GLFW_KEY_TAB);
    state.SetField(-1, "HOST_KEY_ENTER", (u32) GLFW_KEY_ENTER);
    state.SetField(-1, "HOST_KEY_BACKSPACE", (u32) GLFW_KEY_BACKSPACE);
    state.SetField(-1, "HOST_KEY_INSERT", (u32) GLFW_KEY_INSERT);
    state.SetField(-1, "HOST_KEY_DEL", (u32) GLFW_KEY_DEL);
    state.SetField(-1, "HOST_KEY_PAGEUP", (u32) GLFW_KEY_PAGEUP);
    state.SetField(-1, "HOST_KEY_PAGEDOWN", (u32) GLFW_KEY_PAGEDOWN);
    state.SetField(-1, "HOST_KEY_HOME", (u32) GLFW_KEY_HOME);
    state.SetField(-1, "HOST_KEY_END", (u32) GLFW_KEY_END);
    state.SetField(-1, "HOST_KEY_KP_0", (u32) GLFW_KEY_KP_0);
    state.SetField(-1, "HOST_KEY_KP_1", (u32) GLFW_KEY_KP_1);
    state.SetField(-1, "HOST_KEY_KP_2", (u32) GLFW_KEY_KP_2);
    state.SetField(-1, "HOST_KEY_KP_3", (u32) GLFW_KEY_KP_3);
    state.SetField(-1, "HOST_KEY_KP_4", (u32) GLFW_KEY_KP_4);
    state.SetField(-1, "HOST_KEY_KP_5", (u32) GLFW_KEY_KP_5);
    state.SetField(-1, "HOST_KEY_KP_6", (u32) GLFW_KEY_KP_6);
    state.SetField(-1, "HOST_KEY_KP_7", (u32) GLFW_KEY_KP_7);
    state.SetField(-1, "HOST_KEY_KP_8", (u32) GLFW_KEY_KP_8);
    state.SetField(-1, "HOST_KEY_KP_9", (u32) GLFW_KEY_KP_9);
    state.SetField(-1, "HOST_KEY_KP_DIVIDE", (u32) GLFW_KEY_KP_DIVIDE);
    state.SetField(-1, "HOST_KEY_KP_MULTIPLY", (u32) GLFW_KEY_KP_MULTIPLY);
    state.SetField(-1, "HOST_KEY_KP_SUBTRACT", (u32) GLFW_KEY_KP_SUBTRACT);
    state.SetField(-1, "HOST_KEY_KP_ADD", (u32) GLFW_KEY_KP_ADD);
    state.SetField(-1, "HOST_KEY_KP_DECIMAL", (u32) GLFW_KEY_KP_DECIMAL);
    state.SetField(-1, "HOST_KEY_KP_EQUAL", (u32) GLFW_KEY_KP_EQUAL);
    state.SetField(-1, "HOST_KEY_KP_ENTER", (u32) GLFW_KEY_KP_ENTER);
    state.SetField(-1, "HOST_KEY_KP_NUM_LOCK", (u32) GLFW_KEY_KP_NUM_LOCK);
    state.SetField(-1, "HOST_KEY_CAPS_LOCK", (u32) GLFW_KEY_CAPS_LOCK);
    state.SetField(-1, "HOST_KEY_SCROLL_LOCK", (u32) GLFW_KEY_SCROLL_LOCK);
    state.SetField(-1, "HOST_KEY_PAUSE", (u32) GLFW_KEY_PAUSE);
    state.SetField(-1, "HOST_KEY_MENU", (u32) GLFW_KEY_MENU);
    
	// here are the class methods:
	luaL_Reg regTable [] = {
		{ "setWindowSize",		_setWindowSize },
		{ "getTime",			_getTime },
		{ "getVideoModes",		_getVideoModes },
		{ "getDesktopVideoMode", _getDesktopVideoMode },
		{ "minimiseWindow", _minimiseWindow },
		{ "restoreWindow", _restoreWindow },
		{ "setWindowPosition", _setWindowPos },
        { "setOnWindowCloseCallback", _setOnWindowCloseCalback },
		{ "getNumberOfProcessors", _getNumberOfProcessors },
		{ "enable", _enable },
		{ "disable", _disable },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}
