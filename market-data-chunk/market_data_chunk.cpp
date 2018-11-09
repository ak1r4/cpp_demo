#include <benchmark/benchmark.h>
#include <vector>
#include <ostream>
#include <iostream>
#include <utility>
#include <algorithm>
#include <random>
#include <functional>
#include <chrono>

using timestamp_t = uint64_t;
using price_t = uint64_t;
using index_t = uint64_t;
using price_point_t = std::pair<timestamp_t, price_t>;
using chunk_indexes_t = std::pair<index_t, index_t>;
using max_min_t = std::pair<price_t, price_t>;

constexpr timestamp_t time_range_start = 1;
constexpr timestamp_t time_range_end = 15 * 60 * 1000;
constexpr price_t price_range_start = 3000 * 100;
constexpr price_t price_range_end = 6000 * 100;
constexpr size_t n_data_points = 30 * 1000;
constexpr index_t n_chunks = 15;

#define RECORD(t) auto t = std::chrono::high_resolution_clock::now()
#define DELTA_NANO(t1, t0) std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()

template<typename T>
std::ostream& operator<< (std::ostream& os, std::vector<T>& t) {
    os << "[";

    for (const auto& i : t) {
        os << "(" << i.first << ", " << i.second << "), ";
    }
    os << "]";

    return os;
}


std::vector<price_point_t> generate_price_point_vec() {
    // prepare random generator
    std::random_device rd;
    std::default_random_engine generator_time(rd());
    std::default_random_engine generator_price(rd());

    std::uniform_int_distribution<timestamp_t> distribution_time {time_range_start, time_range_end};
    std::uniform_int_distribution<price_t> distribution_price {price_range_start, price_range_end};

    auto time_gen = std::bind(distribution_time, generator_time);
    auto price_gen = std::bind(distribution_price, generator_price);

    std::vector<price_point_t> pp_vec {};

    for (size_t i = 0; i < n_data_points; i++) {
        pp_vec.push_back(price_point_t {time_gen(), price_gen()});
    }

    std::sort(pp_vec.begin(),
              pp_vec.end(),
              [](const price_point_t& pp0, const price_point_t& pp1) {
                  return pp0.first < pp1.first;
              });

    //std::cout << pp_vec << "\n";
    return pp_vec;
}


class MaxMinAgg {
 public:
    MaxMinAgg(const timestamp_t start_time,
              const timestamp_t end_time,
              const size_t n_chunks)
        : start_time_(start_time),
          end_time_(end_time),
          n_chunks_(n_chunks) {
        size_t chunk_size = (end_time - start_time) / n_chunks;
        index_t start = start_time;
        index_t end = start_time + chunk_size;
        for (size_t i = 0; i < n_chunks; i++) {
            max_min_vec_.push_back(max_min_t {price_range_start, price_range_end});
            chunk_indexes_vec_.push_back(chunk_indexes_t {start, end});
            start = end;
            end = start + chunk_size;
        }
    }

    void on_price_point(const price_point_t& pp) {
        size_t chunk_idx = 0;
        for (; chunk_idx < n_chunks_; chunk_idx++) {
            if (pp.first < chunk_indexes_vec_[chunk_idx].first) {
                break;
            }
        }
        chunk_idx--;

        if (pp.second > max_min_vec_[chunk_idx].first) {
            max_min_vec_[chunk_idx].first = pp.second;
        } else if (pp.second < max_min_vec_[chunk_idx].second) {
            max_min_vec_[chunk_idx].second = pp.second;
        }
    }

    const std::vector<max_min_t> result() const {
        return max_min_vec_;
    }

 private:
    const timestamp_t start_time_;
    const timestamp_t end_time_;
    const size_t n_chunks_;

    std::vector<max_min_t> max_min_vec_ {};
    std::vector<chunk_indexes_t> chunk_indexes_vec_ {};
};


void bench(benchmark::State& state) {
    auto pp_vec = generate_price_point_vec();
    MaxMinAgg max_min_agg {time_range_start, time_range_end, n_chunks};

    //RECORD(t0);
    for (auto _ : state) {
        for (const auto& pp : pp_vec) {
            max_min_agg.on_price_point(pp);
        }
    }
    //RECORD(t1);

    //std::cout << "Agg time: " << DELTA_NANO(t1, t0) << "ns\n";

    auto result = max_min_agg.result();

    //std::cout << result << "\n";
}

BENCHMARK(bench);

BENCHMARK_MAIN();
