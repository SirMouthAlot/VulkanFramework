#ifndef __VULKAN_BACKEND_H__
#define __VULKAN_BACKEND_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include <vector>
#include <cstring>

#include "VulkanImport.h"

class VulkanBackend
{
public:
    //Get singleton vulkan backend instance
    static VulkanBackend* GetInstance();
    
    //Vulkan initialization
    void InitVulkan();
    void CleanupVulkan();

private:
    //Singleton setup
    VulkanBackend() { };
    VulkanBackend(VulkanBackend&) = delete;
    //Cleans up singleton
    static void CleanupInstance();

    //Instance and extension stuff
    void CreateInstance();
    std::vector<VkExtensionProperties> GetSupportedExtensions();
    void OutputExtensions(std::vector<VkExtensionProperties> extensions);
    std::vector<const char*> GetRequiredExtensions();

    //Debug and validation layers
    void SetupDebugMessenger();
    void FillMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    bool CheckValidationLayerSupport();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    //Device picking
    bool IsDeviceSuitable(VkPhysicalDevice device);
    void PickPhysicalDevice();

    //Singleton instance
    static VulkanBackend* m_singletonInst;

    //Vulkan variables
    VkInstance m_instance = nullptr;
    VkDebugUtilsMessengerEXT m_debugMessenger;

    //Validation layers variables
    const std::vector<const char*> m_validationLayers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };
#ifdef NDEBUG
    const bool m_enableValidationLayers = false;
#else
    const bool m_enableValidationLayers = true;
#endif //!NDEBUG

    //Device variables
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
};

#endif // !__VULKAN_BACKEND_H__