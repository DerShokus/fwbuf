cmake_minimum_required (VERSION 2.8.11)

project(fwbuf)

# Building
add_subdirectory(src)
add_subdirectory(tests)

# Tests
enable_testing()
add_test (NAME fwbuf_api COMMAND ./tests/bin/fwbuf_api)
