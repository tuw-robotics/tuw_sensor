// Copyright 2023 Eugen Kaltenegger

#include <ros/ros.h>
#include <tuw_sensor_transducer/sensor.h>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "TUW_SENSOR_TRANSDUCER");
  ros::NodeHandle node_handle;

  int sensor_node_hz;
  node_handle.param<int>("sensor_node_hz", sensor_node_hz, 300);
  tuw_sensor_transducer::Sensor sensor(node_handle);

  ros::Rate loop_rate(sensor_node_hz);
  while(ros::ok())
  {
    sensor.update();
    loop_rate.sleep();
  }
}