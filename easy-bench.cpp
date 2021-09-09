//
// Created by maratworker on 09.09.2021.
//

#include <benchmark/benchmark.h>

static void BM_EasyFunction(benchmark::State& state) {
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        [](){
            double f_multi = 0.0;
            for (int i = 0; i < 1000000; ++i) {
                f_multi *= i;
            }
        };
    }
}
// Register the function as a benchmark
BENCHMARK(BM_EasyFunction);
// Run the benchmark
BENCHMARK_MAIN();