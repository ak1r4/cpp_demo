#include <benchmark/benchmark.h>
#include <cstdint>
#include <random>
#include <vector>
#include <utility>
#include <ostream>
#include <iostream>
#include <limits>

/* Uncomment this to run debug */
//#define DEBUG

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

using timestamp_t = int64_t;
using price_t = double;
using price_point_t = std::pair<timestamp_t, price_t>;

constexpr price_t max_price = std::numeric_limits<price_t>::max();
constexpr price_t min_price = std::numeric_limits<price_t>::min();

template<typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& t) {
    os << "[";
    for (const auto& i : t) {
        os << "(" << i.first << "," << i.second << "), ";
    }
    os << "]";
    return os;
}


std::vector<price_point_t> generate_data(size_t n_points) {
    std::vector<price_point_t> pp_vec {};
    timestamp_t t = 0;
    price_t p = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> time_dis(15, 35);
    std::normal_distribution<> price_dis(0, 1);

    for (size_t i = 0; i < n_points; i++) {
        t += time_dis(gen);
        p += price_dis(gen);
        pp_vec.push_back(price_point_t {t, p});
    }

    return pp_vec;
}


struct OHLC {
    price_t open;
    price_t high;
    price_t low;
    price_t close;
    size_t count;

    OHLC() = default;
    OHLC(price_t open, price_t high, price_t low, price_t close, size_t count)
        : open(open), high(high), low(low), close(close), count(count) {}

    friend std::ostream& operator<< (std::ostream& os, const OHLC& o) {
        os << "{"
           << "O: " << o.open << " ,"
           << "H: " << o.high << " ,"
           << "L: " << o.low << " ,"
           << "C: " << o.close << " ,"
           << "Count: " << o.count << "}";
        return os;
    }
};


std::ostream& operator<< (std::ostream& os, std::vector<OHLC>& v) {
    for (const auto& i : v) {
        os << i << "\n";
    }
    return os;
}

void collect(std::vector<OHLC>& ohlc_vec,
             const size_t period_size,
             const size_t n_period,
             const std::vector<price_point_t>& pp_vec) {
    size_t n_points = pp_vec.size();
    size_t period_idx = 0;
    timestamp_t period_start = pp_vec[n_points - 1].first;
    timestamp_t period_end = period_start - period_size;
    D(std::cout << "n_points:" << n_points << "\n");

    for (int i = n_points - 1; i > -1; i--) {
        if (period_idx == n_period) {
            break;
        }

        const auto& pp = pp_vec[i];
        auto& ohlc = ohlc_vec[period_idx];
        ohlc.count++;

        timestamp_t peek_timestamp = ((i - 1) > -1) ? pp_vec[i - 1].first : -1;

        D(std::cout << pp.first << "," << pp.second << ":" << period_idx << "@" << ohlc);
        if (pp.first == period_start) {
            ohlc.open = pp.second;
            D(std::cout << ": open");
        } else if (peek_timestamp < period_end) {
            ohlc.close = pp.second;
            period_idx++;
            D(std::cout << ": close");
            if (peek_timestamp == -1) {
                break;
            }
            period_start = peek_timestamp;
            period_end = period_start - period_size;
        } else if (pp.second > ohlc.high) {
            ohlc.high = pp.second;
            D(std::cout << ": high");
        } else if (pp.second < ohlc.low) {
            ohlc.low = pp.second;
            D(std::cout << ": low");
        }
        D(std::cout << "\n");
    }
}

auto bench = [](benchmark::State& state,
                const std::vector<price_point_t>& pp_vec,
                const size_t n_period,
                const size_t period_size) {
    for (auto _ : state) {
        // init ohlc vector with empty data
        std::vector<OHLC> ohlc_vec {};

        for (size_t i = 0; i < n_period; i++) {
            ohlc_vec.push_back(OHLC {0, min_price, max_price, 0, 0});
        }

        // loop through price point vector and collect OHLC
        collect(ohlc_vec, period_size, n_period, pp_vec);
    }
};

int debug_run() {
    D(std::cout.precision(std::numeric_limits<double>::max_digits10));

    const size_t n_data_points = 100;
    const size_t n_period = 5;
    const size_t period_size = 400;

    auto pp_vec = generate_data(n_data_points);
    D(std::cout << pp_vec << "\n");

    std::vector<OHLC> ohlc_vec {};

    for (size_t i = 0; i < n_period; i++) {
        ohlc_vec.push_back(OHLC {0, min_price, max_price, 0, 0});
    }

    collect(ohlc_vec, period_size, n_period, pp_vec);

    D(std::cout << ohlc_vec << "\n");

    return 0;
}

int main(int argc, char** argv) {

#ifdef DEBUG
    debug_run();
#else
    const size_t n_data_points = 30000;
    const size_t n_period = 14;
    const size_t period_size = 48000;
    auto pp_vec = generate_data(n_data_points);

    benchmark::RegisterBenchmark("bench", bench, pp_vec, n_period, period_size);
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
#endif

    return 0;
}
