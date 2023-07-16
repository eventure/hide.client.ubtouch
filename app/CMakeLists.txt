project(hideme_app)

set(CMAKE_AUTOMOC ON)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${sdkhelper_SOURCE_DIR})

set(SRC
    clitoolconnector.cpp
    clitoolconnector.h
    main.cpp
    hideme.cpp
    hideme.h)

add_executable(hideme ${SRC})

target_link_libraries(hideme
    Qt5::Core
    Qt5::Widgets
    Qt5::Qml
    Qt5::Quick
    Qt5::Gui)

install(TARGETS hideme RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
if(CLICK_MODE)
    install(DIRECTORY ui DESTINATION /)
else()
install(DIRECTORY ui DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/${APP_NAME}/)
endif()
