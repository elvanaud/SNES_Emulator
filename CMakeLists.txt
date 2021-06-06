cmake_minimum_required(VERSION 3.0.0)
project(SNES_Emulator VERSION 0.1.0)

include(CTest)
enable_testing()

set(CXX_STANDARD 2014)

file (GLOB_RECURSE Project_SOURCES CONFIGURE_DEPENDS "./Sources/*.cc")
#file (GLOB_RECURSE Project_HEADERS CONFIGURE_DEPENDS "./*.h")
#include_directories("include")

set(SFML_DIR "../../../Librairies/SFML/SFML-install/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)


#include_directories("${CMAKE_CURRENT_SOURCE_DIR}") 
include_directories("./Sources/") 
include_directories("./Libs")

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_executable(SNES_Emulator  ${Project_SOURCES})
#target_link_libraries(SNES_Emulator ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
target_link_libraries(SNES_Emulator sfml-graphics sfml-audio)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
