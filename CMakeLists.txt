cmake_minimum_required(VERSION 3.5)
project(hide.client.ubtouch VERSION 0.1 LANGUAGES CXX)
include(FindGettext)
if(NOT GETTEXT_FOUND)
    message(FATAL_ERROR "Could not find gettext")
endif(NOT GETTEXT_FOUND)

# Standard install paths
include(GNUInstallDirs)

set(APP_NAME hideme)
set(DESKTOP_FILE "${PROJECT_NAME}.desktop")
set(APP_HARDCODE hide.client.ubtouch)
set(ICON_FILE app/graphics/hideme-app.svg)
set(SPLASH_FILE app/graphics/hideme-app-splash.svg)

set(DATA_DIR ${CMAKE_INSTALL_DATADIR}/${APP_HARDCODE})
set(EXEC ${APP_HARDCODE})
set(ICON ${CMAKE_INSTALL_PREFIX}/${DATA_DIR}/${ICON_FILE})

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

find_package(Qt5 COMPONENTS Core Gui Widgets Qml Quick REQUIRED)
set(APP_DIR ${CMAKE_INSTALL_FULL_DATADIR}/hide.client.ubtouch)
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

add_subdirectory(app)
add_subdirectory(po)

# make the qml files visible on qtcreator
file(GLOB QML_JS_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.qml *.js README* *.url-dispatcher *.txt)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES ${SRC_FILES} ${ICON_FILE} DESTINATION ${DATA_DIR})
install(DIRECTORY app/graphics DESTINATION ${DATA_DIR}/graphics)
