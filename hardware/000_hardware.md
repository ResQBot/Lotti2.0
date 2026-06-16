# 🚀 Lotti 2.0 Hardware and CAD

This folder contains the mechanical CAD design, hardware documentation, and Bill of Materials (BOM) for **Lotti 2.0**, our RoboCup Rescue League robot developed for the **2026 RoboCup World Championship in Incheon, South Korea**.

The documentation explains the project structure, design philosophy, and the components used to build the robot.

---

# 📂 CAD Description

The main assembly, **`Lotti2_Main Assembly`**, contains the complete robot including the manipulator arm.

To simplify transportation and maintenance, the robot is divided into several modular assemblies.

## 🏗️ Center Module

Contains the main electronic components, including:

* Power distribution system
* Onboard computers
* Flipper mechanism motors

## ⚙️ Passive E-Module

Contains:

* Bearing-mounted shafts
* Structural drivetrain components
* Passive track support elements

No active drivetrain components are installed in this module.

## 🔩 Active E-Module

Contains:

* Chain drive motors
* Track drivetrain
* Transmission shafts
* Power transmission components

## 🤖 Manipulator Arm Assembly

Contains:

* Complete robotic arm
* End-effector mounting structure
* Arm electronics and sensors
* Mechanical transmission components

---

## 🎯 Design Philosophy

The modular design provides several advantages:

* Easier transportation
* Faster assembly during competitions
* Simplified maintenance
* Rapid replacement of damaged modules
* Improved accessibility to electronics and drivetrain components

---

# 📋 Bill of Materials (BOM)

## Main Robot

| Item No. | Component                              | Qty. | Notes                                      | Manufacturer / Supplier                                                     |
| -------- | -------------------------------------- | ---- | ------------------------------------------ | --------------------------------------------------------------------------- |
| 1        | CubeMars AK10-9 V2.0 KV60              | 2    | Chain drive motors                         | https://www.cubemars.com/de/product/ak10-9-v2-0-kv60-robotic-actuator.html  |
| 2        | Unitree GO-M8010-6 Motor               | 4    | Flipper mechanism                          | https://shop.unitree.com/products/go1-motor                                 |
| 3        | Aluminum Profile 20x20 Type B Slot 6   | –    | Center module frame                        | https://www.motedis.at/de/Aluprofil-20x20-B-Typ-Nut-6                       |
| 4        | Internal Corner Bracket 21.6×21.6×15.8 | 16   | Structural frame                           | https://3d24.com/Winkel-Innenwinkel-21-6x21-6x15-8-Stahlguss-verzinkt/12560 |
| 5        | M4 T-Slot Nut Type B Slot 6            | 70   | Frame assembly                             | https://3d24.com/Nutenstein-M4-Nut-6-Typ-B-mit-Steg-Stahl-verzinkt/10697    |
| 6        | MINIX Z300-0dB                         | 1    | Main compute unit                          | https://www.minix.com.hk/products/minix-z300-0db-fanless-mini-pc-eu         |
| 7        | Greenworks G24B4 Battery               | 6    | Used with custom battery management system | https://greenworkstools.eu/products/24v-battery-4ah                         |
| 8        | Timing Belt                            | 4    | –                                          | –                                                                           |
| 9        | Waterjet-Cut Parts                     | –    | Custom manufactured                        | –                                                                           |
| 10       | Tower Profile                          | –    | Custom structural profile                  | –                                                                           |
| 11       | Emergency Stop Switch                  | 1    | Safety system                              | https://www.conrad.at                                                       |
| 12       | Cameras                                | –    | Vision system                              | –                                                                           |
| 13       | Livox MID-360                          | 1    | LiDAR sensor                               | https://www.livoxtech.com/de/mid-360                                        |
| 14       | HDMI Bulkhead Connector                | 1    | External access port                       | Amazon                                                                      |
| 15       | USB Extension                          | 1    | External access port                       | –                                                                           |
| 16       | LAN Extension                          | 1    | External access port                       | –                                                                           |
| 17       | PoolCare Track Belt                    | 4    | Flipper chain                              | https://www.hellopool.de/de/p/11620                                         |
| 18       | Waveshare USB-to-CAN Adapter           | 1    | CAN communication                          | Amazon                                                                      |
| 19       | XT60 Connector                         | 2    | Power interface                            | https://www.reichelt.at                                                     |
| 20       | M12 Sensor Cable (4-pin)               | 1    | Sensor connection                          | https://www.reichelt.at                                                     |
| 21       | Amphenol M12A-04PFFP-SF8001 Socket     | 1    | External interface                         | https://www.mouser.at                                                       |

---

## 🦾 Manipulator Arm

| Item No. | Component                          | Qty.   | Notes                      | Manufacturer / Supplier                              |
| -------- | ---------------------------------- | ------ | -------------------------- | ---------------------------------------------------- |
| 1        | Sweep Dynamics Cricket Drive MK II | 6      | Gearboxes                  | https://www.sweepdynamics.com/products/8247658741794 |
| 2        | NEMA 17 (23 mm)                    | 2      | Joint actuators            | https://www.omc-stepperonline.com                    |
| 3        | NEMA 17 Stepper Motor              | 4      | Joint actuators            | –                                                    |
| 4        | MakerBase Servo42D                 | 6      | Closed-loop stepper driver | https://makerbase3d.com                              |
| 5        | RS485 Dongle                       | 1      | Communication interface    | –                                                    |
| 6        | Aluminum Profile                   | 500 mm | Arm structure              | –                                                    |
| 7        | Aluminum Profile                   | 500 mm | Arm structure              | –                                                    |
| 8        | Camera                             | 1      | Visual feedback            | –                                                    |
| 9        | Thermal Camera                     | 1      | Victim detection           | –                                                    |
| 10       | Magnetic Sensor                    | 1      | Position sensing           | –                                                    |
| 11       | ESP Controller                     | 1      | Arm control electronics    | –                                                    |
| 12       | M12 Cable                          | 1      | Power and communication    | –                                                    |
| 13       | M12 Socket                         | 1      | External connection        | –                                                    |
| 14       | XT60 Connector                     | 1      | Power connection           | –                                                    |
| 15       | USB Socket                         | 1      | Data connection            | –                                                    |

---

# 🖨️ Manufacturing

Several additional custom parts are used throughout the robot.

### FDM Printed Components

Most non-load-critical components are manufactured using FDM printing with:

* Nobufil ABSx
* Nobufil PCTG
* PLA

### SLS Printed Components

The remaining actuator and precision mechanical components are manufactured using **Selective Laser Sintering (SLS)** for improved strength and dimensional accuracy.

---

# 🏆 RoboCup 2026

This robot was developed for participation in the **RoboCup Rescue League World Championship 2026** in **Incheon, South Korea**.
