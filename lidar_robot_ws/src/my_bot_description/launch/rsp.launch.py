import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import Command
from launch_ros.actions import Node

def generate_launch_description():
    # Pobieramy ścieżkę do naszego pakietu
    pkg_share = get_package_share_directory('my_bot_description')
    
    # Wskazujemy, gdzie jest główny plik robota
    xacro_file = os.path.join(pkg_share, 'description', 'robot.urdf.xacro')

    # Odpalamy węzeł, który przetworzy xacro i roześle go w świat ROS-a
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': Command(['xacro ', xacro_file])}]
    )

    return LaunchDescription([
        robot_state_publisher_node
    ])