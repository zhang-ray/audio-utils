cmake_minimum_required(VERSION 2.8)
project(audio-utils)

set (CMAKE_CXX_STANDARD 11)

# treat all warnings as errors
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory(find-empty-fragment)
