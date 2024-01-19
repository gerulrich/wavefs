# Findmongocxx.cmake

# Busca las bibliotecas y rutas de inclusión para mongocxx basándose en encabezados de C++

# Intenta encontrar los encabezados de mongocxx
find_path(MONGOCXX_INCLUDE_DIRS mongocxx/client.hpp
        /usr/local/include
        /usr/include
        /usr/local/include/mongocxx/v_noabi/
        )

if(NOT MONGOCXX_INCLUDE_DIRS)
    message(SEND_ERROR "No se encontraron los encabezados de mongocxx.")
endif()

# Proporciona las rutas de inclusión
set(MONGOCXX_INCLUDE_DIRS ${MONGOCXX_INCLUDE_DIRS})

# Busca la biblioteca de mongocxx
find_library(MONGOCXX_LIBRARIES NAMES mongocxx)

if(NOT MONGOCXX_LIBRARIES)
    message(SEND_ERROR "No se encontró la biblioteca mongocxx.")
endif()

# Proporciona la biblioteca de mongocxx
set(MONGOCXX_LIBRARIES ${MONGOCXX_LIBRARIES})