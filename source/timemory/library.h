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

#pragma once

#include "timemory/compat/library.h"

//--------------------------------------------------------------------------------------//
//
#if defined(__cplusplus)

// for generating names
#    include "timemory/variadic/macros.hpp"

//--------------------------------------------------------------------------------------//

struct timemory_scoped_record
{
    timemory_scoped_record(const char* name)
    : m_nid(timemory_get_begin_record(name))
    {}

    timemory_scoped_record(const char* name, const char* components)
    : m_nid(timemory_get_begin_record_types(name, components))
    {}

    template <typename... _Ids,
              typename std::enable_if<(sizeof...(_Ids) > 0), int>::type = 0>
    timemory_scoped_record(const char* name, _Ids... _ids)
    : m_nid(timemory_get_begin_record_enum(name, _ids..., TIMEMORY_COMPONENTS_END))
    {}

    ~timemory_scoped_record() { timemory_end_record(m_nid); }

private:
    uint64_t m_nid = std::numeric_limits<uint64_t>::max();
};

//--------------------------------------------------------------------------------------//

template <typename _Tp>
inline _Tp&
timemory_tl_static(const _Tp& _initial = {})
{
    static thread_local _Tp _instance = _initial;
    return _instance;
}

//--------------------------------------------------------------------------------------//

#endif  // if defined(__cplusplus)
//
//--------------------------------------------------------------------------------------//
