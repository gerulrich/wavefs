# FindCurlpp.cmake

# Busca las bibliotecas y rutas de inclusión para curlpp basándose en encabezados de C++

# Intenta encontrar los encabezados de curlpp
find_path(CURLPP_INCLUDE_DIRS curlpp/cURLpp.hpp
        /usr/local/include
        /usr/include
        )

if(NOT CURLPP_INCLUDE_DIRS)
    message(SEND_ERROR "No se encontraron los encabezados de curlpp.")
endif()

# Proporciona las rutas de inclusión
set(CURLPP_INCLUDE_DIRS ${CURLPP_INCLUDE_DIRS})

# Busca la biblioteca de curlpp
find_library(CURLPP_LIBRARIES NAMES curlpp)

if(NOT CURLPP_LIBRARIES)
    message(SEND_ERROR "No se encontró la biblioteca curlpp.")
endif()

# Proporciona la biblioteca de curlpp
set(CURLPP_LIBRARIES ${CURLPP_LIBRARIES})