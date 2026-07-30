from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition

from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():

    pkg = get_package_share_directory(
        "my_bot_description"
    )

    use_lidar = LaunchConfiguration("use_lidar")

    lidar_arg = DeclareLaunchArgument(
        "use_lidar",
        default_value="true",
        description="Start LD06 lidar"
    )


    teleop = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                pkg,
                "launch",
                "teleop.launch.py"
            )
        )
    )


    rsp = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                pkg,
                "launch",
                "rsp.launch.py"
            )
        )
    )


    ldlidar = get_package_share_directory(
        "ldlidar"
    )


    lidar = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                ldlidar,
                "launch",
                "ldlidar.launch.py"
            )
        ),
        condition=IfCondition(use_lidar)
    )


    return LaunchDescription([
        lidar_arg,
        teleop,
        lidar,
        rsp
    ])