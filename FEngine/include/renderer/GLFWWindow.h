#pragma once

#include <cstdint>
#include <string>
#include <iostream>

#include "vulkan/vulkan.h"
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#include "util/UserInput.h"

class Window
{
public:
	Window(uint32_t width, uint32_t height, std::string name);
	virtual ~Window();

	bool WindowOpen() const;
	void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	VkExtent2D GetExtend2D() const;
	GLFWwindow * GetGLFWwindow() const { return m_window; }

	int GetRefreshRate();

private:
	uint32_t m_width;
	uint32_t m_height;

	GLFWwindow * m_window;

};