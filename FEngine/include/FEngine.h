#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>





class FEngine
{
public:
	void Run();

	const int WIDTH = 800;
	const int HEIGHT = 600;
	GLFWwindow * window;

private:
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkDevice device;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;

	const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	//Vulkan callbacks
	static VkResult CreateDebugReportCallback(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback );
	static void DestroyDebugReportCallback(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData
	);

	struct QueueFamilyIndices
	{
		int graphicsFamily = -1;

		bool isComplete() {
			return graphicsFamily >= 0;
		}
	};

	void initWindow();
	void initVulkan();
	void createLogicalDevice();
	void createInstance();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	void setupDebugCallback();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();



	void mainLoop();
	void cleanup();
};

