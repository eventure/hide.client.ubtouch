project(test_sentry)

set(CMAKE_AUTOMOC ON)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${sdkhelper_SOURCE_DIR}
    $ENV{SENTRY_LIB_INSTALL_DIR}/include)

set(SRC
    main.cpp)

add_executable(test_sentry ${SRC})

target_link_libraries(test_sentry
    Qt5::Core
    -L$ENV{SENTRY_LIB_INSTALL_DIR}/lib -lsentry)


add_test(NAME test_sentry
    COMMAND test_sentry)


add_custom_command(
    TARGET test_sentry
    COMMENT "Run tests"
    POST_BUILD
    COMMAND LD_PRELOAD=$ENV{SENTRY_LIB_INSTALL_DIR}/lib/libsentry.so ./test_sentry
)
