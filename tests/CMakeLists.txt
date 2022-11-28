add_executable(entry entry_test.cpp)
target_compile_features(entry PRIVATE cxx_std_20)

target_link_libraries(entry PRIVATE exl fmt::fmt)