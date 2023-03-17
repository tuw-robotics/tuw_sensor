// Copyright 2023 Eugen Kaltenegger

#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tuw_sensor_transducer/sensor.h>

using tuw_sensor_transducer::Sensor;

Sensor::Sensor(ros::NodeHandle node_handle)
{
  node_handle.param<std::string>("imu_port", this->port_, "/dev/ttyUSB0");
  node_handle.param<int>("imu_baudrate", this->baudrate_, 115200);
  node_handle.param<int>("imu_timeout", this->timeout_, 10);

  this->serialib_ = std::make_unique<serialib>();

  unsigned char connection_status_code = this->serialib_->Open(this->port_.c_str(), this->baudrate_);
  if (connection_status_code == 1)
  {
    ROS_INFO("SUCCESS connecting to device on port %s with baudrate %d", this->port_.c_str(), this->baudrate_);
  } else
  {
    ROS_ERROR("ERROR connecting to device on port %s with baudrate %d", this->port_.c_str(), this->baudrate_);
    throw std::runtime_error("ERROR connecting to device");
  }

  this->raw_publisher_ = node_handle.advertise<sensor_msgs::Imu>("imu_data_raw", 100, false);
  this->rpy_publisher_ = node_handle.advertise<sensor_msgs::Imu>("imu_data_rpy", 100, false);
  this->mag_publisher_ = node_handle.advertise<sensor_msgs::MagneticField>("imu_data_mag", 100, false);

  this->raw_message_ = sensor_msgs::Imu();
  this->raw_message_.header.frame_id                  = "imu";
  this->raw_message_.header.seq                          = +1;
  for (int i = 0; i < 9; i++)
  {
    this->raw_message_.angular_velocity_covariance[i]    = -1;
    this->raw_message_.linear_acceleration_covariance[i] = -1;
    this->raw_message_.orientation_covariance[i]         = -1;
  }

  this->rpy_message_ = sensor_msgs::Imu();
  this->rpy_message_.header.frame_id                  = "imu";
  this->rpy_message_.header.seq                           = 1;
  for (int i = 0; i < 9; i++)
  {
    this->rpy_message_.angular_velocity_covariance[i]    = -1;
    this->rpy_message_.linear_acceleration_covariance[i] = -1;
    this->rpy_message_.orientation_covariance[i]         = -1;
  }

  this->mag_message_ = sensor_msgs::MagneticField();
  this->mag_message_.header.frame_id                = "imu";
  this->mag_message_.header.seq                        = +1;
  for (int i = 0; i < 9; i++)
  {
    this->mag_message_.magnetic_field_covariance[i]     = 0;
  }

}

Sensor::~Sensor()
{
  this->serialib_->Close();
}

