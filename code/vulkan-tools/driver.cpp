#include "vulkan_tools.hpp"

namespace vkt
{
       VKAPI_ATTR VkBool32 VKAPI_CALL vk_dbg_callback(
           VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
       {
              if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
              {
                     throw std::runtime_error(pCallbackData->pMessage);
              }
              return VK_FALSE;
       }

       bool check_instance_layers(const InstanceFunc &func, const char **required_layers, std::uint32_t layer_count)
       {
              if (!layer_count)
              {
                     return false;
              }
              std::uint32_t count = 0;
              vkt_assert(func.vk_enumerate_instance_layer_properties(&count, NULL));
              std::vector<VkLayerProperties> properties(count);
              vkt_assert(func.vk_enumerate_instance_layer_properties(&count, properties.data()));

              for (std::uint32_t i = 0; i < layer_count; ++i)
              {
                     const char *required_layer = required_layers[i];
                     bool found = false;
                     for (size_t j = 0; j < count; ++j)
                     {
                            if (strcmp(properties[j].layerName, required_layer) == 0)
                            {
                                   found = true;
                                   break;
                            }
                     }
                     if (!found)
                     {
                            return false;
                     }
              }
              return true;
       }

       bool check_instance_extensions(const InstanceFunc &func, const char **required_extensions, std::uint32_t extensions_count)
       {
              if (!extensions_count)
              {
                     return false;
              }
              std::uint32_t count = 0;
              vkt_assert(func.vk_enumerate_instance_extension_properties(NULL, &count, NULL));
              std::vector<VkExtensionProperties> properties(count);
              vkt_assert(func.vk_enumerate_instance_extension_properties(NULL, &count, properties.data()));

              for (std::uint32_t i = 0; i < extensions_count; ++i)
              {
                     const char *required_extension = required_extensions[i];
                     bool found = false;
                     for (size_t j = 0; j < count; ++j)
                     {
                            if (strcmp(properties[j].extensionName, required_extension) == 0)
                            {
                                   found = true;
                                   break;
                            }
                     }
                     if (!found)
                     {
                            return false;
                     }
              }
              return true;
       }

       bool Driver::load(bool debug_extension, bool surface_extension)
       {
#ifdef LINUX
              vulkan_dl = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
              if (!vulkan_dl)
              {
                     vulkan_dl = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
              }
#endif //LINUX
              if (!vulkan_dl)
              {
                     return false;
              }
#ifdef LINUX
              func.vk_get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_dl, "vkGetInstanceProcAddr");
#endif //LINUX
              if (!func.vk_get_instance_proc_addr)
              {
                     return false;
              }
              PFN_vkGetInstanceProcAddr load = func.vk_get_instance_proc_addr;
              func.vk_create_instance = (PFN_vkCreateInstance)load(NULL, "vkCreateInstance");
              func.vk_enumerate_instance_extension_properties = (PFN_vkEnumerateInstanceExtensionProperties)load(NULL, "vkEnumerateInstanceExtensionProperties");
              func.vk_enumerate_instance_layer_properties = (PFN_vkEnumerateInstanceLayerProperties)load(NULL, "vkEnumerateInstanceLayerProperties");
              func.vk_enumerate_instance_version = (PFN_vkEnumerateInstanceVersion)load(NULL, "vkEnumerateInstanceVersion");

              std::vector<const char *> layers;
              std::vector<const char *> extensions;
              if (debug_extension)
              {
                     layers.push_back("VK_LAYER_KHRONOS_validation");
                     extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
              }
              if (surface_extension)
              {
                     extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef LINUX
                     extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif //LINUX
              }
              if (!check_instance_layers(func, layers.data(), (std::uint32_t)layers.size()) || !check_instance_extensions(func, extensions.data(), (std::uint32_t)extensions.size()))
              {
                     return false;
              }
              vkt_assert(func.vk_enumerate_instance_version(&api_version));
              VkApplicationInfo app_info =
                  {VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "", VK_MAKE_VERSION(0, 0, 0), "", VK_MAKE_VERSION(0, 0, 0), api_version};
              VkInstanceCreateInfo instance_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &app_info, (uint32_t)layers.size(), layers.data(), (uint32_t)extensions.size(), extensions.data()};
              vkt_assert(func.vk_create_instance(&instance_info, nullptr, &vk_instance));

