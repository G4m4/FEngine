#include "core/input/fanInput.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/input/fanInputManager.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	Input::Input()
	{
        mInputManager = new InputManager();
	}

	//========================================================================================================
	//========================================================================================================
	void Input::Setup( GLFWwindow* _window )
	{
        mWindow = _window;

		int width, height;
		glfwGetWindowSize( _window, &width, &( height ) );

		double x, y;
		glfwGetCursorPos( _window, &x, &y );
		glfwSetKeyCallback( _window, Keyboard::KeyCallback );
		glfwSetCharCallback( _window, Keyboard::CharCallback );
	}

	//========================================================================================================
	//========================================================================================================
	void Input::NewFrame()
	{
		++Get().mCount;
		glfwPollEvents();
	}
}