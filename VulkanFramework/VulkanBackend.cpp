#include "VulkanBackend.h"

VulkanBackend* VulkanBackend::m_singletonInst = nullptr;

VulkanBackend* VulkanBackend::GetInstance()
{
    if (m_singletonInst == nullptr)
    {
        m_singletonInst = new VulkanBackend();
    }
    
    return m_singletonInst;
}

void VulkanBackend::CleanupInstance()
{
    if (m_singletonInst != nullptr)
    {
        delete m_singletonInst;
        m_singletonInst = nullptr;
    }
}

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
    if (m_instance != nullptr)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = nullptr;
    }

    CleanupInstance();
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
    } else {
        createInfo.enabledLayerCount = 0;
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

bool VulkanBackend::IsDeviceSuitable(VkPhysicalDevice device)
{
    //Gets device supported features. 
    //We don't really need to use this right now
    VkPhysicalDeviceProperties deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceFeatures);

    //Just say it's suitable
    return true;
}

void VulkanBackend::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPU with Vulkan Support!!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }


}
