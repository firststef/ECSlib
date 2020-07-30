cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(ECSlib)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(BUILD_EXAMPLES CACHE BOOL OFF)
    set(BUILD_GAMES CACHE BOOL OFF)
else()
    set(BUILD_EXAMPLES OFF)
    set(BUILD_GAMES OFF)
endif()
add_compile_options(-DSHARED=ON -DSTATIC=ON)

add_definitions(-DCIOPILLIS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(raylib)

##################### Variables ############################
# Change if you want modify path or other values           #
############################################################

# Project
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(DEPENDENCIES_DIR ${PROJECT_DIR}/dependencies)
set(PROJECT_NAME ECSlib)


# Outputs
set(OUTPUT_DEBUG Debug)
set(OUTPUT_RELEASE Release)

################# CMake Project ############################
# The main options of project                              #
############################################################

project(${PROJECT_NAME} CXX)

# Define Debug by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
  message(STATUS "Build type not specified: Use Debug by default.")
endif(NOT CMAKE_BUILD_TYPE)

############## Artefacts Output ############################
# Defines outputs , depending BUILD TYPE                   #
############################################################

if(MSVC)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}")
else()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
endif()
endif()

# Messages
message("${PROJECT_NAME}: MAIN PROJECT: ${CMAKE_PROJECT_NAME}")
message("${PROJECT_NAME}: CURR PROJECT: ${CMAKE_CURRENT_SOURCE_DIR}")
message("${PROJECT_NAME}: CURR BIN DIR: ${CMAKE_CURRENT_BINARY_DIR}")

############### Files & Targets ############################
# Files of project and target to build                     #
############################################################

# Source Files
set(SRC_FILES
    ./src/External.cpp
    ./src/main.cpp
    ./src/NetworkSystem.cpp
    ./src/PhyFunctions.cpp
    ./src/PhyFunctions2.cpp
    ./src/PhysicsComponent.cpp
    ./src/PhysicsSystem.cpp
    ./src/Event.cpp
)
source_group("Sources" FILES ${SRC_FILES})

# Header Files
set(HEADERS_FILES
    src/AnimationComponent.h
    src/AnimationEvent.h
    src/AnimationSystem.h
    src/Component.h
    src/Components.h
    src/DefferEvent.h
    src/DefferSystem.h
    src/DrawSystem.h
    src/ECSManager.h
    src/ECSlib.h
    src/Entity.h
    src/Event.h
    src/EventManager.h
    src/Events.h
    src/External.h
    src/GridAddRemoveEvent.h
    src/GridContainerChildComponent.h
    src/GridContainerComponent.h
    src/GridContainerSystem.h
    src/Helpers.h
    src/HitBoxComponent.h
    src/HitBoxEvent.h
    src/HitBoxSystem.h
    src/KeyboardEvent.h
    src/KeyboardInputComponent.h
    src/KeyboardInputSystem.h
    src/MouseEvent.h
    src/MouseInputSystem.h
    src/NetworkEvent.h
    src/NetworkSystem.h
    src/nvidia.h
    src/PhyFunctions.h
    src/PhyFunctions2.h
    src/PhysicsComponent.h
    src/PhysicsSystem.h
    src/Pool.h
    src/SpriteComponent.h
    src/System.h
    src/SystemControlEvent.h
    src/Systems.h
    src/TextureManager.h
    src/TransformComponent.h
)
source_group("Headers" FILES ${HEADERS_FILES})

# Add library to build.
add_library(${PROJECT_NAME} STATIC
   ${SRC_FILES} ${HEADERS_FILES}
)

######################### Flags ############################
# Defines Flags for Windows and Linux                      #
############################################################

if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++17")
   endif()
endif(NOT MSVC)

# Preprocessor definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
   -DGRAPHICS_API_OPENGL_33
   -DPLATFORM_DESKTOP
   -D_CRT_SECURE_NO_WARNINGS
   -D_CRT_NONSTDC_NO_DEPRECATE
   -D_DEBUG
    )
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /MD /Od /EHsc)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
   -DGRAPHICS_API_OPENGL_33
   -DPLATFORM_DESKTOP
   -D_CRT_SECURE_NO_WARNINGS
    )
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /GL /Oi /Gy /EHsc)
    endif()
endif()

########### Link & Dependencies ############################
# Add project dependencies and Link to project             #
############################################################
message(Current source dir:)
message(${CMAKE_BINARY_DIR})
if(MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/raylib/src/Debug/raylib_static.lib winmm.lib ws2_32.lib)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/raylib/src/Release/raylib_static.lib winmm.lib ws2_32.lib)
  endif()
else()
    target_link_libraries(${PROJECT_NAME} raylib)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/raylib/src/)
