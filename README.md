# Autonomously Navigating Robotic Platform for Indoor and Outdoor Navigation

Final Year Project — BS Electronic Engineering, Fatima Jinnah Women University (2023)

A four-wheeled autonomous rover that maps its surroundings and navigates to a target
location — indoors or outdoors — without any pre-planned path, using SLAM, LIDAR,
and an RGB-D camera on top of ROS2.

![Robot with full hardware setup]
"https://github.com/user-attachments/assets/154ce036-f860-4cf7-84fe-1bc360460003"

## Problem

Most industrial robots move along fixed, pre-planned paths. That works fine in a
controlled environment, but breaks down anywhere humans and machines share space —
crowded floors, changing layouts, unexpected obstacles. We wanted a robot that could
build its own map of an unknown environment in real time and plan a collision-free
route through it, using only affordable, off-the-shelf sensors instead of expensive
industrial-grade hardware.

## Approach

- **Mapping & localization:** SLAM (Simultaneous Localization and Mapping) running on
  ROS2, fusing data from an RPLIDAR A1 and a Microsoft Kinect 360 to build a live map
  of the environment and track the robot's position within it.
- **Path planning & navigation:** ROS2's Nav2 stack for obstacle-free path planning;
  the user sets a target location in RViz2 and the robot autonomously plans and
  follows a route to it.
- **Compute & control:** Raspberry Pi 4 as the onboard computer running ROS2, paired
  with an Arduino UNO (via a logic-level converter) for low-level motor control.
- **Locomotion:** Brushless DC hub motors with encoders plus a DC gear motor setup,
  driven through a JY01 99F2 motor controller and motor driver module.
- **Simulation:** The full robot was modeled (URDF) and tested in Gazebo before and
  alongside real hardware testing, to validate wheel control, camera, and LIDAR
  integration ahead of physical trials.
- **Chassis:** Custom modular chassis (redesigned after an initial hardware iteration)
  with separate compartments for electronics, a delivery payload box, sensor mounts,
  and cooling.

## Tech stack

`ROS2` `Gazebo` `RViz2` `Nav2` `SLAM` `Raspberry Pi 4` `Arduino UNO` `RPLIDAR A1`
`Microsoft Kinect 360` `Python/C++ (ROS nodes)`

## Results

- Successfully built live maps of both indoor and outdoor test environments and
  localized the robot within them in real time.
- Autonomously planned and followed collision-free paths to user-assigned target
  locations, validated in both simulation (Gazebo) and on physical hardware.
- Demonstrated a working last-mile delivery use case: user sets a destination, robot
  plans a route, navigates around obstacles, and delivers a payload from its onboard
  storage compartment.

![Robot autonomously navigating to a delivery target]
"https://github.com/user-attachments/assets/2c9454c2-16d6-49ec-8699-c948f3aaf76f"

*RViz2 view: LIDAR scan (white/magenta points), live map, and the planned path (green
arrow) toward the assigned goal.*

## Possible applications

Last-mile delivery, telepresence-based guided tours and inspections, and campus /
building patrolling — any setting where a robot needs to operate around people without
a fixed, pre-programmed path.

## Future work

- Add a GPS/IMU module for outdoor live location tracking.
- Upgrade path planning to consistently choose the shortest route, not just *a* valid
  route.
- Swap the caster wheels for hoverboard-style wheels for better payload capacity and
  control precision.
- Extend the platform with a robotic arm for pick-and-place tasks.

## Team

Ayesha Ahmed, Rimsha Kousar, Shahzadi Aniqa Zafar — supervised by Engr. Mohammad
Haroon Waseem, Department of Electronic Engineering, Fatima Jinnah Women University.

---
*Full dissertation available on request.
*
