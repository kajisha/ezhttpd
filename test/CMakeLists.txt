file(GLOB SOURCES *.c)

enable_testing()

add_executable(test_server ${SOURCES})
target_link_libraries(test_server ezhttpd)

set_target_properties(test_server PROPERTIES LINK_FLAGS -Wl,-lcunit)

add_test(test_server ${EXECUTABLE_OUTOUT_PATH}/test_server)