              func.vk_get_device_proc_addr = (PFN_vkGetDeviceProcAddr)load(vk_instance, "vkGetDeviceProcAddr");
              func.vk_destroy_instance = (PFN_vkDestroyInstance)load(vk_instance, "vkDestroyInstance");
              func.vk_enumerate_physical_devices = (PFN_vkEnumeratePhysicalDevices)load(vk_instance, "vkEnumeratePhysicalDevices");
              func.vk_get_physical_device_features = (PFN_vkGetPhysicalDeviceFeatures)load(vk_instance, "vkGetPhysicalDeviceFeatures");
              func.vk_get_physical_device_format_properties = (PFN_vkGetPhysicalDeviceFormatProperties)load(vk_instance, "vkGetPhysicalDeviceFormatProperties");
              func.vk_get_physical_device_image_format_properties = (PFN_vkGetPhysicalDeviceImageFormatProperties)load(vk_instance, "vkGetPhysicalDeviceImageFormatProperties");
              func.vk_get_physical_device_properties = (PFN_vkGetPhysicalDeviceProperties)load(vk_instance, "vkGetPhysicalDeviceProperties");
              func.vk_get_physical_device_queue_family_properties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)load(vk_instance, "vkGetPhysicalDeviceQueueFamilyProperties");
              func.vk_get_physical_device_memory_properties = (PFN_vkGetPhysicalDeviceMemoryProperties)load(vk_instance, "vkGetPhysicalDeviceMemoryProperties");
              func.vk_create_device = (PFN_vkCreateDevice)load(vk_instance, "vkCreateDevice");
              func.vk_enumerate_instance_extension_properties = (PFN_vkEnumerateInstanceExtensionProperties)load(vk_instance, "vkEnumerateInstanceExtensionProperties");
              func.vk_enumerate_device_extension_properties = (PFN_vkEnumerateDeviceExtensionProperties)load(vk_instance, "vkEnumerateDeviceExtensionProperties");
              func.vk_enumerate_device_layer_properties = (PFN_vkEnumerateDeviceLayerProperties)load(vk_instance, "vkEnumerateDeviceLayerProperties");
              func.vk_get_physical_device_sparse_image_format_properties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)load(vk_instance, "vkGetPhysicalDeviceSparseImageFormatProperties");
              func.vk_destroy_surface_khr = (PFN_vkDestroySurfaceKHR)load(vk_instance, "vkDestroySurfaceKHR");
              func.vk_get_physical_device_surface_support_khr = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)load(vk_instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
              func.vk_get_physical_device_surface_capabilities_khr = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)load(vk_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
              func.vk_get_physical_device_surface_formats_khr = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)load(vk_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
              func.vk_get_physical_device_surface_present_modes_khr = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)load(vk_instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
#ifdef LINUX
              func.vk_create_surface_khr = (PFN_vkCreateXlibSurfaceKHR)load(vk_instance, "vkCreateXlibSurfaceKHR");
#endif //LINUX
              func.vk_create_debug_utils_messenger_ext = nullptr;
              func.vk_destroy_debug_utils_messenger_ext = nullptr;

              if (debug_extension)
              {
                     func.vk_create_debug_utils_messenger_ext = (PFN_vkCreateDebugUtilsMessengerEXT)load(vk_instance, "vkCreateDebugUtilsMessengerEXT");
                     func.vk_destroy_debug_utils_messenger_ext = (PFN_vkDestroyDebugUtilsMessengerEXT)load(vk_instance, "vkDestroyDebugUtilsMessengerEXT");
                     VkDebugUtilsMessengerCreateInfoEXT debug_info =
                         {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT, nullptr, 0,
                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                          VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                          vk_dbg_callback, nullptr};
                     vkt_assert(func.vk_create_debug_utils_messenger_ext(vk_instance, &debug_info, nullptr, &vk_debug_utils_messenger));
              }
              {
                     std::uint32_t count = 0;
                     func.vk_enumerate_physical_devices(vk_instance, &count, nullptr);
                     std::vector<VkPhysicalDevice> physical_devices(count);
                     func.vk_enumerate_physical_devices(vk_instance, &count, physical_devices.data());
                     hardware_devices.resize(count);
                     for (uint32_t i = 0; i < count; ++i)
                     {
                            hardware_devices[i].vk_physical_device = physical_devices[i];
                            func.vk_get_physical_device_properties(physical_devices[i], &hardware_devices[i].vk_properties);
                            func.vk_get_physical_device_features(physical_devices[i], &hardware_devices[i].vk_features);
                            std::uint32_t family_count = 0;
                            func.vk_get_physical_device_queue_family_properties(physical_devices[i], &family_count, nullptr);
                            hardware_devices[i].queue_families.resize(family_count);
                            func.vk_get_physical_device_queue_family_properties(physical_devices[i], &family_count, hardware_devices[i].queue_families.data());
                            std::uint32_t ext_count = 0;
                            vkt_assert(func.vk_enumerate_device_extension_properties(physical_devices[i], NULL, &ext_count, NULL));
                            hardware_devices[i].extensions.resize(ext_count);
                            vkt_assert(func.vk_enumerate_device_extension_properties(physical_devices[i], NULL, &ext_count, hardware_devices[i].extensions.data()));
                     }
              }
              return true;
       }

       void Driver::unload()
       {
              if (vk_debug_utils_messenger != VK_NULL_HANDLE)
              {
                     func.vk_destroy_debug_utils_messenger_ext(vk_instance, vk_debug_utils_messenger, nullptr);
              }
              func.vk_destroy_instance(vk_instance, nullptr);
#ifdef LINUX
              dlclose(vulkan_dl);
#endif //LINUX
       }
}