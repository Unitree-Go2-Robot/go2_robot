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

from ament_index_python.packages import get_package_share_directory, PackageNotFoundError
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, LogInfo
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PythonExpression
from launch_ros.actions import Node

def generate_launch_description():
    lidar = LaunchConfiguration('lidar')
    realsense = LaunchConfiguration('realsense')
    rviz = LaunchConfiguration('rviz')
    voxelmap = LaunchConfiguration('voxelmap')
    video = LaunchConfiguration('video')

    declare_lidar_cmd = DeclareLaunchArgument(
        'lidar',
        default_value='False',
        description='Launch hesai lidar driver'
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

    declare_voxelmap_cmd = DeclareLaunchArgument(
        'voxelmap',
        default_value='False',
        description='Generate voxel map'
    )

    declare_video_cmd = DeclareLaunchArgument(
        'video',
        default_value='True',
        description='Publish front camera video'
    )    

    robot_description_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('go2_description'),
            'launch/'), 'robot.launch.py'])
    )

    driver_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('go2_driver'),
            'launch/'), 'go2_driver.launch.py'])
    )

    voxelmap_node_cmd = Node(
        package='go2_voxelmap',
        executable='voxelmap_node',
        name='voxelmap_node',
        condition=IfCondition(PythonExpression([voxelmap])),
        output='screen'
    )

    video_node_cmd = Node(
        package='image_transport',
        executable='republish',
        name='image_republish_node',
        arguments=['go2', 'compressed'],
        output='screen',
        condition=IfCondition(PythonExpression([video])),
        remappings=[
            ('in/go2', 'frontvideostream'),
            ('out/compressed', 'camera/compressed')
        ]
    )

    try:
        lidar_cmd = IncludeLaunchDescription(
            PythonLaunchDescriptionSource([os.path.join(
                get_package_share_directory('hesai_ros_driver'),
                'launch/'), 'start.py']),
            condition=IfCondition(PythonExpression([lidar])))
    except PackageNotFoundError:
        lidar_cmd = LogInfo(msg="hesai_ros_driver package not found. Skipping LiDAR launch.")

    try:
        realsense_cmd = IncludeLaunchDescription(
            PythonLaunchDescriptionSource([os.path.join(
                get_package_share_directory('realsense2_camera'),
                'launch/'), 'rs_launch.py']),
            condition=IfCondition(PythonExpression([realsense]))
        )
    except PackageNotFoundError:
        realsense_cmd = LogInfo(msg="realsense2_camera package not found. Skipping LiDAR launch.")

    try:
        rviz_cmd = IncludeLaunchDescription(
            PythonLaunchDescriptionSource([os.path.join(
                get_package_share_directory('go2_rviz'),
                'launch/'), 'rviz.launch.py']),
            condition=IfCondition(PythonExpression([rviz]))
        )
    except PackageNotFoundError:
        rviz_cmd = LogInfo(msg="no rviz package found. Skipping rviz launch.")

    ld = LaunchDescription()
    ld.add_action(declare_lidar_cmd)
    ld.add_action(declare_realsense_cmd)
    ld.add_action(declare_rviz_cmd)
    ld.add_action(declare_video_cmd)
    ld.add_action(declare_voxelmap_cmd)
    ld.add_action(robot_description_cmd)
    ld.add_action(lidar_cmd)
    ld.add_action(voxelmap_node_cmd)
    ld.add_action(video_node_cmd)
    ld.add_action(realsense_cmd)
    ld.add_action(driver_cmd)
    ld.add_action(rviz_cmd)

    return ld
