import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.conditions import IfCondition
from launch_ros.actions import Node

def generate_launch_description():

    pkg_my_bot = get_package_share_directory("my_bot_description")
    pkg_ldlidar = get_package_share_directory("ldlidar")

   
    use_lidar = LaunchConfiguration("use_lidar")
    
    lidar_arg = DeclareLaunchArgument(
        "use_lidar",
        default_value="true",
        description="Start LD06 lidar"
    )

   
    teleop = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_my_bot, "launch", "teleop.launch.py")
        )
    )

    rsp = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_my_bot, "launch", "rsp.launch.py")
        )
    )

    lidar = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(pkg_ldlidar, "launch", "ldlidar.launch.py")
        ),
        condition=IfCondition(use_lidar)
    )

   
    stm_parser_node = Node(
        package='my_bot_description',
        executable='stm_parser_node',
        name='stm_parser_node',
        output='screen'
    )

    vel_parser_node = Node(
        package='my_bot_description',
        executable='vel_parser_node',
        name='vel_parser_node',
        output='screen'
    )

   
    return LaunchDescription([
        lidar_arg,          
        rsp,              
        teleop,           
        lidar,              
        stm_parser_node,    
        vel_parser_node    
    ])