// MIT License
//
// Copyright (c) 2019, The Regents of the University of California,
// through Lawrence Berkeley National Laboratory (subject to receipt of any
// required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/** \file backends/nvtx.hpp
 * \headerfile backends/nvtx.hpp "timemory/backends/nvtx.hpp"
 * Defines NVTX markers
 *
 */

#pragma once

#include "timemory/settings.hpp"
#include "timemory/utility/macros.hpp"

#include <cstdint>
#include <initializer_list>
#include <string>
#include <vector>

#if defined(TIMEMORY_USE_NVTX)
#    include <nvtx3/nvToolsExt.h>
#endif

#if defined(_LINUX)
#    include <sys/syscall.h>
#    include <unistd.h>
#elif defined(_WINDOWS)
#    include <processthreadsapi.h>
#elif defined(_MACOS)
#    include <atomic>
#endif

namespace tim
{
namespace nvtx
{
//--------------------------------------------------------------------------------------//
inline uint32_t
get_thread_id()
{
#if defined(_LINUX)
    return syscall(SYS_gettid);
#elif defined(_WINDOWS)
    return GetCurrentThreadId();
#else
    static std::atomic<uint32_t> _thread_counter;
    static thread_local uint32_t _thread_id = _thread_counter++;
    return _thread_id;
#endif
}
//--------------------------------------------------------------------------------------//

template <typename... _Args>
void
consume_parameters(_Args&&...)
{}
//--------------------------------------------------------------------------------------//

namespace color
{
//--------------------------------------------------------------------------------------//
using color_t       = uint32_t;
using color_array_t = std::vector<color_t>;
//--------------------------------------------------------------------------------------//

static const color_t red         = 0xffff0000;
static const color_t blue        = 0xff0000ff;
static const color_t green       = 0xff00ff00;
static const color_t yellow      = 0xffffff00;
static const color_t purple      = 0xffff00ff;
static const color_t cyan        = 0xff00ffff;
static const color_t pink        = 0xff00ffff;
static const color_t light_green = 0xff99ff99;

//--------------------------------------------------------------------------------------//

inline color_array_t&
available()
{
    static color_array_t _instance = { red,    blue, green, yellow,
                                       purple, cyan, pink,  light_green };
    return _instance;
}

//--------------------------------------------------------------------------------------//
}  // namespace color

//--------------------------------------------------------------------------------------//

#if defined(TIMEMORY_USE_NVTX)
using event_attributes_t = nvtxEventAttributes_t;
using range_id_t         = nvtxRangeId_t;
#else
#    if !defined(NVTX_VERSION)
#        define NVTX_VERSION 0
#    endif

#    if !defined(NVTX_EVENT_ATTRIB_STRUCT_SIZE)
#        define NVTX_EVENT_ATTRIB_STRUCT_SIZE 0
#    endif

#    if !defined(NVTX_COLOR_ARGB)
#        define NVTX_COLOR_ARGB 0
#    endif

#    if !defined(NVTX_MESSAGE_TYPE_ASCII)
#        define NVTX_MESSAGE_TYPE_ASCII 0
#    endif

//--------------------------------------------------------------------------------------//

using range_id_t = uint32_t;

//--------------------------------------------------------------------------------------//

struct message_t
{
    std::string ascii = "";
};

//--------------------------------------------------------------------------------------//

struct event_attributes_t
{
    uint32_t version = NVTX_VERSION;
    uint32_t size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
    uint32_t colorType = NVTX_COLOR_ARGB;
    uint32_t color = 0xff00ff;
    uint32_t messageType = NVTX_MESSAGE_TYPE_ASCII;
    message_t message;

    event_attributes_t() = default;
    ~event_attributes_t() = default;
    event_attributes_t(const event_attributes_t&) = default;
    event_attributes_t(event_attributes_t&&) = default;
    event_attributes_t(int) {}

    event_attributes_t& operator=(const event_attributes_t&) = default;
    event_attributes_t& operator=(event_attributes_t&&) = default;
    event_attributes_t& operator=(const std::initializer_list<int32_t>&) { return *this; }
};

#endif

//--------------------------------------------------------------------------------------//

inline event_attributes_t
init_marker(const std::string& _msg, color::color_t _color = 0)
{
    static thread_local color::color_t _counter = 0;
    event_attributes_t                 attrib   = {};
    attrib.version                              = NVTX_VERSION;
    attrib.size                                 = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
    attrib.messageType                          = NVTX_MESSAGE_TYPE_ASCII;
    attrib.colorType                            = NVTX_COLOR_ARGB;
    attrib.message.ascii                        = _msg.c_str();
    attrib.color                                = (_color == 0)
                       ? (color::available().at((_counter++) % color::available().size()))
                       : _color;
    if(settings::debug())
    {
        std::stringstream ss;
        ss << "[nvtx]> initializing marker with label \"" << _msg << "\" and "
           << "color " << std::hex << attrib.color << "...\n";
        std::cout << ss.str() << std::flush;
    }
    return attrib;
}

//--------------------------------------------------------------------------------------//

inline void
name_thread(const std::string& _msg)
{
#if defined(TIMEMORY_USE_NVTX)
    nvtxNameOsThread(get_thread_id(), _msg.c_str());
#else
    consume_parameters(_msg);
#endif
}

//--------------------------------------------------------------------------------------//

inline void
name_thread(const int32_t& _id)
{
#if defined(TIMEMORY_USE_NVTX)
    std::stringstream ss;
    if(_id == 0)
        ss << "MASTER";
    else
        ss << "WORKER_" << _id;
    nvtxNameOsThread(get_thread_id(), ss.str().c_str());
#else
    consume_parameters(_id);
#endif
}

//--------------------------------------------------------------------------------------//

inline void
range_push(const std::string& _msg)
{
#if defined(TIMEMORY_USE_NVTX)
    nvtxRangePush(_msg.c_str());
#else
    consume_parameters(_msg);
#endif
}

//--------------------------------------------------------------------------------------//

inline void
range_push(event_attributes_t& _attrib)
{
#if defined(TIMEMORY_USE_NVTX)
    nvtxRangePushEx(&_attrib);
#else
    consume_parameters(_attrib);
#endif
}

//--------------------------------------------------------------------------------------//

inline void
range_pop()
{
#if defined(TIMEMORY_USE_NVTX)
    nvtxRangePop();
#endif
}

//--------------------------------------------------------------------------------------//

inline range_id_t
range_start(const event_attributes_t& _attrib)
{
#if defined(TIMEMORY_USE_NVTX)
    return nvtxRangeStartEx(&_attrib);
#else
    consume_parameters(_attrib);
    return 0;
#endif
}

//--------------------------------------------------------------------------------------//

inline void
range_stop(const range_id_t& _id)
{
#if defined(TIMEMORY_USE_NVTX)
    nvtxRangeEnd(_id);
#else
    consume_parameters(_id);
#endif
}

//--------------------------------------------------------------------------------------//

inline void
mark(const std::string& _msg)
{
#if defined(TIMEMORY_USE_NVTX)
    nvtxMarkA(_msg.c_str());
#else
    consume_parameters(_msg);
#endif
}

//--------------------------------------------------------------------------------------//

inline void
mark(const event_attributes_t& _attrib)
{
#if defined(TIMEMORY_USE_NVTX)
    nvtxMarkEx(&_attrib);
#else
    consume_parameters(_attrib);
#endif
}

//--------------------------------------------------------------------------------------//

}  // namespace nvtx
}  // namespace tim
