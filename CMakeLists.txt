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

# Set the version for slang
set(slang_Version_Major 0)
set(slang_Version_Minor 1)
set(slang_Version_Patch 0)
set(slang_Version_Status "-dev")
set(PROJECT_VERSION
  "${slang_Version_Major}.${slang_Version_Minor}.${slang_Version_Patch}${slang_Version_Status}"
)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE CACHE BOOL "Disable quiet operation for FetchContent" FORCE)

if(NOT SLANG_USE_TESTS)
  set(use_tests OFF)
  message(STATUS "Set use tests to OFF as none was specified.")
  set(SLANG_USE_TESTS ${use_tests} CACHE STRING "Set use tests." FORCE)
endif()

message(STATUS "SLang Use Tests: ${SLANG_USE_TESTS}")

# Setup Doxygen and generate documentation
if(NOT SLANG_GEN_DOC)
  set(gen_docs OFF)
  message(STATUS "Set generate documentation to OFF as none was specified.")
  set(SLANG_GEN_DOC ${gen_docs} CACHE STRING "Set generate docs." FORCE)
endif()

message(STATUS "SLang Gen Docs: ${SLANG_GEN_DOC}")

if(SLANG_GEN_DOC)
  find_package(Doxygen)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen not found in system.")
  endif()

  FetchContent_Declare(
    doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
  )
  FetchContent_MakeAvailable(doxygen-awesome-css)
  FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)
  
  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  file(GLOB SOURCE_FOR_DOCS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/docs)

  doxygen_add_docs(${PROJECT_NAME}_docs ${SOURCE_FOR_DOCS}
    ALL USE_STAMP_FILE COMMENT "Generating documentation via Doxygen")
endif()

set(${PROJECT_NAME}_INSTALL_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CURRENT_BINARY_DIR ${${PROJECT_NAME}_INSTALL_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${${PROJECT_NAME}_INSTALL_DIR})

add_subdirectory(src)
add_subdirectory(tests)
