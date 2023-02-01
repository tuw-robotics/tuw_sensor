cmake_minimum_required(VERSION 3.0.2)
project(tuw_sensor_transducer)

find_package(
    catkin
  REQUIRED
    roscpp
    rospy
    roslint
)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets SerialPort)

set(CMAKE_AUTOMOC ON)

catkin_package(
  INCLUDE_DIRS
    include
)

include_directories(
  ./include
  ./lib/include
  ./lib/resource
  ${catkin_INCLUDE_DIRS}
)

roslint_cpp()

add_library(
  lib_transducer
  lib/src/EasyProfile/EasyObjectDictionary.cpp
  lib/src/EasyProfile/EasyProfile.cpp
  lib/src/EasyProfile/EasyProtocol.cpp
  lib/src/HelloMotionModule.cpp
)

target_link_libraries(
  lib_transducer
  Qt5::Widgets
  Qt5::SerialPort
)

add_executable(
  lib_transducer_example
  lib/src/main.cpp
)

add_dependencies(
  lib_transducer_example
  lib_transducer
)

target_link_libraries(
  lib_transducer_example
  lib_transducer
  Qt5::Widgets
  Qt5::SerialPort
)