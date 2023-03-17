// Copyright 2023 Eugen Kaltenegger

#ifndef DIP_WS_SENSOR_H
#define DIP_WS_SENSOR_H

#include <ros/ros.h>

#include <sensor_msgs/Imu.h>
#include <sensor_msgs/MagneticField.h>

#include "../../lib/easy_profile/include/serialib.h"
#include "../../lib/easy_profile/include/EasyObjectDictionary.h"
#include "../../lib/easy_profile/include/EasyProfile.h"

namespace tuw_sensor_transducer
{
class Sensor
{
public:
  explicit Sensor(ros::NodeHandle node_handle);
  ~Sensor();
  void update();
private:
  static double degreeToRad(double degree);
  static double normalizeRad(double rad);
  std::string port_;
  int baudrate_ {0};
  int timeout_ {0};
  std::unique_ptr<serialib> serialib_;
  EasyObjectDictionary easy_object_dictionary_;
  EasyProfile easy_profile_ {&this->easy_object_dictionary_};
  ros::Publisher raw_publisher_;
  ros::Publisher rpy_publisher_;
  ros::Publisher mag_publisher_;
  sensor_msgs::Imu raw_message_;
  sensor_msgs::Imu rpy_message_;
  sensor_msgs::MagneticField mag_message_;
  bool rpy_offset{false};
  double r_offset{0.0};
  double p_offset{0.0};
  double y_offset{0.0};
};
}  // namespace tuw_sensor_transducer

#endif //DIP_WS_SENSOR_H
