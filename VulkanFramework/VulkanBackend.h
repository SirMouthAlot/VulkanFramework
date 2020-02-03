#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include <vector>
#include <cstring>

#include "VulkanImport.h"

class VulkanBackend abstract
{
public:
    //Vulkan initialization
    static void InitVulkan();

    static void CleanupVulkan();

private:
    //Creates instance
    static void CreateInstance();
    static std::vector<VkExtensionProperties> GetSupportedExtensions();
    static void OutputExtensions(std::vector<VkExtensionProperties> extensions);
    static std::vector<const char*> GetRequiredExtensions();

    static void SetupDebugMessenger();
    static void FillMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static bool CheckValidationLayerSupport();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    //Vulkan variables
    static VkInstance m_instance;
    static VkDebugUtilsMessengerEXT m_debugMessenger;

    //Validation layers variables
    static const std::vector<const char*> m_validationLayers;
    static const bool m_enableValidationLayers;
};