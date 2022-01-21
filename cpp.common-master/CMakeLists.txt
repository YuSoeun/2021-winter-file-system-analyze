cmake_minimum_required (VERSION 3.20)

project (develop)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories ( 
  $ENV{BOOST_HOME}
  /usr/local/include 
  vendor/include
  include
)

enable_testing()

add_subdirectory(src)
add_subdirectory(ctest)
