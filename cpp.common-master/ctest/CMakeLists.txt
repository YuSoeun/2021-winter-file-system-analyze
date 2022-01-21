cmake_minimum_required (VERSION 3.20)

#roject (develop)

set (TARGET_NAME catch2_tests_run)
set (CMAKE_CXX_STANDARD 23)

set (UNIT_TEST_LIST 
  bplist_repr
  bplist
  byte_buffer2
  plist_parser
  timestamp
  api_sqlite3
  stream
  file_util
  scope_guard
  progressable
  async_runnable
)

foreach(NAME IN LISTS UNIT_TEST_LIST)
  list(APPEND UNIT_TEST_SOURCE_LIST test_${NAME}.cpp)
endforeach()

find_package(Boost 1.73 REQUIRED)

add_executable(${TARGET_NAME}
  main.cpp
  ${UNIT_TEST_SOURCE_LIST}
)

target_link_directories(${TARGET_NAME}
  PUBLIC ${Boost_LIBRARY_DIR}
)

target_link_libraries(${TARGET_NAME}
  PUBLIC
  core
  boost_system
  sqlite3
)

target_include_directories(${TARGET_NAME}
  PUBLIC ../vendor/include
  ${Boost_INCLUDE_DIR}
)

add_test(
  NAME ${TARGET_NAME}
  COMMAND ${TARGET_NAME} -o report.xml -r junit
)
