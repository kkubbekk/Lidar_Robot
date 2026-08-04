cmake_minimum_required(VERSION 3.20)

list(APPEND ZEPHYR_EXTRA_MODULES "/home/kubekpc/zephyrproject/modules/lib/micro_ros_zephyr_module/modules/libmicroros")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lidar_robot_firmware)

target_sources(app PRIVATE
    src/main.c
    src/shared_data.h
    src/task_motors.c
    src/task_motors.h
    src/task_imu.c
    src/imu.c
    src/imu.h
    src/task_microros.c
)

target_link_libraries(app PRIVATE microros)