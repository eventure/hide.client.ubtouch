project(hideme_app)

set(CMAKE_AUTOMOC ON)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${sdkhelper_SOURCE_DIR})

set(SRC
    main.cpp
    hideme.cpp
    hideme.h)

add_executable(hide.client.ubtouch ${SRC})

target_link_libraries(hide.client.ubtouch
    Qt5::Core
    Qt5::Widgets
    Qt5::Qml
    Qt5::Quick
    Qt5::Gui)

install(TARGETS hide.client.ubtouch
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ui
    DESTINATION ${DATA_DIR})
