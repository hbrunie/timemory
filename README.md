# timemory

## Timing + Memory + Hardware Counter Utilities for C / C++ / CUDA / Python

[![Build Status](https://travis-ci.org/NERSC/timemory.svg?branch=master)](https://travis-ci.org/NERSC/timemory)
[![Build status](https://ci.appveyor.com/api/projects/status/8xk72ootwsefi8c1/branch/master?svg=true)](https://ci.appveyor.com/project/jrmadsen/timemory/branch/master)
[![codecov](https://codecov.io/gh/NERSC/timemory/branch/master/graph/badge.svg)](https://codecov.io/gh/NERSC/timemory)

[![Conda Recipe](https://img.shields.io/badge/recipe-timemory-green.svg)](https://anaconda.org/jrmadsen/timemory)
[![Anaconda-Server Badge](https://anaconda.org/jrmadsen/timemory/badges/version.svg)](https://anaconda.org/jrmadsen/timemory)
[![Anaconda-Server Badge](https://anaconda.org/jrmadsen/timemory/badges/platforms.svg)](https://anaconda.org/jrmadsen/timemory)
[![Anaconda-Server Badge](https://anaconda.org/jrmadsen/timemory/badges/downloads.svg)](https://anaconda.org/jrmadsen/timemory)

[timemory on GitHub (Source code)](https://github.com/NERSC/timemory)

[timemory General Documentation](https://timemory.readthedocs.io)

[timemory Source Code Documentation (Doxygen)](https://timemory.readthedocs.io/en/latest/doxygen-docs/)

[timemory Testing Dashboard (CDash)](https://cdash.nersc.gov/index.php?project=TiMemory)

|                |                                                   |
| -------------- | ------------------------------------------------- |
| GitHub         | `git clone https://github.com/NERSC/timemory.git` |
| PyPi           | `pip install timemory`                            |
| Anaconda Cloud | `conda install -c jrmadsen timemory`              |

## Why Use timemory?

__*Timemory is arguably the most customizable performance analysis and tuning API available while maintaining a very low overhead.*__

- __*Direct access*__ to performance analysis data in Python and C++
- Variadic interface to all the utilities from C code
- Variadic interface to all the utilities from C++ code
- Variadic interface to all the utilities from Python code
    - Includes context-managers and decorators
- __*Create your own components*__: any one-time measurement or start/stop paradigm can be wrapped with timemory
    - Flexible and easily extensible interface: __*no data type restrictions in custom components*__
- __*High-performance*__: template meta-programming and lambdas result in extensive inlining
- Ability to arbitrarily switch and combine different measurement types anywhere in application
- Provides static reporting (fixed at compile-time), dynamic reporting (selected at run-time), or hybrid
    - Enable static wall-clock and cpu-clock reporting with ability to dynamically enable hardware-counters at runtime
- Arbitrarily add support for:
    - __*CPU hardware counters*__ via PAPI
    - __*NVIDIA GPU hardware counters*__ via CUPTI
    - __*NVIDIA GPU tracing*__ via CUPTI
    - Generating a __*Roofline*__ for performance-critical sections on the CPU and NVIDIA GPUs
    - Marker forwarding to NVTX for Nsight-Systems and NVprof
    - Marker forwarding to [LIKWID](https://github.com/RRZE-HPC/likwid)
    - Marker forwarding to [Caliper](https://github.com/LLNL/Caliper)
        - Includes marker forwarding to [TAU](https://www.cs.uoregon.edu/research/tau/home.php)
        - Includes marker forwarding to Intel VTune and Advisor
    - Memory usage
    - Tool insertiong around `malloc`, `calloc`, `free`, `cudaMalloc`, `cudaFree`
        - Many more possible!
    - Wall-clock, cpu-clock, system-clock timing
    - Number of bytes read/written to file-system (and rate)
    - Number of context switches
    - Trip counts
    - CUDA kernel runtime(s)
    - [GOTCHA](https://github.com/LLNL/GOTCHA) wrappers around external library function calls

## Overview

Timemory is generic C++11 template library providing a variety of
[performance components](https://timemory.readthedocs.io/en/latest/components/overview/)
for reporting timing, resource usage, hardware counters for the CPU and GPU,
roofline generation, and simplified generation of GOTCHA wrappers to instrument
external library function calls.

Timemory provides also provides Python and C interfaces.

## Purpose

The goal of the package is to provide as easy way to regularly report on the performance
of your code. If you have ever added something like this in your code:

```python
tstart = time.now()
# do something
tstop = time.now()
print("Elapsed time: {}".format(tstop - tstart))
```

Timemory streamlines this work. In C++ codes, all you have to do is include the headers.
It comes in handy especially when optimizing a
certain algorithm or section of your code -- you just insert a line of code that specifies what
you want to measure and run your code: __*initialization and output are automated*__.

## Profiling and timemory

Timemory is not a full profiler and is intended to supplement profilers, not be used in lieu of profiling,
which are important for _discovering where to place timemory markers_.
The library provides an easy-to-use method for __*always-on general HPC analysis metrics*__
(i.e. timing, memory usage, etc.) with the same or less overhead than if these metrics were to
records and stored in a custom solution (there is zero polymorphism) and, for C++ code, extensively
inlined.
__*Functionally, the overhead is non-existant*__: sampling profilers (e.g. gperftools, VTune)
at standard sampling rates barely notice the presence of timemory unless it is been
used _very_ unwisely.

Additional tools are provided, such as hardware counters, to __*increase optimization productivity.*__
What to check whether those changes increased data locality (i.e. decreased cache misses) but don't care about any other sections of the code?
Use the following and set `TIMEMORY_PAPI_EVENTS="PAPI_L1_TCM,PAPI_L2_TCM,PAPI_L3_TCM"` in
the environment:

```cpp
using auto_tuple_t = tim::auto_tuple<tim::component::papi_array_t>;
TIMEMORY_CALIPER(roi, auto_tuple_t, "");
//
// do something in region of interest...
//
TIMEMORY_CALIPER_APPLY(roi, stop);
```

and delete it when finished. It's three extra LOC that may reduce the time
spent: changing code, then runnning profiler, then opening output in profiler,
then finding ROI, then comparing to previous results, and then repeating from
4 hours to 1.

In general, profilers are not run frequently enough and performance degradation
or memory bloat can go undetected for several commits until a production run crashes or
underperforms. This generally leads to a scramble to detect which revision caused the issue.
Here, timemory can __*decrease performance regression identification time.*__
When timemory is combined with a continuous integration reporting system,
this scramble can be mitigated fairly quickly because the high-level reporting
provided allows one to associate a region and commit with exact performance numbers.
Once timemory has been used to help identify the offending commit and identify the general
region in the offending code, a full profiler should be launched for the fine-grained diagnosis.

## Create Your Own Tools/Components

There are numerous instrumentation APIs available but very few provide the ability for _users_ to create
tools/components that will fully integrate with the instrumentation API in their code. The
simplicity of creating a custom component that inherits category-based formatting properties
(`is_timing_category`) and timing unit conversion (`uses_timing_units`)
can be easily demonstrated in ~50 LOC with the `wall_clock` component:

```cpp
namespace tim
{
namespace component { struct wall_clock; }

namespace trait
{
template <> struct is_timing_category<component::wall_clock> : std::true_type {};
template <> struct uses_timing_units<component::wall_clock> : std::true_type {};
}  // namespace trait

namespace component
{
//
// the system's real time (i.e. wall time) clock, expressed as the
// amount of time since the epoch.
//
struct wall_clock : public base<wall_clock, int64_t>
{
    using ratio_t    = std::nano;
    using value_type = int64_t;
    using base_type  = base<wall_clock, value_type>;

    static std::string label() { return "wall"; }
    static std::string description() { return "wall time"; }
    static value_type  record()
    {
        return tim::get_clock_real_now<int64_t, ratio_t>();
    }

    double get_display() const { return get(); }
    double get() const
    {
        auto val = (is_transient) ? accum : value;
        return static_cast<double>(val) / ratio_t::den * get_unit();
    }

    void start()
    {
        set_started();
        value = record();
    }

    void stop()
    {
        auto tmp = record();
        accum += (tmp - value);
        value = std::move(tmp);
        set_stopped();
    }
};

}  // namespace component
}  // namespace tim
```

## GOTCHA and timemory

C++ codes running on the Linux operating system can take advantage of the built-in
[GOTCHA](https://github.com/LLNL/GOTCHA) functionality to insert timemory markers __*around external function calls*__.
[GOTCHA](https://github.com/LLNL/GOTCHA) is similar to `LD_PRELOAD` but operates via a programmable API.
__*This include limited support for C++ function mangling*__ (in general, mangling template functions are not supported -- yet).

Writing a GOTCHA hook in timemory is greatly simplified and applications using timemory can specify their own GOTCHA hooks
in a few lines of code instead of being restricted to a pre-defined set of GOTCHA hooks.

### Example GOTCHA

If an application wanted to insert `tim::auto_timer` around (unmangled) `MPI_Allreduce` and
(mangled) `ext::do_work` in the following executable:

```cpp
#include <mpi.h>
#include <vector>

int main(int argc, char** argv)
{
    init();

    MPI_Init(&argc, &argv);

    int sizebuf = 100;
    std::vector<double> sendbuf(sizebuf, 1.0);
    // ... do some stuff
    std::vector<double> recvbuf(sizebuf, 0.0);

    MPI_Allreduce(sendbuf.data(), recvbuf.data(), sizebuf, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    // ... etc.

    int64_t nitr = 10;
    std::pair<float, double> settings{ 1.25f, 2.5 };
    std::tuple<float, double> result = ext::do_work(nitr, settings);
    // ... etc.

    return 0;
}
```

This would be the required specification using the `TIMEMORY_C_GOTCHA` macro for unmangled functions
and `TIMEMORY_CXX_GOTCHA` macro for mangled functions:

```cpp
#include <timemory/timemory.hpp>

static constexpr size_t NUM_FUNCS = 2;
using gotcha_t = tim::component::gotcha<NUM_FUNCS, tim::auto_timer_t>;

void init()
{
    TIMEMORY_C_GOTCHA(gotcha_t, 0, MPI_Allreduce);
    TIMEMORY_CXX_GOTCHA(gotcha_t, 1, ext::do_work);
}
```

## Additional Information

For more information, refer to the [documentation](https://timemory.readthedocs.io/en/latest/).
