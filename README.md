# Unitree GO2 Robot ROS 2

<p align="center">
<img width="1280" height="420" src="https://github.com/IntelligentRoboticsLabs/go2_robot/assets/44479765/da616d77-cf4d-4acf-af2f-adc99f4f72d7)" alt='Holi'>
</p>

[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
![distro](https://img.shields.io/badge/Ubuntu%2022-Jammy%20Jellyfish-green)
![distro](https://img.shields.io/badge/ROS2-Humble-blue)
[![humble](https://github.com/IntelligentRoboticsLabs/go2_robot/actions/workflows/humble.yaml/badge.svg)](https://github.com/IntelligentRoboticsLabs/go2_robot/actions/workflows/humble.yaml)
[![humble-devel](https://github.com/IntelligentRoboticsLabs/go2_robot/actions/workflows/humble_devel.yaml/badge.svg)](https://github.com/IntelligentRoboticsLabs/go2_robot/actions/workflows/humble_devel.yaml)

In this package is our integration for the Unitee Go2 robot.

## Checklist

- [x] robot description
- [x] odom
- [x] pointcloud
- [x] joint_states
- [x] Visualization in rviz
- [x] cmd_vel
- [x] go2_interfaces
- [x] Change modes
- [x] Change configuration for robot
- [x] SLAM
- [x] Nav2
- [ ] Ros2cli (working in progress)
- [ ] Hardware interface
- [ ] Gazebo simulation

## Installation
You need to have previously installed ROS2. Please follow this [guide](https://docs.ros.org/en/humble/Installation.html) if you don't have it.

```bash
source /opt/ros/humble/setup.bash
```

Create workspace and clone the repository

```bash
mkdir ~/go2_ws/src
cd ~/go2_ws/src
git clone https://github.com/Unitree-Go2-Robot/go2_robot.git -b humble
```

Prepare your thirparty repos:
```bash
sudo apt update && sudo apt install ros-dev-tools -y
vcs import < go2_robot/dependencies.repos
```

Install dependencies and build workspace
```bash
cd ~/go2_ws
sudo rosdep init
rosdep update
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install 
```

Setup the workspace
```bash
source ~/ros2_ws/install/setup.bash
```

## Sensor installation
If you have purchased a hesai lidar 3d, or a realsense d435i, follow the following steps inside the robot.

### Hesai Lidar

```bash
sudo apt-get install libboost-all-dev
sudo apt-get install -y libyaml-cpp-dev
git clone --recurse-submodules https://github.com/HesaiTechnology/HesaiLidar_ROS_2.0.git
cd ..
source /opt/ros/$ROS_DISTRO/setup.bash
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
source install/setup.bash
```

Set the lidar IP to `config/config.yaml`
```yaml
lidar:
- driver:
    udp_port: 2368                                       #UDP port of lidar
    ptc_port: 9347                                       #PTC port of lidar
    device_ip_address: <Device IP>                       #IP address of lidar
    pcap_path: "<Your PCAP file path>"                   #The path of pcap file (set during offline playback)
    correction_file_path: "<Your correction file path>"  #LiDAR angle file, required for offline playback of pcap/packet rosbag
    firetimes_path: "<Your firetime file path>"          #The path of firetimes file
    source_type: 2                                       #The type of data source, 1: real-time lidar connection, 2: pcap, 3: packet rosbag
    pcap_play_synchronization: true                      #Pcap play rate synchronize with the host time
    x: 0                                                 #Calibration parameter
    y: 0                                                 #Calibration parameter
    z: 0                                                 #Calibration parameter
    roll: 0                                              #Calibration parameter
    pitch: 0                                             #Calibration parameter
    yaw: 0                                               #Calibration parameter
ros:
    ros_frame_id: hesai_lidar                            #Frame id of packet message and point cloud message
    ros_recv_packet_topic: /lidar_packets                #Topic used to receive lidar packets from ROS
    ros_send_packet_topic: /lidar_packets                #Topic used to send lidar packets through ROS
    ros_send_point_cloud_topic: /lidar_points            #Topic used to send point cloud through ROS
    send_packet_ros: true                                #true: Send packets through ROS 
    send_point_cloud_ros: true                           #true: Send point cloud through ROS 
```

### Realsense d435i

```bash
sudo apt install ros-humble-realsense2-camera
```

## Usage
### Bringup the robot
Either from your computer, or from inside the robot, execute the following:
```bash
ros2 launch go2_bringup go2.launch.py
```
> If you have a realsense and a lidar inside the robot, use the lidar or realsense parameters.
> It will only work if you throw everything inside the robot

If you want to see your robot through rviz, do it as follows:
```bash
ros2 launch go2_bringup go2.launch.py rviz:=True
```

### Change modes
If what you want is for your robot to be able to change modes, thus performing the movements predefined by the controller, use the following service:
```bash
ros2 service call /mode go2_interfaces/srv/Mode "mode: 'hello'"
```

<details>
<summary>Available modes</summary>
```
damp
balance_stand
stop_move
stand_up
stand_down
sit
rise_sit
hello
stretch
wallow
scrape
front_flip
front_jump
front_pounce
dance1
dance2
finger_heart
```
</details>

### Change configurations for robot
If you want, you can modify the ways the robot walks, the height of the base, the height of the legs when walking... I show you the different parameters that can be modified:

- BodyHeight: Set the relative height of the body above the ground when standing and walking. [0.3 ~ 0.5]
  ```bash
  ros2 service call /body_height go2_interfaces/srv/BodyHeight  "height: 0.0"
  ```
- ContinuousGait: Continuous movement
  ```bash
  ros2 service call /continuous_gait go2_interfaces/srv/ContinuousGait  "flag: false"
  ```
- Euler: Posture when standing and walking. [-0.75 ~ 0.75] [-0.75 ~ 0.75] [-1.5 ~ 1.5]
  ```bash
  ros2 service call /euler go2_interfaces/srv/Euler "roll: 0.0 pitch: 0.0 yaw: 0.0"
  ```
- FootRaiseHeight: Set the relative height of foot lift during movement [-0.06 ~ 0.03]
  ```bash
  ros2 service call /foot_raise_height go2_interfaces/srv/FootRaiseHeight "height: 0.0"
  ```
- Pose: Set true to pose and false to restore
  ```bash
  ros2 service call /pose go2_interfaces/srv/Pose "flag: false"
  ```
- SpeedLevel: Set the speed range [-1 ~ 1]
  ```bash
  ros2 service call /speed_level go2_interfaces/srv/SpeedLevel "level: 0"
  ```
- SwitchGait: Switch gait [0 - 4]
  ```bash
  ros2 service call /switch_gait go2_interfaces/srv/SwitchGait  "d: 0"
  ```
- SwitchJoystick: Native remote control response switch
  ```bash
  ros2 service call /switch_joystick go2_interfaces/srv/SwitchJoystick "flag: false"
  ```

## SLAM
In the future, work in progress.

## NAVIGATION
In the future, work in progress.

## Demos
### Robot description

[go2_tf.webm](https://github.com/IntelligentRoboticsLabs/go2_robot/assets/44479765/b019e6fc-875f-4870-a510-afa6e1353e08)

### Robot PointCloud

https://github.com/IntelligentRoboticsLabs/go2_robot/assets/44479765/c164840a-6857-4d95-a50e-023c5bb44edb

## Acknowledgment
Thanks to [unitree](https://github.com/unitreerobotics/unitree_ros2) for providing the support and communication interfaces with the robot.

## About
This is a project made by the [Intelligent Robotics Lab](https://intelligentroboticslab.gsyc.urjc.es/), a research group from the [Universidad Rey Juan Carlos](https://www.urjc.es/).
Copyright &copy; 2025.

Maintainers:

* [Juan Carlos Manzanares Serrano](https://github.com/Juancams)
* [Juan S. Cely](https://github.com/juanscelyg)

## License

This project is licensed under the Apache License, Version 2 - see the [LICENSE](https://github.com/Unitree-Go2-Robot/go2_robot/blob/humble/LICENSE) file for details.
