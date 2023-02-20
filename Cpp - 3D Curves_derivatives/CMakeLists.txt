cmake_minimum_required(VERSION 3.19)
project(CADExchanger)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(myCADLib)

add_executable(CADExchanger main.cpp)

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

target_include_directories(CADExchanger PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/myCADLib/include")
target_link_directories(CADExchanger PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/myCADLib/lib/$<IF:$<CONFIG:Debug>,Debug,Release>"
        )
target_link_libraries(CADExchanger myCADLib ${SYSTEM_LIBS})