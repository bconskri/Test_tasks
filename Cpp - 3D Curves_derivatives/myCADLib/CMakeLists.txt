cmake_minimum_required(VERSION 3.19)
project(myCADLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# создаём статическую библиотеку
add_library(myCADLib STATIC mycadlib.cpp include/mycadlib.h)

if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

# создаём динамическую библиотеку
add_library(myCADLibDll SHARED mycadlib.cpp include/mycadlib.h)

# При компоновке динамической библиотеки будем использовать статическую.
# Для этого добавим к ней цель ImgLib
target_link_libraries(myCADLibDll myCADLib ${SYSTEM_LIBS})