# ROS 2 Mobile Robot Platform

A custom differential drive mobile robot bridging ROS 2 high-level control with a Zephyr-based STM32 microcontroller.

## Hardware
* **Host PC:** Dell Wyse 5070
* **MCU:** STM32 Nucleo
* **Actuators:** JGB37-520 DC motors with magnetic encoders
* **Sensors:** LD06 Lidar
* **Power:** 300W DC-DC step-down converter

## Software Stack
* **High-level:** Linux, ROS 2
* **Low-level:** Zephyr RTOS
* **Bridge:** micro-ROS

## Features
* `vel_parser_node` for custom `Twist` to wheel velocity kinematics.
* LD06 Lidar integrated with verified 2D SLAM functionality.

## TODO
* **PCB:** Design a custom power and logic distribution board.
* **Power Management:** Implement a 1-Wire Dallas bypass to resolve Dell BIOS CPU throttling.
* **Navigation:** Assemble the physical chassis and configure the Nav2 stack.
* **Camera:** Add Xbox360 Kinnect to add vision.
