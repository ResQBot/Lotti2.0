This folder contains the mechanical CAD design and other hardware information of our RoboCup Rescue League robot Lotti 2 for the 2026 World Championship in Incheon, South Korea.

The documentation explains the file structure and design logic, and includes a complete list of the components used to build the robot.

**CAD Describtion**

The main assembly, `Lotti2_Main Assembly`, contains the complete robot, including the manipulator arm.
To simplify transportation, the robot is designed to be separated into three main sections consisting of several subassemblies:
• **Center Module**
Contains the main electronic components, including the power distribution system, onboard computers, and the motors for the flipper mechanism.

• **Passive E-Module**
Includes the supported and bearing-mounted shafts without active drivetrain components.

• **Active E-Module**
Contains the chain drive motors and the drivetrain system responsible for powering the tracks and their transmission shafts.

• **Manipulator Arm Assembly**
Contains the complete robotic arm system and its mechanical structure.

The modular design allows for easier maintenance, transportation, and rapid assembly during competitions.

- • **Center Module**
Contains the main electronic components, including the power distribution system, onboard computers, and the motors for the flipper mechanism.
- • **Passive E-Module**
Includes the supported and bearing-mounted shafts without active drivetrain components.
- • **Active E-Module**
Contains the chain drive motors and the drivetrain system responsible for powering the tracks and their transmission shafts.
- • **Manipulator Arm Assembly**
Contains the complete robotic arm system and its mechanical structure.

Bill of Materials (BOM)

| Item no. | Component | Quantity | Notes | Manufacturer |
| --- | --- | --- | --- | --- |
| 1 | CubeMars AK10-9 V2.0 KV60 | 2 | used for the chain drive | https://www.cubemars.com/de/product/ak10-9-v2-0-kv60-robotic-actuator.html |
| 2 | Unitree GO-M8010-6 Motor | 4 | used for the flipper mechanism | https://shop.unitree.com/products/go1-motor?srsltid=AfmBOoqF7fJ-rWpZs84D-cTo2KK4hBersjbRqIKsZ6-mvKW0uMsvOAq- |
| 3 | Aluprofil 20x20 B-Typ Nut 6 |  | used for the base of the center part | https://www.motedis.at/de/Aluprofil-20x20-B-Typ-Nut-6 |
| 4 | Winkel, Innenwinkel, 21,6x21,6x15,8, Stahlguß | 16 |  | https://3d24.com/Winkel-Innenwinkel-21-6x21-6x15-8-Stahlguss-verzinkt/12560 |
| 5 | Nutenstein M4, Nut 6, Typ B, mit Steg, Stahl, verzinkt | 70 |  | https://3d24.com/Nutenstein-M4-Nut-6-Typ-B-mit-Steg-Stahl-verzinkt/10697 |
| 6 | MINIX Z300-0dB | 1 | compute unit | https://www.minix.com.hk/products/minix-z300-0db-fanless-mini-pc-eu?srsltid=AfmBOoo9jLagLo2BtGUpFJwaGK-cI7xLtStRonZ72Du2wsgakh1ewfYI |
| 7 | Greenworks G24B4 | 6 | used with custom batterie management system | https://greenworkstools.eu/products/24v-battery-4ah/?shpxid=dfb51b51-d73e-40c3-97d0-671b5102df03&srsltid=AfmBOoq_UTWhgEtXbXobi-9Hnm_5ZP4SUNdu2otZ8nCrpTu7B27N71k4 |
| 8 | Riemen | 4 |  |  |
| 9 | Wasserstrahlteile |  |  |  |
| 10 | Profil für den Turm |  |  |  |
| 11 | Not-Aus Schalter | 1 |  | https://www.conrad.at/de/p/schlegel-yvooi-not-aus-schalter-250-v-ac-5-a-2-oeffner-1-schliesser-ip65-ip67-1-st-1509366.html |
| 12 | Kameras |  |  |  |
| 13 | Livox MID-360 | 1 |  | https://www.livoxtech.com/de/mid-360 |
| 14 | HDMI Einbaubuchse | 1 |  | https://www.amazon.de/HuaLiSiJi-Verlängerungskabel-Stecker-Weibliche-Computer/dp/B0D736775F/ref=sr_1_7?__mk_de_DE=ÅMÅŽÕÑ&crid=3G4ZZSLX0NY4S&dib=eyJ2IjoiMSJ9.jSHECp3ao0uaMWC1uH_anbk4iQu77r1wbjA_zMtbQ0HGV3LwF6aPKiHBgyuTDHcqr7BVlsDCXTF3tNfeTEdzyCRW4f26gOAvNBewG3HeoPoAyVJbD2_8S-9vt3ezjGoBi89w4nn7JV2wDVVC1eBlFQJM6EuG0s8OWiLFcjIcGLJtjXKiH80TFVG8fJWiWGVQvir73aKNCJCMGs-J6FlIwdVxAGIGJWH9CpT7IvrH3cM.XDwL3GrfRoQj7Az7QdAchkbHMYg-3kPx6fpRcKqOXm4&dib_tag=se&keywords=hdmi%2Bbuchse&qid=1779711161&sprefix=hdmi%2Bbuchse%2Caps%2C164&sr=8-7&th=1 |
| 15 | USB Verlängerung | 1 |  |  |
| 16 | Lan Verlängerung | 1 |  |  |
| 17 | PoolCare Raupenband | 4 | Flipper chain | https://www.hellopool.de/de/p/11620?campaign=bing&msclkid=a245a6ee695816cb2a41cf09bb1c3cf2 |
| 18 | Waveshare USB to CAN Adapter | 1 |  | https://www.amazon.de/Waveshare-Adapter-Analyzer-Communication-Raspberry/dp/B0BMQ8GCQC/ref=sr_1_4_sspa?crid=2H09759P3BWQF&dib=eyJ2IjoiMSJ9.J1suqBlPY1mDxMHkEDOfls_uHunCF7s3z1meoqnG1VRmRvxtXb1nDUCmtqGpdOynb5fG68G4Dcbvyu1ph-i8WguJvzlBkU5gHJAFMiI5YvIxyAbkjp49lqm7fYOBwFu0Wi0O1aVVlq1r_uF3PkSz2jJ-YSSi4GHaDiFTqm6xazIh0II9zSWLXiIVtU47Td2PFI3m85cfR9DGym2hZuBKhgwK17TY6YolGv48Jhx8EQEsGBgCrcSAxyYUhh7HgvlGHKY9zdmJRc6Al54OvktDcJBTXLcltAQ5MjhfgCZlHkk.6wzxQqupUDlh0bWUpm1VbnyXiG1JXflOOSMZ7l-7juw&dib_tag=se&keywords=can+adapter&qid=1779697278&sprefix=can+ada%2Caps%2C244&sr=8-4-spons&aref=CKOYjO7uIr&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1 |
| 19 | XT60 Stecker | 2 |  | https://www.reichelt.at/at/de/shop/produkt/steckverbinder_fuer_li-polymer-akkus_2-polig_xt60_zum_einbau-342797?utm_source=psuma&utm_medium=idealo.at&PROVID=2602 |
| 20 | Sensorleitung, M12, 4 pol | 1 |  | https://www.reichelt.at/at/de/shop/produkt/sensorleitung_m12_4_pol_st_offenes_ende_2_m-222989?PROVID=2807&gad_source=1&gad_campaignid=20703100232&gbraid=0AAAAADwnxtbxw-2YxY5juMjy89hMqzbHO&gclid=Cj0KCQjwy_fOBhC6ARIsAHKFB7-3YfDiCdhYjWQ-JEQthaIByKuMNydcWb5-mhEN0WvDMqI8qEbKvCgaAi9vEALw_wcB#closemodal |
| 21 | M12A-04PFFP-SF8001 Buchse | 1 |  | https://www.mouser.at/ProductDetail/Amphenol-LTW/M12A-04PFFP-SF8001?qs=Wmhir8UuqEm8W%252BswIX7fqw%3D%3D&mgh=1&vip=1&utm_id=20600662790&utm_source=google&utm_medium=cpc&utm_marketing_tactic=emeacorp&gad_source=1&gad_campaignid=20604282607&gbraid=0AAAAADn_wf36VER6e1wStrZSipBGPsL7N&gclid=CjwKCAjw-dfOBhAjEiwAq0RwI1S5ochc49ijqCUtUUBqwt_nz9yoJd18M8oduWRP7kRJxJFevq4ezRoCGFcQAvD_BwE |

