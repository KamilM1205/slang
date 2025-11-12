set(
  SOURCES
  main.cpp
  error.cpp
  panic.cpp
  errors.cpp
  ast.cpp
  lexer.cpp
  parser.cpp
)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/)

add_executable(${PROJECT_NAME} ${SOURCES})
install(TARGETS ${PROJECT_NAME} DESTINATION ${${PROJECT_NAME}_INSTALL_DIR})
