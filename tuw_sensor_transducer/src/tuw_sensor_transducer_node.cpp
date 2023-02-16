// Copyright 2023 Eugen Kaltenegger

#include <ros/ros.h>
#include "tuw_sensor_transducer/sensor.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "TUW_SENSOR_TRANSDUCER");
  ros::NodeHandle node_handle;

  tuw_sensor_transducer::Sensor sensor(node_handle);

  ros::Rate loop_rate(300);
  while(ros::ok())
  {
    sensor.update();
    loop_rate.sleep();
  }
}