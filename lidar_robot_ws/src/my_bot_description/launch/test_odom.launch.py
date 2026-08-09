from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='my_bot_description',
            executable='mock_stm_node',
            name="mock_stm_node",
        ),
        Node(
            package='my_bot_description',
            executable='stm_parser_node',
            name='stm_parser_node',
            output='screen'
                    
        )
    ])