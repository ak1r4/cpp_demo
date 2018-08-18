#include <iostream>
#include <thread>
#include <benchmark/benchmark.h>

void threaded_incr_vec(int* arr, unsigned int size, int thread_n) {
//    std::cout << "Thread: " << thread_n << std::endl;

    unsigned long long iterations = 10000000;

    while (--iterations) {
        for (unsigned int i = 0; i < size; i++) {
            --arr[i];
        }
    }
}


void demo(unsigned int vec_size) {
    int a1[vec_size];
    int a2[vec_size];

//    std::cout << "memory: [" << &a1[0] << "," << &a1[vec_size - 1] << "]"
//              << "[" << &a2[0] << "," << &a2[vec_size - 1] << "]" << std::endl;

    std::thread th1 {threaded_incr_vec, (int*)a1, vec_size, 1};
    std::thread th2 {threaded_incr_vec, (int*)a2, vec_size, 2};

    th1.join();
    th2.join();
}


void bench_demo(benchmark::State& state) {
    for (auto _ : state) {
        demo(state.range(0));
    }
}

BENCHMARK(bench_demo)->DenseRange(5, 100, 5);

BENCHMARK_MAIN();
