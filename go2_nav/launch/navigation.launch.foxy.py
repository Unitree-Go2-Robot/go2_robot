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
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import SetRemap


def generate_launch_description():
    package_dir = get_package_share_directory("go2_nav")
    nav2_dir = get_package_share_directory("nav2_bringup")

    # Configuration Variables
    use_sim_time = LaunchConfiguration("use_sim_time")
    slam = LaunchConfiguration("slam")
    rviz = LaunchConfiguration("rviz")
    map_file = LaunchConfiguration("map")
    params_file = LaunchConfiguration("params_file")
    namespace = LaunchConfiguration("namespace")

    declare_use_sim_time_cmd = DeclareLaunchArgument(
        "use_sim_time", default_value="false"
    )

    declare_slam_cmd = DeclareLaunchArgument("slam", default_value="False")

    declare_use_rviz_cmd = DeclareLaunchArgument("rviz", default_value="False")

    declare_map_cmd = DeclareLaunchArgument("map", default_value="")

    declare_nav_params_cmd = DeclareLaunchArgument(
        "params_file",
        default_value=os.path.join(package_dir, "params", "go2_nav_params_foxy.yaml"),
    )

    declare_namespace_cmd = DeclareLaunchArgument("namespace", default_value="")

    # Actions
    localization_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_dir, "launch", "localization_launch.py")
        ),
        launch_arguments={
            "use_sim_time": use_sim_time,
            "slam": slam,
            "map": map_file,
            "params_file": params_file,
            "namespace": namespace,
        }.items(),
    )

    navigation_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_dir, "launch", "navigation_launch.py")
        ),
        launch_arguments={
            "use_sim_time": use_sim_time,
            "params_file": params_file,
            "namespace": namespace,
        }.items(),
    )

    rviz_cmd = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_dir, "launch", "rviz_launch.py")
        ),
        launch_arguments={
            "use_sim_time": use_sim_time,
            "rviz": rviz,
            "namespace": namespace,
        }.items(),
    )

    # Remappings
    cmd_vel_remap = SetRemap(src="cmd_vel_nav", dst="cmd_vel")

    ld = LaunchDescription()
    ld.add_action(declare_use_sim_time_cmd)
    ld.add_action(declare_slam_cmd)
    ld.add_action(declare_nav_params_cmd)
    ld.add_action(declare_use_rviz_cmd)
    ld.add_action(declare_map_cmd)
    ld.add_action(declare_namespace_cmd)
    ld.add_action(localization_cmd)
    ld.add_action(navigation_cmd)
    ld.add_action(rviz_cmd)
    ld.add_action(cmd_vel_remap)

    return ld
