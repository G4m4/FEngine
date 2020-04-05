#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{
	//================================================================================================================================
	// The list of available keys in a single enum
	//================================================================================================================================

	enum KeyMask
	{
		_SPACE			= 1 << GLFW_KEY_SPACE
		, APOSTROPHE	= 1 << GLFW_KEY_APOSTROPHE
		, COMMA			= 1 << GLFW_KEY_COMMA
		, MINUS			= 1 << GLFW_KEY_MINUS
		, PERIOD		= 1 << GLFW_KEY_PERIOD
		, SLASH			= 1 << GLFW_KEY_SLASH
		, KEY0			= 1 << GLFW_KEY_0
		, KEY1			= 1 << GLFW_KEY_1
		, KEY2			= 1 << GLFW_KEY_2
		, KEY3			= 1 << GLFW_KEY_3
		, KEY4			= 1 << GLFW_KEY_4
		, KEY5			= 1 << GLFW_KEY_5
		, KEY6			= 1 << GLFW_KEY_6
		, KEY7			= 1 << GLFW_KEY_7
		, KEY8			= 1 << GLFW_KEY_8
		, KEY9			= 1 << GLFW_KEY_9
		, SEMICOLON		= 1 << GLFW_KEY_SEMICOLON
		, EQUAL			= 1 << GLFW_KEY_EQUAL
		, A				= 1 << GLFW_KEY_A
		, B				= 1 << GLFW_KEY_B
		, C				= 1 << GLFW_KEY_C
		, D				= 1 << GLFW_KEY_D
		, E				= 1 << GLFW_KEY_E
		, F				= 1 << GLFW_KEY_F
		, G				= 1 << GLFW_KEY_G
		, H				= 1 << GLFW_KEY_H
		, I				= 1 << GLFW_KEY_I
		, J				= 1 << GLFW_KEY_J
		, K				= 1 << GLFW_KEY_K
		, L				= 1 << GLFW_KEY_L
		, M				= 1 << GLFW_KEY_M
		, N				= 1 << GLFW_KEY_N
		, O				= 1 << GLFW_KEY_O
		, P				= 1 << GLFW_KEY_P
		, Q				= 1 << GLFW_KEY_Q
		, R				= 1 << GLFW_KEY_R
		, S				= 1 << GLFW_KEY_S
		, T				= 1 << GLFW_KEY_T
		, U				= 1 << GLFW_KEY_U
		, V				= 1 << GLFW_KEY_V
		, W				= 1 << GLFW_KEY_W
		, X				= 1 << GLFW_KEY_X
		, Y				= 1 << GLFW_KEY_Y
		, Z				= 1 << GLFW_KEY_Z
		, LEFT_BRACKET	= 1 << GLFW_KEY_LEFT_BRACKET
		, BACKSLASH		= 1 << GLFW_KEY_BACKSLASH
		, RIGHT_BRACKET = 1 << GLFW_KEY_RIGHT_BRACKET
		, GRAVE_ACCENT	= 1 << GLFW_KEY_GRAVE_ACCENT
		, WORLD_1		= 1 << GLFW_KEY_WORLD_1
		, WORLD_2		= 1 << GLFW_KEY_WORLD_2
		, ESCAPE		= 1 << GLFW_KEY_ESCAPE
		, ENTER			= 1 << GLFW_KEY_ENTER
		, TAB			= 1 << GLFW_KEY_TAB
		, BACKSPACE		= 1 << GLFW_KEY_BACKSPACE
		, INSERT		= 1 << GLFW_KEY_INSERT
		, DELETE		= 1 << GLFW_KEY_DELETE
		, RIGHT			= 1 << GLFW_KEY_RIGHT
		, LEFT			= 1 << GLFW_KEY_LEFT
		, DOWN			= 1 << GLFW_KEY_DOWN
		, UP			= 1 << GLFW_KEY_UP
		, PAGE_UP		= 1 << GLFW_KEY_PAGE_UP
		, PAGE_DOWN		= 1 << GLFW_KEY_PAGE_DOWN
		, HOME			= 1 << GLFW_KEY_HOME
		, END			= 1 << GLFW_KEY_END
		, CAPS_LOCK		= 1 << GLFW_KEY_CAPS_LOCK
		, SCROLL_LOCK	= 1 << GLFW_KEY_SCROLL_LOCK
		, NUM_LOCK		= 1 << GLFW_KEY_NUM_LOCK
		, PRINT_SCREEN	= 1 << GLFW_KEY_PRINT_SCREEN
		, PAUSE			= 1 << GLFW_KEY_PAUSE
		, F1			= 1 << GLFW_KEY_F1
		, F2			= 1 << GLFW_KEY_F2
		, F3			= 1 << GLFW_KEY_F3
		, F4			= 1 << GLFW_KEY_F4
		, F5			= 1 << GLFW_KEY_F5
		, F6			= 1 << GLFW_KEY_F6
		, F7			= 1 << GLFW_KEY_F7
		, F8			= 1 << GLFW_KEY_F8
		, F9			= 1 << GLFW_KEY_F9
		, F10			= 1 << GLFW_KEY_F10
		, F11			= 1 << GLFW_KEY_F11
		, F12			= 1 << GLFW_KEY_F12
		, F13			= 1 << GLFW_KEY_F13
		, F14			= 1 << GLFW_KEY_F14
		, F15			= 1 << GLFW_KEY_F15
		, F16			= 1 << GLFW_KEY_F16
		, F17			= 1 << GLFW_KEY_F17
		, F18			= 1 << GLFW_KEY_F18
		, F19			= 1 << GLFW_KEY_F19
		, F20			= 1 << GLFW_KEY_F20
		, F21			= 1 << GLFW_KEY_F21
		, F22			= 1 << GLFW_KEY_F22
		, F23			= 1 << GLFW_KEY_F23
		, F24			= 1 << GLFW_KEY_F24
		, F25			= 1 << GLFW_KEY_F25
		, KP_0			= 1 << GLFW_KEY_KP_0
		, KP_1			= 1 << GLFW_KEY_KP_1
		, KP_2			= 1 << GLFW_KEY_KP_2
		, KP_3			= 1 << GLFW_KEY_KP_3
		, KP_4			= 1 << GLFW_KEY_KP_4
		, KP_5			= 1 << GLFW_KEY_KP_5
		, KP_6			= 1 << GLFW_KEY_KP_6
		, KP_7			= 1 << GLFW_KEY_KP_7
		, KP_8			= 1 << GLFW_KEY_KP_8
		, KP_9			= 1 << GLFW_KEY_KP_9
		, KP_DECIMAL	= 1 << GLFW_KEY_KP_DECIMAL
		, KP_DIVIDE		= 1 << GLFW_KEY_KP_DIVIDE
		, KP_MULTIPLY	= 1 << GLFW_KEY_KP_MULTIPLY
		, KP_SUBTRACT	= 1 << GLFW_KEY_KP_SUBTRACT
		, KP_ADD		= 1 << GLFW_KEY_KP_ADD
		, KP_ENTER		= 1 << GLFW_KEY_KP_ENTER
		, KP_EQUAL		= 1 << GLFW_KEY_KP_EQUAL
		, LEFT_SHIFT	= 1 << GLFW_KEY_LEFT_SHIFT
		, LEFT_CONTROL	= 1 << GLFW_KEY_LEFT_CONTROL
		, LEFT_ALT		= 1 << GLFW_KEY_LEFT_ALT
		, LEFT_SUPER	= 1 << GLFW_KEY_LEFT_SUPER
		, RIGHT_SHIFT	= 1 << GLFW_KEY_RIGHT_SHIFT
		, RIGHT_CONTROL = 1 << GLFW_KEY_RIGHT_CONTROL
		, RIGHT_ALT		= 1 << GLFW_KEY_RIGHT_ALT
		, RIGHT_SUPER	= 1 << GLFW_KEY_RIGHT_SUPER
		, MENU			= 1 << GLFW_KEY_MENU
	};
}