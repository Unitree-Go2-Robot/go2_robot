# BSD 3-Clause License

# Copyright (c) 2024, Intelligent Robotics Lab
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.

# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.

# * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration


def generate_launch_description():
    lidar_hesai = LaunchConfiguration('lidar_hesai')
    lidar_livox = LaunchConfiguration('lidar_livox')
    realsense = LaunchConfiguration('realsense')
    rviz = LaunchConfiguration('rviz')

    declare_lidar_hesai_cmd = DeclareLaunchArgument(
        'lidar_hesai',
        default_value='False',
        description='Launch Hesai lidar driver'
    )

    declare_lidar_livox_cmd = DeclareLaunchArgument(
        'lidar_livox',
        default_value='False',
        description='Launch Livox MID 360 lidar driver'
    )

    declare_realsense_cmd = DeclareLaunchArgument(
        'realsense',
        default_value='False',
        description='Launch realsense driver'
    )

    declare_rviz_cmd = DeclareLaunchArgument(
        'rviz',
        default_value='False',
        description='Launch rviz'
    )

    def get_robot_description(context):
        return [IncludeLaunchDescription(
            PythonLaunchDescriptionSource([os.path.join(
                get_package_share_directory('go2_description'),
                'launch', 'robot.launch.py')])
        )]

    def get_driver_cmd(context):
        return [IncludeLaunchDescription(
            PythonLaunchDescriptionSource([os.path.join(
                get_package_share_directory('go2_driver'),
                'launch', 'go2_driver.launch.py')])
        )]
    
    def get_lidar_hesai_cmd(context):
        if context.perform_substitution(lidar_hesai) == 'True':
            return [IncludeLaunchDescription(
                PythonLaunchDescriptionSource([os.path.join(
                    get_package_share_directory('hesai_ros_driver'),
                    'launch', 'start.launch.py')])
            )]
        return []
    
    def get_lidar_livox_cmd(context):
        if context.perform_substitution(lidar_livox) == 'True':
            return [IncludeLaunchDescription(
                PythonLaunchDescriptionSource([os.path.join(
                    get_package_share_directory('livox_ros_driver2'),
                    'launch_ROS2', 'rviz_MID360_launch.launch.py')])
            )]
        return []

    def get_realsense_cmd(context):
        if context.perform_substitution(realsense) == 'True':
            return [IncludeLaunchDescription(
                PythonLaunchDescriptionSource([os.path.join(
                    get_package_share_directory('realsense2_camera'),
                    'launch', 'rs_launch.py')])
            )]
        return []

    def get_rviz_cmd(context):
        if context.perform_substitution(rviz) == 'True':
            return [IncludeLaunchDescription(
                PythonLaunchDescriptionSource([os.path.join(
                    get_package_share_directory('go2_rviz'),
                    'launch', 'rviz.launch.py')])
            )]
        return []

    ld = LaunchDescription()
    ld.add_action(declare_lidar_hesai_cmd)
    ld.add_action(declare_lidar_livox_cmd)
    ld.add_action(declare_realsense_cmd)
    ld.add_action(declare_rviz_cmd)
    ld.add_action(OpaqueFunction(function=get_robot_description))
    ld.add_action(OpaqueFunction(function=get_driver_cmd))
    ld.add_action(OpaqueFunction(function=get_lidar_hesai_cmd))
    ld.add_action(OpaqueFunction(function=get_lidar_livox_cmd))
    ld.add_action(OpaqueFunction(function=get_realsense_cmd))
    ld.add_action(OpaqueFunction(function=get_rviz_cmd))

    return ld
