#include "fanIncludes.h"

#include "vulkan/util/vkWindow.h"

namespace vk {

	//================================================================================================================================
	//================================================================================================================================
	Window::Window(const char * _name, const VkExtent2D _size, const glm::ivec2 _position, VkInstance _vkInstance) :
		m_vkInstance(_vkInstance) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_window = glfwCreateWindow(_size.width, _size.height, _name, nullptr/* fullscreen monitor */, nullptr);
		glfwCreateWindowSurface(_vkInstance, m_window, nullptr, &m_surface);
		std::cout << std::hex << "VkSurfaceKHR\t\t" << m_surface << std::dec << std::endl;

		glfwSetWindowPos(m_window, _position.x, _position.y);
	}

	//======================================================c==========================================================================
	//================================================================================================================================
	Window::~Window() {
		vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
		m_surface = VK_NULL_HANDLE;
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	//================================================================================================================================
	//================================================================================================================================
	VkExtent2D Window::GetExtent() const {
		int width; int height;
		glfwGetFramebufferSize(m_window, &width, &height);
		return { static_cast<uint32_t>(width) ,static_cast<uint32_t>(height) };
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::ivec2	Window::GetPosition() const {
		glm::ivec2 position;
		glfwGetWindowPos(m_window, &position.x, &position.y);
		return position;
	}
}