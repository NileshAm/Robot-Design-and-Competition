# Robot Design and Competition
This is the project repository for module EN2533-Robot Design and Competition.</br>
This repository contains all the [design](/Design/) files and [firmware](/Code/) for for the robot we designed.

## Final product

![Robot Image](/Design/Vizualize/Full%20Assembly.png)

<p align="center">
  <img src="/Public/74bad68f-a2de-40cd-be93-ab241adf3510.jpg" width="45%">
  <img src="/Public/f2884f0a-7d66-4f97-b89a-05fca45c3a4b.jpg" width="45%">
</p>

### Component Selection
| Category | Component | Model / Specification | Quantity | Reason for Selection |
|--------|----------|----------------------|----------|----------------------|
| Controller | Microcontroller | Arduino Mega 2560 | 1 | Mainly choosen for its high pin count to handle the sensors |
| Motor Driver | Motor Driver Module | bts7960 motor driver | 2 | Provides bidirectional control and sufficient current for DC motors |
| Actuator | JGA25-370 12V with encoders | 12V, 280 RPM | 2 | High torque suitable and encoder for precision control of the  robot |
| Battery | 11.1V LiPo |  4800mah | 1 | Lightweight with high energy density |
| Power Regulation | Voltage Regulator | Buck Converter (LM2596),  | 2 | Steps down battery voltage safely for electronics (2 is used for a electronics rail and a high power rail seperated for electronics stability.) |
| Power Regulation | Voltage Regulator | 12V Buck Boost converter,  | 1 | To regulate the motor voltage to be at constant 12V. |
| Sensor | Distance Sensor | VL53LOX Tof Sensors | 5 | Obstacle detection |
| Sensor | Line Sensor | IR Reflective Sensor | 8 | Line-following capability |
| Mechanical | Chassis | 3D Printed | 1 | Strong, lightweight, and custom-fit |
| Mechanical | Wheel | Rubber Wheel (65 mm) | 4 | Good traction and stability |
| Interface | Display | OLED | 1 | System status and debugging feedback |
| Safety | Switch | Toggle / Push Button | 1 | Safe power isolation |

## Folder structure

- [Design](/Design/) : Contains all the robot design files
  - [Parts](/Design/Parts/) : Contains all part files of the design.
  - [Assemblies](/Design/Assemblies/) : Contains all the assembly files.
  - [Drawings](/Design/Drawings/) : Contains all the enginnering drawing files.
  - [Manufacturing](/Design/Manufacturing/) : Contains all the files required to manufacture the robot.

- [Code](/Code/) : Contains all the firmware for the robot.
  - [include](/Code/include/) : Project headers for files. ([/include/README.md](/Code/include/README.md))
  - [lib](/Code/lib/) : Project specific component files. ([/lib/README.md](/Code/lib/README.md))
  - [src](/Code/src/) : Main file of the project.
  - [test](/Code/test/) : Test files of the project. ([/test/README.md](/Code/test/README.md))

## Branching Structure

- **Main Branch** : All the design and firmware files.
- **Design branch** : Changes made to the design of the robot is done here
  - **Design sub-branch** : Individual changes of the desgin of the robot are done in these braches.<br>
    - Name must follow the convention : `design-*`<br>
      - Example : `design-arm`

    - After finishing the sub-branch design, branch must be merged with the **Design branch** with and the with the **Main branch**.
  - **Code sub-branch** : Individual changes of firmware of the robot are done in these braches.<br>
    - Name must follow the convention : `code-*`<br>
      - Example : `code-PID`

    - After finishing the sub-branch, branch must be merged with the **Code branch** with and the with the **Main branch**.