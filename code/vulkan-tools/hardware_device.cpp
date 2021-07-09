#include "vulkan_tools.hpp"

namespace vkt
{
       bool check_physical_device_extensions(const std::vector<VkExtensionProperties> &extensions, const char **required_extensions, uint32_t extension_count)
       {
              for (uint32_t i = 0; i < extension_count; ++i)
              {
                     const char *required_extension = required_extensions[i];
                     bool found = false;
                     for (uint32_t j = 0; j < extensions.size(); ++j)
                     {
                            if (strcmp(extensions[j].extensionName, required_extension) == 0)
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

       bool check_for_queue(const std::vector<VkQueueFamilyProperties> &queue_families, VkQueueFlagBits queue_type)
       {
              for (size_t i = 0; i < queue_families.size(); ++i)
              {
                     const VkQueueFamilyProperties &queue_props = queue_families[i];
                     if ((queue_props.queueFlags & queue_type) == queue_type && queue_props.queueCount > 0)
                     {
                            return true;
                     }
              }
              return false;
       }

       bool HardwareDevice::is_swapchain_extension_supported(Driver &driver, VkSurfaceKHR vk_surface)
       {
              for (size_t i = 0; i < queue_families.size(); ++i)
              {
                     VkBool32 supported = VK_FALSE;
                     vkt_assert(driver.func.vk_get_physical_device_surface_support_khr(vk_physical_device, i, vk_surface, &supported));
                     if (supported == VK_TRUE)
                     {
                            std::vector<const char *> swapchain_extension = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
                            return check_physical_device_extensions(extensions, swapchain_extension.data(), swapchain_extension.size());
                     }
              }
              return false;
       }

       bool HardwareDevice::has_graphics_queue()
       {
              return check_for_queue(queue_families, VK_QUEUE_GRAPHICS_BIT);
       }

       bool HardwareDevice::has_compute_queue()
       {
              return check_for_queue(queue_families, VK_QUEUE_COMPUTE_BIT);
       }

       bool HardwareDevice::has_transfer_queue()
       {
              return check_for_queue(queue_families, VK_QUEUE_TRANSFER_BIT);
       }
}