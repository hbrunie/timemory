//  MIT License
//
//  Copyright (c) 2018, The Regents of the University of California,
//  through Lawrence Berkeley National Laboratory (subject to receipt of any
//  required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdio.h>
#include <string>

#include "timemory/clocks.hpp"
#include "timemory/macros.hpp"
#include "timemory/serializer.hpp"

//============================================================================//

namespace tim
{
//----------------------------------------------------------------------------//

namespace timing
{
//----------------------------------------------------------------------------//

enum record_type
{
    REALTIME,
    SYSTEM,
    USER,
    MONOTONIC,
    MONOTONIC_RAW,
    THREAD_CPUTIME,
    PROCESS_CPUTIME
};

//----------------------------------------------------------------------------//

template <typename _Tp, typename value_type = intmax_t>
struct base
{
    using ratio_t    = std::micro;
    using Type       = _Tp;
    value_type value = value_type(0);

    base(value_type _value = value_type(0))
    : value(_value)
    {
    }

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar(serializer::make_nvp(Type::label(), value),
           serializer::make_nvp("units", Type::unit));
    }

    value_type operator()() { return (value = Type::record()); }
    value_type start() { return (*this)(); }
    value_type stop() { return (*this)(); }

    _Tp& max(const base<_Tp>& rhs) { return (value = std::max(value, rhs.value)); }
    _Tp  max(const base<_Tp>& rhs) const { return std::max(value, rhs.value); }
    _Tp& min(const base<_Tp>& rhs) { return (value = std::min(value, rhs.value)); }
    _Tp  min(const base<_Tp>& rhs) const { return std::min(value, rhs.value); }

    _Tp& operator+=(const base<_Tp>& rhs)
    {
        value += rhs.value;
        return static_cast<_Tp&>(*this);
    }
    _Tp& operator-=(const base<_Tp>& rhs)
    {
        value -= rhs.value;
        return static_cast<_Tp&>(*this);
    }

    friend _Tp operator+(const base<_Tp>& lhs, const base<_Tp>& rhs)
    {
        return base<_Tp>(lhs) += rhs;
    }

    friend _Tp operator-(const base<_Tp>& lhs, const base<_Tp>& rhs)
    {
        return base<_Tp>(lhs) -= rhs;
    }

    _Tp& operator+=(const value_type& rhs)
    {
        value += rhs;
        return static_cast<_Tp&>(*this);
    }
    _Tp& operator-=(const value_type& rhs)
    {
        value -= rhs;
        return static_cast<_Tp&>(*this);
    }
    _Tp& operator*=(const value_type& rhs)
    {
        value *= rhs;
        return static_cast<_Tp&>(*this);
    }
    _Tp& operator/=(const value_type& rhs)
    {
        value /= rhs;
        return static_cast<_Tp&>(*this);
    }

