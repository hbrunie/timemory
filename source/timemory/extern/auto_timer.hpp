//  MIT License
//
//  Copyright (c) 2019, The Regents of the University of California,
// through Lawrence Berkeley National Laboratory (subject to receipt of any
// required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

/** \file auto_timer.hpp
 * \headerfile auto_timer.hpp "timemory/extern/auto_timer.hpp"
 * Extern template declarations that include CUDA
 *
 */

#pragma once

#include "timemory/components.hpp"
#include "timemory/utility/macros.hpp"
#include "timemory/variadic/auto_hybrid.hpp"
#include "timemory/variadic/auto_list.hpp"
#include "timemory/variadic/auto_tuple.hpp"

//--------------------------------------------------------------------------------------//
// auto_timer
//
#if defined(TIMEMORY_EXTERN_TEMPLATES) && !defined(TIMEMORY_EXTERN_TEMPLATE_BUILD)

// clang-format off

TIMEMORY_DECLARE_EXTERN_TUPLE(minimal_auto_timer_t,
                              ::tim::component::wall_clock,
                              ::tim::component::cpu_clock,
                              ::tim::component::cpu_util,
                              ::tim::component::peak_rss)

TIMEMORY_DECLARE_EXTERN_LIST(minimal_auto_timer_t,
                             ::tim::component::caliper,
                             ::tim::component::papi_array_t,
                             ::tim::component::cuda_event,
                             ::tim::component::nvtx_marker,
                             ::tim::component::cupti_activity,
                             ::tim::component::cupti_counters)

TIMEMORY_DECLARE_EXTERN_HYBRID(minimal_auto_timer_t)

TIMEMORY_DECLARE_EXTERN_TUPLE(auto_timer_t,
                              ::tim::component::wall_clock,
                              ::tim::component::system_clock,
                              ::tim::component::user_clock,
                              ::tim::component::cpu_util,
                              ::tim::component::peak_rss)

TIMEMORY_DECLARE_EXTERN_LIST(auto_timer_t,
                             ::tim::component::gperf_cpu_profiler,
                             ::tim::component::gperf_heap_profiler,
                             ::tim::component::caliper,
                             ::tim::component::papi_array_t,
                             ::tim::component::cpu_roofline_sp_flops,
                             ::tim::component::cpu_roofline_dp_flops,
                             ::tim::component::cuda_event,
                             ::tim::component::nvtx_marker,
                             ::tim::component::cupti_activity,
                             ::tim::component::cupti_counters,
                             ::tim::component::gpu_roofline_flops,
                             ::tim::component::gpu_roofline_hp_flops,
                             ::tim::component::gpu_roofline_sp_flops,
                             ::tim::component::gpu_roofline_dp_flops)

TIMEMORY_DECLARE_EXTERN_HYBRID(auto_timer_t)

// clang-format on

#endif

//======================================================================================//