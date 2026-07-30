from launch import LaunchDescription
from launch_ros.actions import Node

from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():

    pkg = get_package_share_directory(
        "my_bot_description"
    )

    teleop_config=os.path.join(
        pkg,
       "config",
        "teleop.yaml")

    return LaunchDescription([
        Node(
            package="joy",
            executable="joy_node",
            name="joy_node"
        ),

        Node(
            package="teleop_twist_joy",
            executable="teleop_node",
            name="teleop_twist_joy",
            parameters=[
                teleop_config
            ]
        )
    ])