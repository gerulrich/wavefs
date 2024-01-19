# FindCurl.cmake

# Busca las bibliotecas y rutas de inclusión para curl basándose en encabezados de C

# Intenta encontrar los encabezados de curl
find_path(CURL_INCLUDE_DIRS curl/curl.h
        /usr/local/include
        /usr/include
        )

if(NOT CURL_INCLUDE_DIRS)
    message(SEND_ERROR "No se encontraron los encabezados de curl.")
endif()

# Proporciona las rutas de inclusión
set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})

# Busca la biblioteca de curl
find_library(CURL_LIBRARIES NAMES curl)

if(NOT CURL_LIBRARIES)
    message(SEND_ERROR "No se encontró la biblioteca curl.")
endif()

# Proporciona la biblioteca de curl
set(CURL_LIBRARIES ${CURL_LIBRARIES})
