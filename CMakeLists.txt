cmake_minimum_required(VERSION 3.5)
project(hideme VERSION 0.5 LANGUAGES CXX)
include(FindGettext)
if(NOT GETTEXT_FOUND)
    message(FATAL_ERROR "Could not find gettext")
endif(NOT GETTEXT_FOUND)

# Standard install paths
include(GNUInstallDirs)

option(CLICK_MODE "Installs to a contained location" on)
option(BUILD_CLI "Installs to a contained location" on)
option(BUILD_TESTS "Build tests" on)
option(SYSTEMD_ON_ROOT "Install systemd services into rootfs" off)

set(CLICK_NAME hideme.eventure)
set(APP_NAME hideme)
set(DESKTOP_FILE "${PROJECT_NAME}.desktop")
set(ICON_FILE app/graphics/hideme-app.svg)
set(SPLASH_FILE app/graphics/hideme-app-splash.svg)

if(SYSTEMD_ON_ROOT)
    add_compile_definitions(SYSTEMD_WITH_ROOT)
endif()

if(CLICK_MODE)
    set(CMAKE_INSTALL_PREFIX /)
    set(CMAKE_INSTALL_BINDIR /)
    set(CMAKE_INSTALL_DATADIR /)
    set(DATA_DIR /)
    set(CMAKE_INSTALL_FULL_DATADIR /)
    set(ICON hideme-app.svg)
    set(SPLASH ${SPLASH_FILE})
    set(EXEC "hideme")
    set(DESKTOP_DIR ${DATA_DIR})
    set(URLS_DIR ${DATA_DIR})

    add_compile_definitions(WITH_CLICK)
    set(CLI_PATH /opt/click.ubuntu.com/${CLICK_NAME}/${PROJECT_VERSION}/lib/aarch64-linux-gnu/bin/hide.me)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_BINARY_DIR}/../sentry/${ARCH_TRIPLET}/install")

    configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json ${APP_NAME}.apparmor DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE} DESTINATION ${DESKTOP_DIR})
    install(FILES ${SRC_FILES} app/graphics/hideme-app.svg DESTINATION DESTINATION /)

else(CLICK_MODE)
    set(DATA_DIR ${CMAKE_INSTALL_DATADIR}/${APP_NAME})
    set(EXEC ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${APP_NAME})
    set(ICON ${CMAKE_INSTALL_PREFIX}/${DATA_DIR}/${ICON_FILE})

    set(CLI_PATH "/usr/bin/hide.me")
endif(CLICK_MODE)

file(GLOB SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.qml *.js *.png *.js)

configure_file(${DESKTOP_FILE}.in.in ${DESKTOP_FILE}.in)

add_custom_target(${DESKTOP_FILE} ALL
    COMMENT "Merging translations into ${DESKTOP_FILE}..."
    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE}
            --desktop --template=${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in
            -o ${DESKTOP_FILE}
            -d ${CMAKE_SOURCE_DIR}/po
)

find_package(Qt5 COMPONENTS Core Gui Widgets Qml Quick DBus REQUIRED)
find_package(SENTRY CONFIG REQUIRED)
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
configure_file(hideme.service.in ${CMAKE_CURRENT_BINARY_DIR}/hideme.service @ONLY)

add_subdirectory(app)
add_subdirectory(po)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_CLI)
    add_subdirectory(tools)
endif(BUILD_CLI)

if(CLICK_MODE)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}  DESTINATION /)
    install(DIRECTORY app/graphics DESTINATION DESTINATION /ui/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CA.pem  DESTINATION /)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hideme.service  DESTINATION /)
    add_definitions("-DWITH_CLICK")
else(CLICK_MODE)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES ${SRC_FILES} ${ICON_FILE} DESTINATION DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/${APP_NAME}/)
    install(DIRECTORY app/graphics DESTINATION DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/${APP_NAME}/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CA.pem  DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/${APP_NAME}/)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hideme.service DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/${APP_NAME}/)
endif(CLICK_MODE)


if(NOT SENTRY_FOUND)
    message(SEND_ERROR "Failed to find Sentry")
    return()
else()
    message(STATUS "Found Sentry: " $ENV{SENTRY_LIB_INSTALL_DIR})
    include_directories(${SENTRY_LIB_INSTALL_DIR})
endif()
