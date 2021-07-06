#pragma once
#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#define ORION "Orion"
#define ORION_VERSION_MAJOR 1
#define ORION_VERSION_MINOR 0

#if defined(__linux__) && !defined(__ANDROID__)
#define LINUX
#include <dlfcn.h>
#include <X11/Xlib.h>
#else
#error "no other platforms supported yet"
#endif

/*STD*/
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <array>
#include <fstream>
#include <functional>
#include <utility>
#include <exception>
#include <limits>
#include <iostream>

/*UTILS*/
#include "utils/node.hpp"

#endif //__COMMON_HPP__