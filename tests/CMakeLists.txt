include_directories(../include)

link_directories("${CMAKE_CURRENT_BINARY_DIR}/bin")

add_executable(fwbuf_api deque.c)

target_link_libraries(fwbuf_api LINK_PUBLIC fwbuf)


set_target_properties(fwbuf_api
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)
