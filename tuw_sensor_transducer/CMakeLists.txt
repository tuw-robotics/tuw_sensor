cmake_minimum_required(VERSION 3.0.2)
project(tuw_sensor_transducer)

find_package(
    catkin
  REQUIRED
    roscpp
    rospy
    roslint
    sensor_msgs
)

#find_package(Qt5 REQUIRED COMPONENTS Core Widgets SerialPort)

catkin_package(
  INCLUDE_DIRS
  include
  lib/easy_profile/include
)

include_directories(
  ./include
  ${catkin_INCLUDE_DIRS}
)

roslint_cpp()

add_library(
  lib_transducer
  lib/easy_profile/src/EasyObjectDictionary.cpp
  lib/easy_profile/src/EasyProfile.cpp
  lib/easy_profile/src/EasyProtocol.cpp
  lib/easy_profile/src/serialib.cpp
)

add_library(
  ${PROJECT_NAME}
  src/${PROJECT_NAME}/sensor
)

add_dependencies(
  ${PROJECT_NAME}
  lib_transducer
)

target_link_libraries(
  ${PROJECT_NAME}
  lib_transducer
)

add_executable(
  ${PROJECT_NAME}_node
  src/${PROJECT_NAME}_node.cpp
)

add_dependencies(
  ${PROJECT_NAME}_node
  ${PROJECT_NAME}
  ${catkin_EXPORTED_TARGETS}
)

target_link_libraries(
  ${PROJECT_NAME}_node
  ${PROJECT_NAME}
  ${catkin_LIBRARIES}
)