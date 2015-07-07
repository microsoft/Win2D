// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once


// Each test is repeated a number of times, and the results averaged to ensure stable timing.
const int TestPasses = 100;

// The inner loop within each test pass is repeated a much larger number of times.
const int InnerRepetitions = 1000000;

// Each inner loop repetition reads from a different parameter value, cycling through a set of this size.
// We want this big enough to exercise memory -> SIMD register load paths (so the entire test cannot be
// enregistered) but not so big as to spill out of cache and introduce unpredictable memory latencies.
const int ParamCount = 64;


// The core thing being measured: repeats a simple operation a large number of times.
// Marked as noinline to encourage inlining of the operation lambda, and to
// keep this as separate as possible from the surrounding infrastructure goop.
template<typename TValue, typename TParams, typename TOperation>
__declspec(noinline) void RunInnerLoop(TValue* value, TParams& params, TOperation const& operation)
{
    for (int i = 0; i < InnerRepetitions; i++)
    {
        operation(value, params[i % ParamCount]);
    }
}


// Runs a single test pass, returning how long it took.
template<typename TValue, typename TParam, typename TOperation>
double RunTestPass(TOperation const& operation)
{
    // Generate a random (but identical for every pass) starting value and parameter list.
    srand(1);

    auto value = MakeRandom<TValue>();
    auto params = MakeRandom<TParam, ParamCount>();
    
    // Run the test, and time how long it takes.
    LARGE_INTEGER startTime;
    QueryPerformanceCounter(&startTime);

    RunInnerLoop(&value, params, operation);

    LARGE_INTEGER endTime;
    QueryPerformanceCounter(&endTime);

    // Make sure the compiler doesn't try to optimize out our computation!
    EnsureNotOptimizedAway(value);

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    return static_cast<double>(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
}


// Analyzes a collection of test pass results to make sure the timing is reasonably stable.
template<typename T>
double GetDeviationPercentage(T const& results)
{
    // Discard the top and bottom 10% of the results.
    auto begin = results.begin() + results.size() / 10;
    auto end = results.end() - results.size() / 10;

    // Compute the mean.
    auto count = std::distance(begin, end);
    double mean = std::accumulate(begin, end, 0.0) / count;

    // Compute standard deviation.
    double sum = std::accumulate(begin, end, 0.0, [&](double previous, double value)
    {
        return previous + (value - mean) * (value - mean);
    });

    double variance = sum / count;
    double deviation = sqrt(variance);

    // Return deviation as a percentage of the mean.
    return deviation / mean * 100;
}


// The main test entrypoint.
template<typename TValue, typename TParam, typename TOperation>
__declspec(noinline) void RunPerfTest(std::string const& testName, TOperation const& operation)
{
    // Repeat the test multiple times.
    std::array<double, TestPasses> results;

    std::generate(results.begin(), results.end(), [&]
    {
        return RunTestPass<TValue, TParam>(operation);
    });

    // Analyze the results.
    std::sort(results.begin(), results.end());

    auto median = results[TestPasses / 2];
    auto deviation = GetDeviationPercentage(results);

    printf("%s, %f, %f%%\n", testName.c_str(), median, deviation);
}
