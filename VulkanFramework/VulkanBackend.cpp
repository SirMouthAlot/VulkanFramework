#include "VulkanBackend.h"

//Vulkan variables
VkInstance VulkanBackend::m_instance = VK_NULL_HANDLE;
VkDebugUtilsMessengerEXT VulkanBackend::m_debugMessenger;

const std::vector<const char*> VulkanBackend::m_validationLayers = 
{
        "VK_LAYER_KHRONOS_validation"
};
#ifdef NDEBUG
    const bool VulkanBackend::m_enableValidationLayers = false;
#else
    const bool VulkanBackend::m_enableValidationLayers = true;
#endif //!NDEBUG

void VulkanBackend::InitVulkan()
{
	//Creates Vulkan instance!!!
	CreateInstance();
	//Prints out supported extensions
	OutputExtensions(GetSupportedExtensions());

	//Sets up the debug messenger
	SetupDebugMessenger();
}

void VulkanBackend::CleanupVulkan()
{
    if (m_enableValidationLayers)
    {
        VulkanImport::DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    //Cleans up after vk instance
    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

void VulkanBackend::CreateInstance()
{
    //Check if validation layers requested are available
    if (m_enableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    auto extensions = GetRequiredExtensions();

    //This is technically optional, but provides info that can be used to optimize things.
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //This is not optional, tells the driver what global extensions and validation layers to be used.
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<int32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (m_enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();

        FillMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    //Makes sure that the instance was sucessfully created
    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vulkan instance");
    }
}

std::vector<VkExtensionProperties> VulkanBackend::GetSupportedExtensions()
{
    //Extension count
    uint32_t extensionCount = 0;
    //Gets the supported extension count
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    //Vector of extensions
    std::vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    return extensions;
}

void VulkanBackend::OutputExtensions(std::vector<VkExtensionProperties> extensions)
{
    std::cout << "available extensions:" << std::endl;

    for (const auto& extension : extensions)
    {
        std::cout << "\t" << extension.extensionName << std::endl;
    }
}

std::vector<const char*> VulkanBackend::GetRequiredExtensions()
{
    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

    if (m_enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanBackend::SetupDebugMessenger()
{
    if (!m_enableValidationLayers) return;

    //Fill in the details about the messenger itself
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    FillMessengerCreateInfo(createInfo);

    if (VulkanImport::CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to set up debug messenger");
    }
}

void VulkanBackend::FillMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

bool VulkanBackend::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanBackend::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
