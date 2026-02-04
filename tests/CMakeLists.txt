if(SLANG_USE_TESTS)
  include(FetchContent)
  set(FETCHCONTENT_QUIET FALSE)

  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )

  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  add_executable(
    lexer
    ${CMAKE_CURRENT_LIST_DIR}/lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/error.cpp
    ${PROJECT_SOURCE_DIR}/src/errors.cpp
    ${PROJECT_SOURCE_DIR}/src/panic.cpp
  )
  target_link_libraries(
    lexer
    GTest::gtest_main
  )
  target_include_directories(lexer PUBLIC ${PROJECT_SOURCE_DIR}/src)

  add_executable(
    errors
    ${CMAKE_CURRENT_LIST_DIR}/errors.cpp
    ${PROJECT_SOURCE_DIR}/src/error.cpp
    ${PROJECT_SOURCE_DIR}/src/errors.cpp
  )
  target_link_libraries(
    errors
    GTest::gtest_main
  )
  target_include_directories(errors PUBLIC ${PROJECT_SOURCE_DIR}/src)

  add_executable(
    ast
    ${CMAKE_CURRENT_LIST_DIR}/ast.cpp
    ${PROJECT_SOURCE_DIR}/src/ast.cpp
    ${PROJECT_SOURCE_DIR}/src/error.cpp
    ${PROJECT_SOURCE_DIR}/src/errors.cpp
    ${PROJECT_SOURCE_DIR}/src/panic.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer.cpp
  )
  target_link_libraries(
    ast
    GTest::gtest_main
  )
  target_include_directories(ast PUBLIC ${PROJECT_SOURCE_DIR}/src)

  add_executable(
    parser
    ${CMAKE_CURRENT_LIST_DIR}/parser.cpp
    ${PROJECT_SOURCE_DIR}/src/ast.cpp
    ${PROJECT_SOURCE_DIR}/src/error.cpp
    ${PROJECT_SOURCE_DIR}/src/errors.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/parser.cpp
    ${PROJECT_SOURCE_DIR}/src/panic.cpp
  )
  target_link_libraries(
    parser
    GTest::gtest_main
  )
  target_include_directories(parser PUBLIC ${PROJECT_SOURCE_DIR}/src)

  include(GoogleTest)
  gtest_discover_tests(lexer)
  gtest_discover_tests(errors)
  gtest_discover_tests(ast)
  gtest_discover_tests(parser)

  add_test(NAME lexer COMMAND testlexer WORKING_DIRECTORY ${${PROJECT_NAME}_INSTALL_DIR}})
  add_test(NAME errors COMMAND testerrors WORKING_DIRECTORY ${${PROJECT_NAME}_INSTALL_DIR}})
  add_test(NAME ast COMMAND testast WORKING_DIRECTORY ${${PROJECT_NAME}_INSTALL_DIR})
  add_test(NAME parser COMMAND testparser WORKING_DIRECTORY ${${PROJECT_NAME}_INSTALL_DIR})
endif()

