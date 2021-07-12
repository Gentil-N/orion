#pragma once
#ifndef __VULKAN_HPP__
#define __VULKAN_HPP__

#include "../code/common.hpp"

#define VK_NO_PROTOTYPES
#ifdef LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#endif //LINUX

#include "vulkan-headers/include/vulkan/vulkan.h"

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "vulkan-headers/include/vulkan/vulkan.hpp"

#endif //__VULKAN_HPP__