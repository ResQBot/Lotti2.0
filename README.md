# Lotti2.0 by Res.Q Bots Austria

Lotti2.0 is the seccond generation of our tracked **U**rban **S**earch and **E**xploration **R**obot (or USER).

We hope that any info you find in this repository helps you in your quest to develop your own USER, or robot of any other kind.
If you have any questions or want to partner up, feel free to contact us.

## Repository content:

### getting_started
contains a (more or less) detailed introduction to the whole topic of using Ubuntu and ROS, using our ROS packages and how to develop your own. This is a work in progress and you will definitely need addittional information to get deeper into ROS development.

### hardware
contains all (more or less) the hardware information and the CAD files.

### resources
contains user manuals, guidebooks, datasheets, etc. for motory, sensors, parts, ... used in our current robot.

### src 
contains our **ROS2 code** to control the robot and make use of its sensors.

### external

[#external](#external)

Enthält externe Abhängigkeiten, die als Git-Submodule eingebunden sind:

- **[athena](https://github.com/tu-darmstadt-ros-pkg/athena)** (TU Darmstadt) – wir nutzen daraus
  das ESP32-basierte E-Stop-System (`E-Stop`-Ordner im Submodule) für die Not-Aus-Funktionalität.
- **[hector_ros2_utils](https://github.com/tu-darmstadt-ros-pkg/hector_ros2_utils)** (TU Darmstadt) –
  Basis-Utility-Bibliothek, die von der E-Stop-ROS2-Node benötigt wird.

Beide Repos stehen unter MIT-Lizenz. Vielen Dank an das TU-Darmstadt-Team für die Bereitstellung!

#### Setup

Da dieses Repo Submodule enthält, beim Klonen unbedingt `--recurse-submodules` verwenden:

\`\`\`bash
git clone --recurse-submodules https://github.com/ResQBot/Lotti2.0.git
\`\`\`

Falls bereits ohne diese Option geklont wurde, nachträglich:

\`\`\`bash
git submodule update --init --recursive
\`\`\`
&nbsp;
## Disclaimer
Please remember, that we are not professionals in the field of robotics, but merely curious students fooling around. While (most) code and designs you'll find here may work for us, they might not work for you and we do not take any responsibility for potential danger arising from the use of our ideas.