There are some additional parts used, which are mostly 3D Printed using FLM printing and Nobofill ABSx or Nobofill PCTG or simbly PLA

Arm

| Item no. | Component | Quantity | Notes | Manufacturer |
| --- | --- | --- | --- | --- |
| 1 | Sweep Dynamics Cricket Drive MK II | 6 | Gears | https://www.sweepdynamics.com/products/8247658741794 |
| 2 | Nema 17 23mm | 2 |  | https://www.omc-stepperonline.com/de/e-serie-nema-17-bipolar-1-8deg-17ncm-24-07oz-in-1a-42x42x23mm-4-draehte-17he08-1004s |
| 3 | Nema 17 | 4 |  |  |
| 4 | MakerBase Servo42D | 6 | Closed Loop Stepper Motor Driver | https://makerbase3d.com/product/servo42d-nema17-closed-loop-stepper-motor-driver-cnc-3d-printer-for-gen_l-foc-quiet-and-efficient/?srsltid=AfmBOoqeTXVpP29UNL7O6KlMeFQm1iyPiRjEoJ8ijaSXMcr94vJDeQUx |
| 5 | RS485 dongle | 1 |  |  |
| 6 | Aluprofil  | 500mm |  |  |
| 7 | Aluprofil | 500mm |  |  |
| 8 | Kamera | 1 |  |  |
| 9 | Wärmebildkamera | 1 |  |  |
| 10 | Magnetsensor | 1 |  |  |
| 11 | ESP | 1 |  |  |
| 12 | M12 Kabel | 1 |  |  |
| 13 | m12 Buchse | 1 |  |  |
| 14 | XT 60 stecker | 1 |  |  |
| 15 | USB Buchse | 1 |  |  |

The remaining parts of the actuator are manufactured usind SLS 3D printing.
