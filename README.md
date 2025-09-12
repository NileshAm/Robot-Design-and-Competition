# Robot Design and Competition
This is the project repository for module EN2533-Robot Design and Competition.</br>
This repository contains all the design files and firmware for for the robot we designed.

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