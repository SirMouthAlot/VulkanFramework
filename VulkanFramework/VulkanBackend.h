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
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>

#include "VulkanImport.h"
#include "Util.h"

struct QueueFamilyIndices
{
    std::optional<uint32_t> m_graphicsFamily;
    std::optional<uint32_t> m_presentFamily;

    bool IsComplete()
    {
        return m_graphicsFamily.has_value() && m_presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR m_capabilities;
    std::vector<VkSurfaceFormatKHR> m_formats;
    std::vector<VkPresentModeKHR> m_presentModes;
};

class VulkanBackend
{
public:
    //Get singleton vulkan backend instance
    static VulkanBackend* GetInstance();
    
    //Vulkan initialization
    void InitVulkan(GLFWwindow* window, const int width, const int height);

    void DrawFrame();
    void WaitForIdle();

    void CleanupVulkan();

    const int MAX_FRAMES_IN_FLIGHT = 2;

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
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    void CreateLogicalDevice();
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

    //Rendering setup
    void CreateSurface(GLFWwindow* window);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const int width, const int height);
    void CreateSwapChain(const int width, const int height);

    //Imaging
    void CreateImageViews();

    //Graphics Pipeline
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    void CreateRenderPass();
    void CreateGraphicsPipeline();

    //Framebuffers
    void CreateFramebuffers();

    //Command stuff
    void CreateCommandPool();
    void CreateCommandBuffers();
    
    //Sephamore stuffs
    void CreateSyncObjects();

    //Singleton instance
    static VulkanBackend* m_singletonInst;

    //Vulkan Variables
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    const std::vector<const char*> m_validationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> m_deviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_swapChainImageViews;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
    size_t m_currentFrame = 0;

#ifdef NDEBUG
    const bool m_enableValidationLayers = false;
#else
    const bool m_enableValidationLayers = true;
#endif //!NDEBUG
};

#endif // !__VULKAN_BACKEND_H__