void Sensor::update()
{
  char serial_buffer[128];
  int buffer_size = sizeof(serial_buffer) / sizeof(char);
  int read_result = this->serialib_->Read(serial_buffer, buffer_size, static_cast<unsigned int>(NULL));

  Ep_Header header;
  char* data = serial_buffer;

  if (EP_SUCC_ == this->easy_profile_.On_RecvPkg(data, sizeof(serial_buffer) / sizeof(char), &header))
  {
    int sender_id = header.fromId;
    int command_type = header.cmd;

    // suppress warnings
    (void)sender_id;
    (void)command_type;

    switch (command_type)
    {
      case EP_CMD_ACK_:
      {
        Ep_Ack ep_Ack;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_Ack(&ep_Ack))
        {
          // currently not supported - handle data here
        }
        break;
      }
      case EP_CMD_STATUS_:
      {
        Ep_Status ep_Status;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_Status(&ep_Status))
        {
          // currently not supported - handle data here
        }
        break;
      }
      case EP_CMD_Raw_GYRO_ACC_MAG_:
      {
        ROS_DEBUG("received raw data");
        Ep_Raw_GyroAccMag ep_Raw_GyroAccMag;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_Raw_GyroAccMag(&ep_Raw_GyroAccMag))
        {
          this->raw_message_.header.seq += 1;
          this->raw_message_.header.stamp = ros::Time::now();

          this->raw_message_.angular_velocity.x = std::isnan(ep_Raw_GyroAccMag.gyro[0]) ? 0.0 : ep_Raw_GyroAccMag.gyro[0];
          this->raw_message_.angular_velocity.y = std::isnan(ep_Raw_GyroAccMag.gyro[1]) ? 0.0 : ep_Raw_GyroAccMag.gyro[1];
          this->raw_message_.angular_velocity.z = std::isnan(ep_Raw_GyroAccMag.gyro[2]) ? 0.0 : ep_Raw_GyroAccMag.gyro[2];

          this->raw_message_.linear_acceleration.x = std::isnan(ep_Raw_GyroAccMag.acc[0]) ? 0.0 : ep_Raw_GyroAccMag.acc[0];
          this->raw_message_.linear_acceleration.y = std::isnan(ep_Raw_GyroAccMag.acc[1]) ? 0.0 : ep_Raw_GyroAccMag.acc[1];
          this->raw_message_.linear_acceleration.z = std::isnan(ep_Raw_GyroAccMag.acc[2]) ? 0.0 : ep_Raw_GyroAccMag.acc[2];

          this->mag_message_.header.seq += 1;
          this->mag_message_.header.stamp = ros::Time::now();

          this->mag_message_.magnetic_field.x = std::isnan(ep_Raw_GyroAccMag.mag[0]) ? 0.0 : ep_Raw_GyroAccMag.mag[0];
          this->mag_message_.magnetic_field.y = std::isnan(ep_Raw_GyroAccMag.mag[1]) ? 0.0 : ep_Raw_GyroAccMag.mag[1];
          this->mag_message_.magnetic_field.z = std::isnan(ep_Raw_GyroAccMag.mag[2]) ? 0.0 : ep_Raw_GyroAccMag.mag[2];

          this->raw_publisher_.publish(this->raw_message_);
          this->mag_publisher_.publish(this->mag_message_);
        }
        break;
      }
      case EP_CMD_Q_S1_S_:
      {
        Ep_Q_s1_s ep_Q_s1_s;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_Q_s1_s(&ep_Q_s1_s))
        {
          // currently not supported - handle data here
        }
        break;
      }
      case EP_CMD_Q_S1_E_:
      {
        Ep_Q_s1_e ep_Q_s1_e;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_Q_s1_e(&ep_Q_s1_e))
        {
          // currently not supported - handle data here
        }
        break;
      }
      case EP_CMD_EULER_S1_S_:
      {
        Ep_Euler_s1_s ep_Euler_s1_s;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_Euler_s1_s(&ep_Euler_s1_s))
        {
          // currently not supported - handle data here
        }
        break;
      }
      case EP_CMD_EULER_S1_E_:
      {
        Ep_Euler_s1_e ep_Euler_s1_e;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_Euler_s1_e(&ep_Euler_s1_e))
        {
          // currently not supported - handle data here
        }
        break;
      }
      case EP_CMD_RPY_:
      {
        ROS_DEBUG("received rpy data");
        Ep_RPY ep_RPY;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_RPY(&ep_RPY))
        {
          this->rpy_message_.header.seq += 1;
          this->rpy_message_.header.stamp = ros::Time::now();

          if (!this->rpy_offset && ep_RPY.roll != 0.0 && ep_RPY.pitch != 0.0 && ep_RPY.yaw != 0.0)
          {
            this->rpy_offset = true;
            this->r_offset = Sensor::normalizeRad(Sensor::degreeToRad(ep_RPY.roll ));
            this->p_offset = Sensor::normalizeRad(Sensor::degreeToRad(ep_RPY.pitch));
            this->y_offset = Sensor::normalizeRad(Sensor::degreeToRad(ep_RPY.yaw  ));
            ROS_DEBUG("Initial setup for IMU finished");
            ROS_DEBUG("offset roll: %f", this->r_offset);
            ROS_DEBUG("offset pitch: %f", this->p_offset);
            ROS_DEBUG("offset yaw: %f", this->y_offset);
          }

          if (this->rpy_offset)
          {
            double roll  = Sensor::normalizeRad(Sensor::degreeToRad(ep_RPY.roll ) - this->r_offset);
            double pitch = Sensor::normalizeRad(Sensor::degreeToRad(ep_RPY.pitch) - this->p_offset);
            double yaw   = Sensor::normalizeRad(Sensor::degreeToRad(ep_RPY.yaw  ) - this->y_offset);

            tf2::Quaternion orientation_quaternion;
            orientation_quaternion.setRPY(roll, pitch, yaw);
            tf2::convert(orientation_quaternion, this->rpy_message_.orientation);

            this->rpy_publisher_.publish(this->rpy_message_);
          }
        }
        break;
      }
      case EP_CMD_GRAVITY_:
      {
        Ep_Gravity ep_Gravity;
        if (EP_SUCC_ == this->easy_object_dictionary_.Read_Ep_Gravity(&ep_Gravity))
        {
          // currently not supported - handle data here
        }
        break;
      }
      default:
      {
        ROS_WARN("ERROR invalid command type");
      }
      break;
    }
  }
  else
  {
    ROS_DEBUG("read was not successful, read result: %d", read_result);
  }
}

double Sensor::degreeToRad(double degree)
{
  return degree * M_PI / 180.0;
}

double tuw_sensor_transducer::Sensor::normalizeRad(double rad)
{
  while (rad >  2.0 * M_PI)
  {
    rad = rad - 2.0 * M_PI;
  }

  while (rad < -2.0 * M_PI)
  {
    rad = rad + 2.0 * M_PI;
  }

  return rad;
}