    friend std::ostream& operator<<(std::ostream& os, const base<_Tp>& ru)
    {
        std::stringstream ss, ssv, ssi;
        auto tmp = static_cast<double>(ru.value / static_cast<double>(ratio_t::den));
        ss << "    > ";
        ssv << std::setprecision(3) << std::setw(8) << std::fixed << tmp;
        ssi << " " << std::setw(8) << std::left << _Tp::label();
        ss << ssv.str() << ssi.str() << " [sec]";
        os << ss.str();
        return os;
    }
};

//----------------------------------------------------------------------------//

struct realtime_clock : public base<realtime_clock>
{
    static const record_type category = REALTIME;
    static std::string       label() { return "real"; }
    static std::string       descript() { return "wall time"; }
    static const intmax_t    unit = units::usec;
    static double record() { return tim::get_clock_realtime_now<intmax_t, ratio_t>(); }
};

//----------------------------------------------------------------------------//

struct system_clock : public base<system_clock>
{
    static const record_type category = SYSTEM;
    static std::string       label() { return "sys"; }
    static std::string       descript() { return "system time"; }
    static const intmax_t    unit = units::usec;
    static double record() { return tim::get_clock_system_now<intmax_t, ratio_t>(); }
};

//----------------------------------------------------------------------------//

struct user_clock : public base<user_clock>
{
    static const record_type category = USER;
    static std::string       label() { return "user"; }
    static std::string       descript() { return "user time"; }
    static const intmax_t    unit = units::usec;
    static double record() { return tim::get_clock_monotonic_now<intmax_t, ratio_t>(); }
};

//----------------------------------------------------------------------------//

struct monotonic_clock : public base<monotonic_clock>
{
    static const record_type category = USER;
    static std::string       label() { return "mono"; }
    static std::string       descript() { return "monotonic time"; }
    static const intmax_t    unit = units::usec;
    static double record() { return tim::get_clock_monotonic_now<intmax_t, ratio_t>(); }
};

//----------------------------------------------------------------------------//

struct monotonic_raw_clock : public base<monotonic_raw_clock>
{
    static const record_type category = USER;
    static std::string       label() { return "raw_mono"; }
    static std::string       descript() { return "monotonic raw time"; }
    static const intmax_t    unit = units::usec;
    static double            record()
    {
        return tim::get_clock_monotonic_raw_now<intmax_t, ratio_t>();
    }
};

//----------------------------------------------------------------------------//

struct thread_cpu_clock : public base<thread_cpu_clock>
{
    static const record_type category = USER;
    static std::string       label() { return "thr_cpu"; }
    static std::string       descript() { return "thread cpu time"; }
    static const intmax_t    unit = units::usec;
    static double record() { return tim::get_clock_thread_now<intmax_t, ratio_t>(); }
};

//----------------------------------------------------------------------------//

struct process_cpu_clock : public base<process_cpu_clock>
{
    static const record_type category = USER;
    static std::string       label() { return "proc_cpu"; }
    static std::string       descript() { return "process cpu time"; }
    static const intmax_t    unit = units::usec;
    static double record() { return tim::get_clock_process_now<intmax_t, ratio_t>(); }
};

//----------------------------------------------------------------------------//

typedef std::tuple<realtime_clock, system_clock, user_clock, monotonic_clock,
                   monotonic_raw_clock, thread_cpu_clock, process_cpu_clock>
    types_t;

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

template <typename _Tp>
struct max : public base<_Tp>
{
    max(base<_Tp>& obj) { obj.value = std::max(obj.value, _Tp::record()); }
};

//----------------------------------------------------------------------------//

template <typename _Tp>
struct record : public base<_Tp>
{
    record(base<_Tp>& obj) { obj(); }
    record(base<_Tp>& obj, const base<_Tp>& rhs) { obj += rhs.value; }
};

//----------------------------------------------------------------------------//

template <typename _Tp>
struct reset : public base<_Tp>
{
    reset(base<_Tp>& obj) { obj.value = 0; }
};

//----------------------------------------------------------------------------//

template <typename _Tp>
struct print : public base<_Tp>
{
    print(base<_Tp>& obj, std::ostream& os) { os << obj << std::endl; }
};

//----------------------------------------------------------------------------//

template <typename _Tp>
struct minus : public base<_Tp>
{
    minus(base<_Tp>& obj, const base<_Tp>& rhs) { obj.value -= rhs.value; }
    minus(base<_Tp>& obj, const intmax_t& rhs) { obj.value -= rhs; }
};

//----------------------------------------------------------------------------//

template <typename _Tp>
struct plus : public base<_Tp>
{
    plus(base<_Tp>& obj, const base<_Tp>& rhs) { obj.value += rhs.value; }
    plus(base<_Tp>& obj, const intmax_t& rhs) { obj.value += rhs; }
};

//----------------------------------------------------------------------------//

template <typename _Tp>
struct multiply : public base<_Tp>
{
    multiply(base<_Tp>& obj, const base<_Tp>& rhs) { obj.value *= rhs.value; }
    multiply(base<_Tp>& obj, const intmax_t& rhs) { obj.value *= rhs; }
};

//----------------------------------------------------------------------------//

template <typename _Tp>
struct divide : public base<_Tp>
{
    divide(base<_Tp>& obj, const base<_Tp>& rhs) { obj.value /= rhs.value; }
    divide(base<_Tp>& obj, const intmax_t& rhs) { obj.value /= rhs; }
};

//----------------------------------------------------------------------------//

template <typename _Tp, typename Archive>
struct serial : public base<_Tp>
{
    serial(base<_Tp>& obj, Archive& ar, const unsigned int)
    {
        ar(serializer::make_nvp(_Tp::label(), obj.value));
    }
};

//----------------------------------------------------------------------------//

}  // namespace timing

//----------------------------------------------------------------------------//

}  // namespace tim

//----------------------------------------------------------------------------//
