cmake_minimum_required(VERSION 3.27)
project(wavefs)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall --pedantic -g")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

# Agrega las fuentes del proyecto
add_executable(wavefs
        src/main.cpp
        src/wavefs/wavefs.cpp
        src/virtual/image_directory.cpp
        src/virtual/music_directory.cpp
        src/rest_client.cpp
        src/file_downloader.h
        src/file_downloader.cpp
        src/rest/tidal_client.cpp
        src/rest/tidal_client.h
        src/encode/base64.cpp
        src/encode/base64.h
        src/config/config.cpp
        src/config/config.h
        src/rest/oauth_restclient.cpp
        src/rest/oauth_restclient.h
        src/rest/types.h
        src/wavefs/wavefs_types.h
        src/cypher/cypher.cpp
        src/cypher/cypher.h
        src/repository/repository.cpp
        src/repository/repository.h
)

find_package(FUSE REQUIRED)
find_package(mongocxx REQUIRED)
find_package(curl REQUIRED)
find_package(curlpp REQUIRED)
find_package(OpenSSL REQUIRED)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(wavefs
        ${FUSE_LIBRARIES}
        ${MONGOCXX_LIBRARIES}
        bsoncxx
        curlpp
        curl
        nlohmann_json::nlohmann_json
        ${OPENSSL_LIBRARIES})

target_include_directories(wavefs PRIVATE include
        ${FUSE_LIBRARIES}
        ${MONGOCXX_INCLUDE_DIRS}
        ${CURLPP_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
        ${nlohmann_json_INCLUDE_DIRS}
        /usr/local/include/bsoncxx/v_noabi
        /usr/local/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc)
