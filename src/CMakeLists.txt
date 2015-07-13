add_library(ezhttpd STATIC tcp_server.c http_server.c)

target_include_directories(ezhttpd PUBLIC "$<BUILD_INTERFACE:${ROOT_SOURCE_DIR}/src>")
