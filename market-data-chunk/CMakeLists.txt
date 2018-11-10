add_executable(market-data-chunk market_data_chunk.cpp)
target_link_libraries(market-data-chunk benchmark pthread)

add_executable(market-data-periods market_data_periods.cpp)
target_link_libraries(market-data-periods benchmark pthread)
