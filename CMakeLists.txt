cmake_minimum_required(VERSION 3.16)
project(slang)

if(NOT CMAKE_BUILD_TYPE)
  set(default_build_type "Debug")
  message(STATUS "Set the build type to `${default_build_type}` as none was specified.")
  set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING "Chooce the build type." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
  "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message(STATUS "slang Build Type: ${CMAKE_BUILD_TYPE}")

# Set the version for cozyfetch
set(cozyfetch_Version_Major 0)
set(cozyfetch_Version_Minor 1)
set(cozyfetch_Version_Patch 0)
set(cozyfetch_Version_Status "-dev")
set(PROJECT_VERSION
  "${cozyfetch_Version_Major}.${cozyfetch_Version_Minor}.${cozyfetch_Version_Patch}${cozyfetch_Version_Status}"
)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 20)

if(NOT SLANG_USE_TESTS)
  set(use_tests OFF)
  message(STATUS "Set use tests to OFF as none was specified.")
  set(SLANG_USE_TESTS ${use_tests} CACHE STRING "Set use tests." FORCE)
endif()

message(STATUS "slang Use Tests: ${COZY_USE_TESTS}")

set(${PROJECT_NAME}_INSTALL_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CURRENT_BINARY_DIR ${${PROJECT_NAME}_INSTALL_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${${PROJECT_NAME}_INSTALL_DIR})

add_subdirectory(src)
add_subdirectory(tests)
