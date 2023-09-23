# `tuw_sensor_transducer`

## Description
This package contains a node to operate the TransducerM TM352 IMU sensor.

*Important note:* The files in the `lib` folder based on the files provided by [SYD Dynamics](https://www.syd-dynamics.com/).
The original files can be downloaded on the [SYD Dynamics Download Center](https://www.syd-dynamics.com/download-center/).
Please note the `License.txt` file in the `lib` folder.

## Operation
In order to operate the `tuw_sensor_transducer_node` run:
```bash
roslaunch tuw_sensor_transduser imu.launch
```
This will start the node to operate the TransducerM IMU as well as a static transform for the IMU on the Independent Wheel Offset Steering robot prototype.
Additionally, an optional filter for the IMU is started.
This filter is ffrom the [`imu_filter_madgwick`](https://wiki.ros.org/imu_filter_madgwick) package.
