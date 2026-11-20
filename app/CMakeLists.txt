project(hideme_app)

set(CMAKE_AUTOMOC ON)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${sdkhelper_SOURCE_DIR}
    $ENV{SENTRY_LIB_INSTALL_DIR}/include)

set(SRC
    clitoolconnector.cpp
    clitoolconnector.h
    main.cpp
    hideme.cpp
    hideme.h
    socektconnector.cpp
    socektconnector.h
    settings.cpp
    settings.h
    servicemanager.cpp
    servicemanager.h
    serverselectionmodel.cpp
    serverselectionmodel.h
    systemdmanager.cpp
    systemdmanager.h
    logging.cpp
    logging.h)

add_executable(hideme ${SRC})

target_link_libraries(hideme
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::DBus
    -L$ENV{SENTRY_LIB_INSTALL_DIR}/lib -lsentry)

install(TARGETS hideme RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
if(CLICK_MODE)
    install(DIRECTORY ui DESTINATION /)
else()
install(DIRECTORY ui DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/${APP_NAME}/)
endif()
