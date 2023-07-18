include(ExternalProject)

set(CLI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cli)
ExternalProject_Add(cli
    SOURCE_DIR ${CLI_DIR}
    DOWNLOAD_COMMAND cd ${CLI_DIR}
    PREFIX ${CLI_DIR}
    CONFIGURE_COMMAND cd ${CLI_DIR} && export GOPATH=${CMAKE_CURRENT_BINARY_DIR} && /usr/lib/go-1.18/bin/go mod download golang.org/x/text && /usr/lib/go-1.18/bin/go get github.com/vishvananda/netlink@v1.1.0
    BUILD_COMMAND cd ${CLI_DIR} && export GOPATH=${CMAKE_CURRENT_BINARY_DIR} && /usr/lib/go-1.18/bin/go build -o hide.me
    BUILD_IN_SOURCE 1)
