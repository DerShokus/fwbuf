find_package(CMocka REQUIRED)

include_directories(${CMOCKA_INCLUDE_DIR} ../include)

link_directories("${CMAKE_CURRENT_BINARY_DIR}/bin")

add_executable(fwbuf_api deque.c)
add_executable(run_tests run_tests.c)

target_link_libraries(run_tests ${CMOCKA_LIBRARY} fwbuf)
target_link_libraries(fwbuf_api LINK_PUBLIC fwbuf)


set_target_properties(fwbuf_api run_tests
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)
