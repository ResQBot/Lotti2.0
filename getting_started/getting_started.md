# Res.Q Bots Setup
Dieses Tutorial ist eine Schritt-für-Schritt Anleitung, um die komplette Softwaresuite des Res.Q Bots Teams einzurichten und mit dem Coden zu starten. Es werden keinerlei Vorkenntnisse benötigt.



**Disclaimer:**  
This REEADME is meant for internal use by Res.Q Bots members. While we do not forbid anyone else from using it, we want to caution you, that this file is not complete and updated infrequently. It is by no means a reliable source to work with, if you are not in direct contact with one of the people who have been working on it. Most of the packages used are open source packages developed by other people, who know what they are doing way better than us.
We have done our best to link to the original GitHubs for any non standard ROS packages.




<!-- ##################################################################################
#######################################################################################
#######################################################################################
#######################################################################################
####################################################################################-->
&nbsp;
# Inhalt
[Betriebssystem](#betriebssystem)   
[Ubuntu Grundlagen](#ubuntu-terminals)  
&nbsp;&nbsp;&nbsp;&nbsp; [Ubuntu Terminal Befehle](#wichtige-befehle)  
&nbsp;&nbsp;&nbsp;&nbsp; [ROS2 Befehle](#ros2-befehle)  
&nbsp;&nbsp;&nbsp;&nbsp; [Nützliche Programme](#nützliche-programme)  
[ROS2 Installieren](#ros2-installieren)  
[ROS2 Packages installieren und einrichten](#ros2-packages-installieren-und-einrichten)  
&nbsp;&nbsp;&nbsp;&nbsp; [Package Liste](#package-liste)  
&nbsp;&nbsp;&nbsp;&nbsp; [Steuerung](#steuerung)  
&nbsp;&nbsp;&nbsp;&nbsp; [Sensorik](#sensorik)  
&nbsp;&nbsp;&nbsp;&nbsp; [Lidar](#lidar-livox-mid-360)  
&nbsp;&nbsp;&nbsp;&nbsp; [Anzeige](#anzeige)  
&nbsp;&nbsp;&nbsp;&nbsp; [Development-Tools](#development-Tools)    
[Packages entwickeln](#packages-entwickeln)  
&nbsp;&nbsp;&nbsp;&nbsp; [Grundlagen](#grundlagen)  
&nbsp;&nbsp;&nbsp;&nbsp; [ros2_control Packages](#ros2_control-packages)  
&nbsp;&nbsp;&nbsp;&nbsp; []()  
&nbsp;&nbsp;&nbsp;&nbsp; []()  


<!-- TODO
	Links vervollständigen
 -->

<!-- ##################################################################################
#######################################################################################
#######################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
# Betriebssystem
Aktuell verwenden wir **Ubuntu 24.04 LTS** als OS, da dies die Basis für **ROS2 Jazzy** bildet.  

### Voraussetzungen
Internetverbindung  
USB Stick mit min 8 GB Speicher  


### Installation
Zuerst muss das **Ubuntu 24.04 Desktop** Image von der Ubuntu Website heruntergeladen werden.  
Danach ein Tool zum Erstellen eines USB-Startermediums installieren. z.B. **Rufus** oder **Balena-Etcher**  
Diese Tools haben auf ihren Webseites normalerweise Anleitungen, wie sie zu verwenden sind. Hier kann nicht viel falsch gemacht werden, solange **Ubuntu 24.04 Desktop** als image gewählt wird.

Für den PC im Roboter sollte die **Server** Variante des OS gewählt werden, da dieser ohne Bildschirm verwendet wird und dadurch Rechenleistung gespart werden kann, sowie die verwendung von **Remote-Tools** vereinfacht wird. 


<!-- ##################################################################################
#######################################################################################
#######################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
# Ubuntu Grundlagen
Ein sehr vereinfachtes und unvollständiges Intro in die **Linux Shell**, das den Umgang mit **Befehlsterminals** lehren und erklären soll.   
In Ubuntu lässt sich alles mit Terminalbefehlen erledigen. Gerade fürs Programmieren und den Umgang mit ROS ist es von enormer Bedeutung das Terminal nutzen zu können. 


&nbsp;
## Terminal UI
Die Tastenkombination `Strg`+`Alt`+`T` öffnet ein neues Terminal. Links in der aktuellen Zeile wird angezeigt, welcher **User** den Befehl ausführt, auf welchem **Host** der Befehl ausgeführt wird und in welchem **Verzeichnis** man sich gerade befindet, z.B.:

	resqbots@Q-T-Pi:~$ echo "hello world"

`resqbots` ist der **User**   

`Q-T-Pi` ist der **Host** ( = das Gerät auf dem der User angemeldet ist)

`~` ist das default Verzeichnis beim einloggen

`$` ist ein Trennsymbol, danach fängt der Befehl an

`echo "hello world"` ist der Befehl, der ausgeführt werden soll


&nbsp;
## Wichtige Befehle

### Strg + C
Die Tastenkombination `Strg` + `C` bricht den aktuell laufenden Prozess ab. Dies ist besonders wichtig, um ROS-Packages zu stoppen.


<!--################################################################################-->
&nbsp;
### cd
`cd <Verzeichnisname>` wechselt vom aktuellen Verzeichnis in das gewälte Unterverzeichnis. Mit `/` können mehrere Ebenen an Unterverzeichnissen aneinander gereiht werden. z.B.:

	cd ros_ws/src/resqbots_drive_interface/

Der Verzeichnisname `..` wechselt ein Verzeichnis nach "oben". So kommt man mit

	cd ..

wieder in das Verzeichnis `ros_ws/src/` und mit 

	cd ../..
wäre man im Verzeichnis `ros_ws`

Der Befehl `cd` ohne Verzeichnisnamen führt immer in das Standardverzeichnis zurück, egal wo man vorher war.

Das `~` Symbol ist ein Platzhalter für den "standard" Arbeitsbereich. Wenn man also beispielsweise aus dem Verzeichnis `lidar_ws/src` in das Verzeichnis `moveit2_ws/src` wechseln will, gelingt das mit:

	cd ~/moveit2_ws/src


<!--################################################################################-->
&nbsp;
### sudo
`sudo` steht kurz für "super user do" und entspricht dem **"Als Administrator ausführen"** bei Windows. Viele Befehle können nur mit dem Vorsatz `sudo` ausgeführt werden. Auch geschützte Dateien können nur mit `sudo` verändert werden.


<!--################################################################################-->
&nbsp;
### apt install
`apt install <Paketname>` wird quasi ausschließlich mit `sudo` gemeinsam verwendet und installiert das gewünschte Paket.

	sudo apt install hollywood

installiert das hollywood Paket. (Ein netter Scherz, wenn man unwissende beeindrucken will.)


<!--################################################################################-->
&nbsp;
### mkdir
`mkdir <Verzeichnisname>` erstellt das gewünschte Verzeichnis. 

	mkdir Test

erstellt das Verzeichnis `Test`

Der zusatz `-p` sorgt dafür, dass alle Übergeordneten Verzeichnisse unverändert bleiben. Mit `-p` können auch Unterverzeichnisse in einem Befehl mit-genereirt werden.

	mkdir -p ros_ws/src

erstellt das Verzeichnis `src` im Verzeichnis `ros_ws`. Falls das Verzeichnis `ros_ws` nicht existiert, wird es ebenfalls erstellt.


<!--################################################################################-->
&nbsp;
### rm
`rm <Dateiname>` wird eigentlich nur mit `sudo` davor verwendet. `sudo rm <Dateiname>` löscht die benannte Datei.
Mit dem zusatz `-r` können auch ganze Verzeichnisse gelöscht werden.

	sudo rm -r ros_ws

löscht den oben erstellten `ros_ws` und **alle Unterverzeichnisse**


<!--################################################################################-->
&nbsp;
### nano
`nano` ist eine Texteditor von Ubuntu. Damit können nahezu alle schreibbaren Dateiformate göffnet und bearbeitet werden. Für schreibgeschützte Dateien musst `sudo` vorgesetzt werden.  
Mit `nano <Dateiname>` wird die benannte Datei geöffnet, oder, falls sie nicht existiert, erstellt. Mit `sudo nano` erstellte Dateien sind für "normale" User schreibgeschützt und können nur mit `sudo` bearbeitet werden.

	sudo nano README.txt

erstellt z.B. eine schreibgeschützte `README.txt`.


<!--################################################################################-->
&nbsp;
### Tab
Mit der `Tab` Taste können befehle automatisch vervollstöndigt werden.  
Mit einem doppelten `Tab` werden alle verfügbaren Optionen angezeigt.


<!--################################################################################-->
&nbsp;
### ssh
`ssh user@host` stellt eine Remoteverbindung zum gewählten Gerät her und loggt sich als der genannte User ein. Der **Host** kann dabei druch die **IP-Adresse** oder den **Gerätenamen** mit dem zusatz `.local` angegeben werden.

	ssh resqbots@192.168.1.42

	ssh resqbots@q-t-pi.local

loggt sich z.B. als user **resqbots** auf unserem RaspberryPi namens **Q-T-Pi** ein.  

Für das wiederholte Einloggen auf dem gleichen Gerät, sollte mit

	ssh-keygen

ein **ssh public key** erstellt werden, der dann mit

	ssh-copy-id user@host

auf das Zielgerät kopiert wird. Dadurch ist bei der Verbindung mittels `ssh` kein Passwort mehr nötig und es können z.B. skripte geschrieben werden, die durch die Remoteverbindung arbeiten.


<!--################################################################################-->
&nbsp;
### pkill
`pkill` kann ausgeführte Programme und ROS Nodes beenden, wenn diese nicht auf `strg+c` reagieren.  

	pkill --signal 2 -f /Pfad/zu/install/Verzeichnis

`--signal 2` schickt ROS Nodes das standard Signal zum Runterfahren. Das ist der sanfteste Weg, eine Node zu beenden. (z.B. wenn das Fenster dazu geschlossen wurde, ohne die Node vorher zu schließen.)  
`-f /Pfad` gibt den Pfad zum `/install` Ordner des Prozesses an, der geschlossen werden soll.


<!--################################################################################-->
&nbsp;
### ./ + Dateiname 
`./<Dateiname>` führt das gewählte **Shell-Skript** aus, falls das möglich ist. In einem Skript können z.B. mehrere Befehle aneinander gereiht sein, oder Befehle mit vielen Optionen ausgeführt werden.  
**Shell-Skripte** sind durch die Endung `.sh` gekennzeichnet und beginnen mit der Zeile `#!/bin/bash`  


<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
## ROS2 Befehle

<!--################################################################################-->
### ros2 run & ros2 launch
`ros2` ist ähnlich wie `sudo` eine Vorsilbe, die der Shell sagt, dass sie das Programm mit der ROS2 Umgebung ausführen soll.  
`run` und `launch` sind zwei Varianten, Programme (Packages) in ROS zu starten. `run` startet das Programm in seiner einfachsten Form. `launch` startet das **launch file** des Packages und kann oft mit einer vielzahl von Optionen versehen werden.

	ros2 run joy joy_node

startet z.B. das ROS2-Standardprogramm für Controller (X-Box/Playstation/etc.). `joy` ist der Name des Programms, `joy_node` ist die Node (also die Funktion), die ausgeführt werden soll.

	ros2 launch tele_op operator.launch.py

startet unser `tele_op` Package und die `joy_node`. Im `operator.launch.py`-Launch-File steht, welche Nodes mit welchen Optionen gestartet werden sollen.  
Die meisten launch Befehle enden auf `.launch` und `.py` oder `.cpp`. `.launch` signalisiert, dass es sich um eine Launch-Datei handelt, `.py` zeigt, dass es eine **Python**-Datei ist, `.cpp` steht für **C++**.


<!--################################################################################-->
&nbsp;
### weitere ROS2 Befehle
`ros2 node list` -> Liste aktiver ROS Nodes  
`ros2 topic list` -> Liste aktiver ROS Topics  
`ros2 topic echo` -> Zeigt angegebenes Topic an  
`ros2 service list` -> Liste verfügbarer ROS Services  
`ros2 contol *` -> ale Befehle im zusammenhang mit ros2_control (Liste der Hardwareinterfaces/Controller, Status der Interfaces/Controller, etc)  
`ros2 run rqt_graph rqt_graph` -> Zeigt eine graphische Darstellung aller aktiven Nodes, Services, Topics, etc
`sudo apt install ros-jazzy-PackageName` -> installiert das genannte Package



<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
## Nützliche Programme
`network-manager` -> Netzwerk-Management  
`net-tools` -> Netzwerkadapter  
`iperf3` -> Netzwerkgeschwindigkeit  
`nmap` -> Geräte im Netzwerk finden




<!-- ##################################################################################
#######################################################################################
#######################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;
# ROS2 installieren
Wir verwenden auf unseren Rechnern `ROS2 Jazzy`. Für die Installation folgt am besten der [offiziellen ROS2 Jazzy Installationsanleitung](https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html)


### ROS einrichten:
Alle Packages in Ros müssen **gesourced** werden, damit sie von der Shell gestartet werden können.
Um ein Package schnell in der aktuellen Shell zu sourcen kann der Befehl 

	source $package_ws$/install/setup.bash 

verwendet werden. Wobei `$package_ws$` durch den Pfad zum Ordner, in dem der `colcon-build` Befehl ausgeführt wurde, ersetzt werden muss. (z.B. ~/ros_ws) 
Falls man den `colcon-build` Befehl im aktuellen Ordner verwendet hat, reicht `source install/setup.bash`.

Um ein Package permanent zu sourcen muss der Befehlt in die **~/.bashrc** Datei geschrieben werden. Das ist eine Setup Datei, die der Shell sagt, was sie vor dem Start alles machen muss. (z.B. ROS sourcen, oder die Hintergrundfarbe ändern.) 

	echo "source /opt/ros/jazzy/setup.bash" >> ~/.bashrc

Die **DOMAIN_ID** sagt ROS welche Geräte in deinem Netzwerk zur gleichen Gruppe gehören. Sie ist per Default auf **0**, kann aber verändert werden. Wir haben sie aus Spaß und zur Sicherheit auf **17** gelegt. 
Auch das muss in der Shell eingestellt werden:

	echo "export ROS_DOMAIN_ID=17" >> ~/.bashrc

**rosdep** ist ein Verzeichnis in dem viele Packages über alle aktiven ROS Distros hinterlegt sind. Manchmal hilft es, das **rosdep** zur Verfügung zu haben.

	sudo rosdep init && rosdep update

Zuletzt sollte ein **Workspace** erstellt werden, in dem an eigenen Packages gearbeitet werden kann:

	cd && mkdir -p ros_ws/src

Grundsätzlich werden ROS Packages, die nicht einfach per `sudo apt install` installiert werden können in **Workspaces** gespeichert und gebuildet, damit das Dateimanagement vereinfacht wird. Die Workspaces sinnvoll zu benennen ist dabei von Vorteil.




<!-- ##################################################################################
#######################################################################################
#######################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
# ROS2 Packages installieren und einrichten
Packages sind Programme, die im ROS2 Framework ausgeführt werden können. Für die meisten Funktionen eines Roboters gibt es bereits fertige Packages. Bei einigen müssen nur geringere Einstellungen angepasst werden, um sie mit unserem Roboter zu verwenden, andere müssen stärker angepasst werden. Falls ein eigenes Package geschrieben werden muss, kann aber auch dabei auf Grundbausteine aus der ROS2 Bibliothek zurückgegriffen werden.





### Package-Liste
&nbsp; &nbsp; **Comunity Packages**   
&nbsp; &nbsp; &nbsp; &nbsp; [**Steuerung**](#steuerung)  
&nbsp; &nbsp; &nbsp; &nbsp; [joy](#joy) -> liest Controllerdaten aus   
&nbsp; &nbsp; &nbsp; &nbsp; [robot_state_publisher](#robot-state-publisher) -> zeigt Position und Lage des Roboters an  
&nbsp; &nbsp; &nbsp; &nbsp; [ros2_control und ros2_controllers](#ros2_control-und-ros2_controller) -> Motoren ansteuern und Feedback einholen  
&nbsp; &nbsp; &nbsp; &nbsp; [moveit2](#moveit2) -> Armkinematik  
  
&nbsp; &nbsp; &nbsp; &nbsp; [**Sensorik**](#sensorik)  
&nbsp; &nbsp; &nbsp; &nbsp; [audio_common](#audio_common) -> Micro und Lautsprecher  
&nbsp; &nbsp; &nbsp; &nbsp; [camera_ros]() -> USB Kamera Interface

&nbsp; &nbsp; &nbsp; &nbsp; [**LiDAR**](#lidar-livox-mid-360)  
&nbsp; &nbsp; &nbsp; &nbsp; [Livox_SDK2](#livox_sdk2)-> LiDAR Interface  
&nbsp; &nbsp; &nbsp; &nbsp; [livox_ros_driver2](#livox_ros_driver2) -> LiDAR Interface  
&nbsp; &nbsp; &nbsp; &nbsp; [PCL](#pcl) -> support für Fast_LIO2  
&nbsp; &nbsp; &nbsp; &nbsp; [Eigen](#eigen3) -> support für Fast_LIO2   
&nbsp; &nbsp; &nbsp; &nbsp; [Fast_LIO2](#fast_lio2) -> LiDAR Mapping  
&nbsp; &nbsp; &nbsp; &nbsp; [nav2_map_server](#nav2_map_server) -> speichert LiDAR Karten

&nbsp; &nbsp; &nbsp; &nbsp; [**Anzeige**](#anzeige)  
&nbsp; &nbsp; &nbsp; &nbsp; [rviz2]() -> LiDAR Karte und robot_state  
&nbsp; &nbsp; &nbsp; &nbsp; [rqt_image_view]() -> zeigt Kamerabilder an  

&nbsp; &nbsp; &nbsp; &nbsp; **Support Packages**  
&nbsp; &nbsp; &nbsp; &nbsp; v4l2  
&nbsp; &nbsp; &nbsp; &nbsp; image-transport-plugins  
&nbsp; &nbsp; &nbsp; &nbsp; ioport  

&nbsp; &nbsp; &nbsp; &nbsp; [**Development-Tools**](#development-tools)  
&nbsp; &nbsp; &nbsp; &nbsp; [xacro]()  
&nbsp; &nbsp; &nbsp; &nbsp; [joint_state_publisher_gui]()  
&nbsp; &nbsp; &nbsp; &nbsp; []()  

&nbsp; &nbsp; **Eigene Packages**   
&nbsp; &nbsp; &nbsp; &nbsp; [lotti2_teleop]() -> verteilt User Input an die ros2_controller  
&nbsp; &nbsp; &nbsp; &nbsp; [lotti2_control]() -> ros2_control Package für den "Lotti" Roboter. Enthält das Robotermodell, Infos über alle Motoren und die Interfaces um diese anzusteuern.  
&nbsp; &nbsp; &nbsp; &nbsp; [lotti2_flipper_controller]() -> Wandelt User Input in Motorbefehle um.  





<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
## Steuerung

### joy
`joy` liest die Befehle eines standard Spielecontrollers aus und published sie im Topic `/joy`.  
`joy` ist normalerweise bereits in der ROS2 Basisinstallation enthalten. Falls `joy` aus irgendeinem Grund nicht direkt nach der ROS2 Installation läuft, kann es, wie alle Basispackages, nachträglich installiert werden.

	sudo apt install ros-jazzy-joy



<!--################################################################################-->
&nbsp;  
### robot state publisher
Der `robot_state_publisher` ist eine Node, die den aktuellen Status des Roboters published. Das ist notwendig für komplexere Bewegungsbefehle und zur visualisierung des Roboters. Der `robot_state_publisher`alleine tut nicht viel, ist aber ein notwendiger Bestandteil der ros2_control Arbeitskette.

	sudo apt install ros-jazzy-robot-state-publisher



<!--################################################################################-->
&nbsp;
### ros2_control und ros2_controller
Für die Steuerung des Roboters verwenden wir `ros2_control`. Das ist eine mächtige Controller-Interface Kombination, die extra für die Steuerung von Roboter entwickelt wurde. Für tiefere Einblicke in die Verwendung ist am besten eines der verlinkten Tutorials geeignet.

`ros2_control` und `ros2_controller` sind standard Packages in ROS2 und können nur gemeinsam verwendet werden, daher sollte man immer beide installieren.

	sudo apt install ros-jazzy-ros2-control ros-jazzy-ros2-controllers



<!--################################################################################-->
&nbsp;  
### Moveit2
`Moveit2` enthält mächtige Tools für inverse Kinematiken. Wir nutzen dieses Package um unseren Arm zu steuern.  
Auch, wenn nicht alle Komponenten von `Moveit2` benötigt werden, lohnt es sich alle zu installieren, damit sie zur Verfügung stehen.

    sudo apt install ros-jazzy-moveit*




<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
## Sensorik

### audio_common
Dieses Setup ermöglicht die Audio-Kommunikation über ROS 2 Humble zwischen einem Laptop und einem Raspberry Pi – in beide Richtungen:  
Laptop → Raspberry Pi (Audioaufnahme am Laptop, Wiedergabe am Pi)  
Raspberry Pi → Laptop (Audioaufnahme am Pi, Wiedergabe am Laptop)

**Installation: Audio-Tools und Dev-Packages**

	sudo apt install -y 
  	libasound2-dev 
  	gstreamer1.0-plugins-base 
 	libgstreamer1.0-dev 
 	liborc-0.4-dev 
  	libdw-dev libelf-dev libunwind-dev
	espeak alsa-utils pavucontrol

**ROS2 Workspace einrichten - auf BEIDEN Geräten!**

	mkdir -p ~/ros2_audio_ws/src
	cd ~/ros2_audio_ws/src
	git clone -b ros2 https://github.com/ros-drivers/audio_common.git

**Abhängigkeiten & Workspace bauen**

	cd ~/ros2_audio_ws
	rosdep install --from-paths src --ignore-src -r -y
	colcon build --symlink-install

**Kommunikation Raspberry Pi → Laptop**

**Raspberry Pi (Sender)**

	export AUDIODEV=plughw:1,0
	source ~/ros2_audio_ws/install/setup.bash
	ros2 run audio_capture audio_capture_node --ros-args -r __node:=capture_pi

**Laptop (Empfänger)**

	source ~/ros2_audio_ws/install/setup.bash
	ros2 run audio_play audio_play_node --ros-args -r __node:=play_laptop

**Verifikation & Tests**

**Verbindung prüfen (z. B. auf dem Pi):**

	ros2 topic info /audio
	
**Erwartete Ausgabe:**

	Type: audio_common_msgs/msg/AudioData
	Publisher count: 1
	Subscription count: 1

**Live-Daten anzeigen:**

	ros2 topic echo /audio

**Testausgabe senden (z. B. am Laptop):**

	espeak "Hallo Raspberry" --stdout | aplay



<!--################################################################################-->
&nbsp;
### camera_ros
Wir verwenden das `camera_ros` Package für unsere USB Kameras.  
Da `camera_ros` ein Standardpackage von `humble` ist, gestaltet sich die Installation ganz einfach:

	sudo apt install ros-jazzy-camera-ros

Die Einstellungen für die individuellen Kameras werden in `.yaml` Dateien festgehalten.  

<!-- TODO
	-besipiel .yaml datei
	-erklärung der Parameter
	-erklärung, wie kamera gestartet und .yaml geladen wird 
	-->



<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
## LiDAR: Livox MID 360
Wir arbeiten mit dem **Livox MID 360** LiDAR. Um diesen nutzen zu können müssen auf dem Gerät, das die Daten vom LiDAR auslesen soll die Packages `Livox_SDK2`  und `livox_ros_driver2` installiert sein.
Um die Daten vom LiDAR zu einer Karte zu verarbeiten, verwenden wir `Fast_LIO2`. Das wiederum benötigt `Eigen3` und `PCL` um zu funktionieren.


Die Links zu den Originalprogrammen findet ihr hier:  
**Livox_SDK2:** https://github.com/Livox-SDK/Livox-SDK  
**Fast_LIO2:** https://github.com/hku-mars/FAST_LIO2  
**livox_ros_driver2:** https://github.com/Livox-SDK/livox_ros_driver2  


&nbsp;  
Vor der Installation sollte ein **Workspace** für die Livox Packages erstellt werden:

	cd  && mkdir -p livox_ws/src 



<!--################################################################################-->
&nbsp;  
### Livox_SDK2
Das GitHub Repo der **SDK** in den **Workspaca** kopieren

	cd ~/livox_ws/src
	git clone https://github.com/Livox-SDK/Livox-SDK2.git
	
Da die **SDK** ursprünglich für frühere ROS Versionen entwickelt wurde müssen vor dem builden noch Änderungen vorgenommen werden. Dazu wird die Zeile `#include <cstdint>` in den Bereichen mit den anderen `#include` Zeilen in den Dateien `sdk_core/comm/define.h` und `sdk_core/logger_handler/file_manager.h`  eingefügt: 

	cd Livox-SDK2
	nano sdk_core/comm/define.h
	nano sdk_core/logger_handler/file_manager.h
	
Danach ist das Package bereit für den build-Prozess:

	mkdir build && cd build
	cmake ..
	make -j

Nach die Umsetzung des Befehls `make -j` dauert extrem lang und beim RaspberryPi hängt sich wahrscheinlich die ssh Verbindung auf. Nicht wundernn, einfach das Terminal schließen und nach ca. 15 Minuten neu verbinden und weiter machen. 

**Fals Verbindung abgebrochen**

ssh aufbauen, dann 
	
	cd livox_ws/src/Livox-SDK2/build

**weiter builden**

	sudo make install

Auch der build-Prozess dauer ewig, nicht wundern. `Livox_SDK2` ist nur ein support Package für `livox_ros_driver2` und muss nicht gesourced werden.



<!--################################################################################-->
&nbsp;  
### livox_ros_driver2 
Das GitHup Repo in den **Workspace** kopieren

	cd ~/livox_ws/src
	git clone https://github.com/Livox-SDK/livox_ros_driver2.git

und dann builden

	cd livox_ros_driver2
	./build.sh humble

Der Parameter `humble` funktioniert auch bei humble, das Package ist einfach etwas älter.

Nach dem erfolgreichen Builden das Package sourcen:

	source ~/livox_ws/install/setup.bash
	echo "source ~/livox_ws/install/setup.bash" >> ~/.bashrc
	


<!--################################################################################-->
&nbsp;  
### PCL
PCL ist eine Support-Suite, die viele Mathematische Prozesse im Bezug auf Punktewolken übernimmt.
Sie ist einfach zu installieren:

	sudo apt update
	sudo apt install pcl-*
	sudo apt install libpc-dev libpcl-ros-dev




<!--################################################################################-->
&nbsp;  
### Eigen3 
Eigen ist eine Sammlung an Header-Files, die den build Prozess anderer Packages unterstützen kann. Sie wird benötigt um `Fast_LIO2` builden zu können.

Für Eigen sollte ein extra **Workspace** erstellt werden, da es sonst zu ungewollten interaktionen mit anderen Packages kommen könnte. Der restliche Installationsprozess läuft wie immer ab.

	git clone https://gitlab.com/libeigen/eigen.git
	cd eigen
	mkdir build && cd build
	cmake ..
	make -j
	sudo make install

Auch hier ist ein **sourcen** nicht nötig.



<!--################################################################################-->
&nbsp;  
### Fast_LIO2
Zu guter Letzt kann endlich `Fast_LIO2` installiert werden.

`Fast_LIO2` ist ein Package, das LiDAR Daten zu einer Karte zusammenfügen kann, ohne dafür **Odometry** Daten zu benötigen, indem es ähnlich dem Menschen "versteht" , dass sich Wände nicht bewegen, sonder der LiDAR selbst. (Nur mit deutlich mehr Mathe dahinter.)

Auch für `Fast_LIO2` wird ein eigener **Workspace** erstellt, das das Package wieder lang zum builden braucht und nicht versehentlich neu gebuildet werden sollte, wenn man nur ein eigenes Package testen will.

	cd  && mkdir -p fast_lio2_ws/src
	cd fast_lio2_ws/src
	git clone https://github.com/Ericsii/FAST_LIO2_ROS2.git --recursive
	cd ..
	rosdep install --from-paths src --ignore-src -y

Nachdem das GitHub Repo in den **Workspace** kopiert und durch `rosdep install` erweitert wurde, muss noch die `CMakeLists.txt` Datei überarbeitet werden um mit der neuen ROS Version zu funktionieren.

	nano src/FAST_LIO2_ROS2/CMakeLists.txt

Und an allen Stellen das **c++14** und **c++17** durch **c++20** ersetzen.

Danach kann das Package mit `colcon` gebaut und im Anschluss gesourced werden.

	colcon build --symlink-install
	source install/setup.bash
	echo "source ~/fast_lio2_ws/install/setup.bash" >> ~/.bashrc

Falls das Package auf einem Gerät installiert wurde, das selbst nicht live die erstellte Karte anzeigen soll muss noch die **launch-Datei** angepasst werden.

	nano install/fast_lio2/share/fast_lio2/launch/mapping.launch.py

Die Zeile `ld.add_action(rviz_node)` mit **#** auskommentieren.



<!--################################################################################-->
&nbsp;  
### Nav2_map_server
`Nav2` ist eine Package Suite für die Roboternavigation. Das Subsystem `nav2_map_server` verwenden wir, um die in Fast_LIO2 erstellten Karten zu speichern.

**Installation:**

	sudo apt update
	sudo apt install ros-jazzy-nav2-map-server

Der Nav2_map_server wird beim Start von Fast_LIO2 automatisch mit gestartet.



<!--################################################################################-->
&nbsp;  
&nbsp;  
### LiDAR Einrichten
Um den Lidar verwenden zu können müssen noch ein paar Schritte erledigt werden.

Die `livox_ros_driver2` kann nur mit **statischen IP Adressen** arbeiten. Daher muss im Gerät, das die LiDAR Daten auslesen soll, **DHCP** ausgeschaltet werden. Mit

	cd /etc/netplan/
	ls

werden alle Dateien im Verzeichnis `netplan` angezeigt. Normalerweise ist es nur eine, die `50-cloud-init.yaml` oder so ähnlich heißt. Diese `.yaml` Dateien sind schreibgeschützt, müssen also mit `sudo` geöffnet werden.

	sudo nano <Dateiname>

Das Terminal sollte nun ca. so aussehen:

	network:
		wifis:
			wlan0:
				dhcp4: true
				

Unter dem Existierenden Abschnitt für `wifis` den folgenden Abschnitt einfügen:

	ethernets:
        eth0:
            dhcp4: false
            addresses:
            - 192.168.1.50/24
            optional: true

Nach dem Speichern und Verlassen der Datei müssen die Änderungen übernommen werden:

	sudo netplan apply

Jetzt hat das Gerät eine **statische IP** von 192.168.1.50 im **Ethernet**. Das Gerät kann jetzt nicht mehr über Ethernet angesteuert werden. Wird das benötigt, einfach das `dhcp4: false` durch `dhcp4: true` ersetzen.



Jetzt muss in der **Konfig-Datei** die **IP Adresse** festgehalten werden.
Der Befehlt

	nano livox_ws/install/livox_ros_driver2/share/livox_ros_driver2/config/MID360_config.json 

zeigt die `MID360_config.json` Datei. Sie sieht nach der ersten Installation wie folgt aus:

<pre>
{
"lidar_summary_info" : {
	"lidar_type": 8
},
"MID360": {
	"lidar_net_info" : {
	"cmd_data_port": 56100,
	"push_msg_port": 56200,
	"point_data_port": 56300,
	"imu_data_port": 56400,
	"log_data_port": 56500
	},
	"host_net_info" : {
	"cmd_data_ip" : "192.168.1.5",
	"cmd_data_port": 56101,
	"push_msg_ip": "192.168.1.5",
	"push_msg_port": 56201,
	"point_data_ip": "192.168.1.5",
	"point_data_port": 56301,
	"imu_data_ip" : "192.168.1.5",
	"imu_data_port": 56401,
	"log_data_ip" : "",
	"log_data_port": 56501
	}
},
"lidar_configs" : [
	{
	"ip" : "192.168.1.12",
	"pcl_data_type" : 1,
	"pattern_mode" : 0,
	"extrinsic_parameter" : {
		"roll": 0.0,
		"pitch": 0.0,
		"yaw": 0.0,
		"x": 0,
		"y": 0,
		"z": 0
	}
	}
]
}
</pre>

Im Abschnitt `"MID360":` überall die **IP Adresse** `192.168.1.5` durch die `192.168.1.50` ersetzen. 

Im Abschnitt `"lidar_configs" :` kann die **IP Adresse** des LiDARS eingestellt werden. Um diese zu erfahren, den LiDAR mit einem Netzwerk verbinden, einen PC mit dem gleichen Netzwerk verbinden und

	sudo apt update && sudo apt install nmap
	ip a 	#zeigt deine IP Adresse
	nmap <deine IP Adresse> 	# nur die letzte Zahl ersetzen durch 0/24 

In der Liste sollte irgendwo der LiDAR auftauchen. Die **Default IP** unseres LiDARs ist `192.168.1.196`. 



<!--################################################################################-->
&nbsp;  
### Lidar Optionen
In der Datei `*/src/FAST_LIO2_ROS2/config/mid360.yaml` können Einstellungen für den LiDAR Betrieb vorgenommen werden. `*` steht für die übergeordneten Verzeichnisse. In unserem Fall wahrscheinlich `~/fast_lio2_ws`

	nano ~/fast_lio2_ws/src/FAST_LIO2_ROS2/config/mid360.yaml

Gibt folgendes aus:

<pre>
/**:
	ros__parameters:
		feature_extract_enable: false
		point_filter_num: 3
		max_iteration: 3
		filter_size_surf: 0.5
		filter_size_map: 0.5
		cube_side_length: 1000.0
		runtime_pos_log_enable: false
		map_file_path: "./test.pcd"

		common:
			lid_topic:  "/livox/lidar"
			imu_topic:  "/livox/imu"
			time_sync_en: false         # ONLY turn on when external time synchronization is really >
			time_offset_lidar_to_imu: 0.0 # Time offset between lidar and IMU calibrated by other al>
										# This param will take effect no matter what time_sync_en is>

		preprocess:
			lidar_type: 1                # 1 for Livox serials LiDAR, 2 for Velodyne LiDAR, 3 for ou>
			scan_line:  4
			blind: 0.5
			timestamp_unit: 3
			scan_rate: 10

		mapping:
			acc_cov: 0.1
			gyr_cov: 0.1
			b_acc_cov: 0.0001
			b_gyr_cov: 0.0001
			fov_degree:    360.0
			det_range:     100.0
			extrinsic_est_en:  true      # true: enable the online estimation of IMU-LiDAR extrinsic
			extrinsic_T: [ -0.011, -0.02329, 0.04412 ]
			extrinsic_R: [ 1., 0., 0.,
							0., 1., 0.,
							0., 0., 1.]

		publish:
			path_en: true                # true: publish Path
			effect_map_en: false         # true: publish Effects
			map_en: true                 # true: publish Map cloud
			scan_publish_en:  true       # false: close all the point cloud output
			dense_publish_en: false      # false: low down the points number in a global-frame point>
			scan_bodyframe_pub_en: true  # true: output the point cloud scans in IMU-body-frame

		pcd_save:
			pcd_save_en: true
			interval: -1                 # how many LiDAR frames saved in each pcd file; 
										# -1 : all frames will be saved in ONE pcd file, may lead to>
</pre>


Bei `map_file_path:` den gewünschten Pfad + Dateinamen eingeben z.B. `"~/maps/latest_map.pcd"`

`scan_rate:` kann auf 10, 30, 50 oder 100 gesetzt werden und steht für die Abtastrate des LiDAR. **Vorsicht:** je höher die `scan_rate` desto mehr Rechenleistung wird für die Karte benötigt und desto schneller füllt sich der Arbeitsspeicher.



<!--################################################################################-->
&nbsp;  
&nbsp;  
### Lidar verwenden
**Lidar Starten**  
Auf dem Laptop im Terminal im `/home directory`

	./lidar_launch.sh	

eingeben, für automatischen Start.  
Oder auf dem Mini PC 2 Terminals öffnen

	Terminal1: ros2 launch livox_ros_driver2 msg_MID360_launch.py
	Terminal2: ros2 launch fast_lio2 mapping.launch.py config_file:=mid360.yaml

**Karte speichern**

Automatisch im Laptopterminal `/home directory` mit 

	/.save_map.sh
es erscheint nun eine Meldung im Terminal:

	waiting for service to become available...
	requester: making request: std_srvs.srv.Trigger_Request()

und kurz darauf folgt:
	
	response:

	std_srvs.srv.Trigger_Response(success=True, message='Map saved.')
in diesem Fall war das speichern erfolgreich.  

Falls nach einigen Sekunden keine weitere Nachricht folgt, ist ein Fehler aufgetreten.  
Die einfachsten möglichen Ursachen sind:  
- Das Lidar Programm läuft nicht  
- Die Verbindung zum MiniPC ist unterbrochen  
- Der map_saver_server läuft nicht  




<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
## Anzeige

### rviz2  
Das Modell des Roboters und die aktuelle Lidar Karte visualisieren wir mit `rviz2`.

	sudo apt install ros-jazzy-rviz2



<!--################################################################################-->
&nbsp;  
### rqt_image_view  
Für die Anzeige der Kamerabilder verwenden wir `rqt_image_view`. Es ist Teil der `rqt`-Suite, die für viele Funktionen in ROS2 hilfreich ist. Sie sollte bei der ROS Basisinstallation bereits heruntergeladen sein. Falls nicht kann entweder nur `rqt_image_view` oder die gesamte rqt-Suite installiert werden:

	sudo apt install ros-jazzy-rqt-image-view
	
	sudo apt install-ros-jazzy-rqt-*




<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
## Development-Tools

### xacro
`xacro` ist eine Erweiterung, die es erlaubt, eine `.URDF` auf mehrere Dateien aufzuteilen. Für mehr Infos siehe Abschnitt [URDF](#urdf). `xacro` ist kein ROS2 Package und muss deshalb installiert werden.

	sudo apt install ros-jazzy-xacro



<!--################################################################################-->
&nbsp;  
### joint_state_publisher_gui
Das `joint_state_publisher_gui` startet ein graphisches Interface mit Schiebereglern für alle momentan gepublishten Joints. Durch verschieben der Regler wird der Zustand des Joints angepasst. Dieses Tool hilft beim testen einer neuen oder angepassten `URDF` und hilft bei der Fehlersuche, falls ein Robotermodell nicht wie gedacht funktioniert.

	sudo apt install ros-jazzy-joint-state-publisher-gui








<!-- ##################################################################################
#######################################################################################
#######################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
# Packages entwickeln

### Grundlagen
Um von Null an zu lernen, wie man ein ROS2 Package entwickelt sollten die [Tutorials](https://docs.ros.org/en/humble/Tutorials.html) der ROS2 Dokumentation abgearbeitet werden. Danach sind die Basics klar(er) und es kann mit der Entwicklung komplexerer Programme fortgefahren werden.



<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
## ros2_control Packages

<!-- TODO
	Einleitung in ros2_control
	Tutorials verlinken
 -->

Für ros2_control Packages ist eine gewisse Ordnerstruktur notwendig, damit das System alle Komponenten findet. Der folgende Block zeigt eine Beispielstruktur, in der jede Enrückung eine niedrigere Ordnerebene darstellt. Bezeichnungen ohne Dateiendung sind Ordner. Die Funktionen und der Inhalt der jeweiligen Ordner werden im Verlauf des Kapitels genauer erklärt. Die wichtigsten Segmente sind mit Links versehen. 


Package_name  
&nbsp; &nbsp; &nbsp; &nbsp; bringup  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; config  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;  &nbsp; &nbsp; &nbsp; &nbsp; [controllers.yaml](#controllers.yaml)  
&nbsp; &nbsp; &nbsp; &nbsp; [launch](#launch-file-erstellen)  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; full.launch.py  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; teil.launch.py  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; ...  
&nbsp; &nbsp; &nbsp; &nbsp; description  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; meshes  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; link1.stl  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; link2.stl  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; link3.stl  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; ...  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [ros2_control](#ros2_controlxacro)  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; ros2_control.xacro  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [srdf](#srdf)  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; srdf_arm.xacro  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; srdf_body.xacro  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; ...  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [urdf](#urdf)  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Robot.urdf.xacro  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; arm.xacro  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; body.xacro  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; sensors.xacro  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; ...  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [hardware](#hardware-interfaces)  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; include  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Package_name  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; interface1.hpp  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; interface2.hpp  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; utility.hpp  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; interface1.cpp  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; interface2.cpp  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; CMakeLists.txt  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; package.xml  
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; [ros2_control.xml](#ros2_controlxml)  



<!--TODO
	Ordnerstruktur verfollständigen
-->


<!-- ###############################################################################-->
### URDF  
Das `.urdf` File enthält Informationen über alle Bauteile und Gelenke des Roboters. Ein `.urdf` File kann als grobe Abstraction manuell erstellt, oder aus einem CAD Modell exportiert werden.  
Um die Grundlagen von URDF zu verstehen, ist es ratsam, zunächst manuell ein einfachens `.urdf` File zu schreiben. Dabei helfen diese Tutorials:

[ROS2 URDF Tutorial](https://docs.ros.org/en/humble/Tutorials/Intermediate/URDF/URDF-Main.html)  
[Articulated Robotics Tutorial mit Videos](https://articulatedrobotics.xyz/tutorials/ready-for-ros/urdf/)

Ein `.urdf`File kann auch ohne eine fertige Steuerung inspiziert werden.
Falls das `.urdf` File mit `xacro` erstellt wurde, muss es erst mit

	xacro file_name.urdf.xacro > file_name.urdf

zu einem normalen `.urdf` File kompiliert werden. Danach 2 Terminals öffnen.

	1. ros2 run robot_state_publisher robot_state_publisher file_name.urdf
	2. ros2 run joint_state_publisher_gui joint_state_publisher_gui

`1.` publisht die Position und Lage des Robots, `2.` published das `/tf` Topic und ermöglicht, die Gelenke des Roboters durch Schieber zu bewegen.  




&nbsp;  
**URDF aus CAD**  

Um ein `.urdf` File **aus einem CAD Modell zu extrahieren** z.B. für SolidWorks [hier](https://github.com/ros/solidworks_urdf_exporter) das Plugin installieren und [diesem](https://wiki.ros.org/sw_urdf_exporter/Tutorials/Export%20an%20Assembly) Tutorial folgen. Nach dem Export die URDF mit einem online URDF-Viewer-Tool überprüfen (z.B. [diesem](https://viewer.robotsfan.com/) hier). Dabei auf sinnvolle Rotationsachsen und -limits achten.  

Wenn die exportierten Dateien nach der Überprüfung zufriedenstellend sind, die Ordner `meshes` und `urdf` aus der exportierten Datei in den Ordner `description` kopieren. Anschließend in der `.urdf` Datei bei allen `links` den Dateipfad für die meshes anpassen.

	<mesh filename="package://PACKAGENAME/description/meshes/DATEINAME.STL" />

Hierbei hilft es, den Abschnitt `"package://PACKAGENAME` zu markieren und alle Instanzen davon zu bearbeiten (in VS Code standardmäßig mit `strg`+`F2`).  

Bei Robotern mit vielen unabhängig beweglichen Teilen ist es sinnvoll, die URDF nach Bewegungsgruppen aufzuteilen, um die Übersicht zu behalten. Dann muss auch nicht immer die ganze URDF neu exportiert werden, wenn in der CAD Konstruktion eine Baugruppe überarbeitet wird.



<!-- ###############################################################################-->
### SRDF

<!-- TODO -->


<!-- ###############################################################################-->
### ros2_control.xacro

In dieser Datei werden alle ROS2 Hardware Interfaces aufgezählt, welche Gelenke ihnen zugeordnet sind und welche state- und command-interfaces sie claimen.  
Zusätzlich können hier Parameter hinterlegt werden, die von den Interfaces beim Start ausgelesen werden. Nach Änderungen in dieser Datei muss nicht neu gebuildet werden.  

**Beispieldatei:**

    <?xml version="1.0"?>
    <robot xmlns:xacro="http://www.ros.org/wiki/xacro" >


        <ros2_control name="Group1Name" type="system" rw_rate="50">
            <hardware>
                <plugin>robot_interface1/RobotInterface1</plugin>
                <param name="param1">value1</param>
                <param name="param2">value2</param>
            </hardware>

            <joint name="robot_joint1">
                <command_interface name="position"/>
                <command_interface name="effort"/>
                <state_interface name="position"/>
                <state_interface name="velocity"/>
                <state_interface name="effort"/>
                <state_interface name="temp"/>
            </joint>

            <joint name="robot_joint2">
                <command_interface name="position"/>
                <command_interface name="effort"/>
                <state_interface name="position"/>
                <state_interface name="velocity"/>
                <state_interface name="effort"/>
                <state_interface name="temp"/>
            </joint>
        </ros2_control>

        <ros2_control name="Group2Name" type="system" rw_rate="50">
            <hardware>
                <plugin>robot_interface2/RobotInterface2</plugin>
                <param name="param1">value3</param>
                <param name="param2">value4</param>
            </hardware>

            <joint name="robot_joint3">
                <command_interface name="position"/>
                <command_interface name="effort"/>
                <state_interface name="position"/>
                <state_interface name="velocity"/>
                <state_interface name="effort"/>
                <state_interface name="temp"/>
            </joint>

            <joint name="robot_joint4">
                <command_interface name="position"/>
                <command_interface name="effort"/>
                <state_interface name="position"/>
                <state_interface name="velocity"/>
                <state_interface name="effort"/>
                <state_interface name="temp"/>
            </joint>
        </ros2_control>
    </robot>

Der Header 

    <?xml version="1.0"?>  
    <robot xmlns:xacro="http://www.ros.org/wiki/xacro" > 
    
ist notwendig um `xacro` zu verwenden.

`<ros2_control>` zeigt dem Programm an, dass hier die Hardware Interfaces aufgelistet werden.  
Das Argumente `name="GroupName"` beschreibt, welchen Teil des Roboters die Hardware Interfaces steuern, ist aber nur inerhalb dieser Datei relevant. Falls es z.B. ein Interface für die Räder und eines für den Arm gibt, braucht man zwei ros2_control Gruppen.  
`type="system"` zeigt an, dass es sich um ein System aus Roboterteilen handelt. Andere Kategorien sind für uns vorerst irrelevant.  
`rw_rate="50"` setzt die **read-write-Rate** auf 50 Hz.  

Mit dem Wraper `<hardware>` wird die Beschreibung des Hardware Interfaces für diese Gruppe gestartet. `<plugin>` beschreibt, welches Hardware interface verwendet werden soll. Der name muss mit dem in der `ros2_control.xml` Datei übereinstimmen.  
`<param>` bestimmt die Namen der Parameter, an Stelle von `value1` kommt der Wert des Parameters. Dies kann eine Zahl oder Text sein.  

Nach dem `<hardware>` Abschnitt werden die Gelenke des Roboters aufgelistet, die das Interface steuern kann. Jedes Gelenk startet mit `<joint name="">` der Name des Gelenks muss mit dem in der URDF übereinstimmen. Nach dem Namen des Gelenks wird aufgezählt, welche **command_interfaces** und welche **state_interfaces** das Gelenk hat. **command_interfaces** sind die Befehle, die an den Motor geschickt werden können. **state_interfaces** sind Informationen, die der Motor zurück schickt.  

**Alle Abschnitte die mit `<tagXY>` begonnen wurden, müssen am ende mit `</tagXY>` geschlossen werden.**


&nbsp;  
<!-- ###############################################################################-->
### Hardware Interfaces

Im Ordner `hardware` werden alle Hardware-Interfaces angelegt. Diese sind für die Kommunikation zwischen den Controllern und den Motoren zuständig. Hier werden Umrechnungsfaktoren für Getriebe, Serielle Verbindungen und ähnliches hinterlegt. Generell muss für jede Bewegungsgruppe ein eigenes Hardware-Interface verwendet werden, selbst, wenn beide Gruppen die gleichen Motoren und Controller verwenden, sollte diese Regel beibehalten werden, um interne Konflikte zu vermeiden. Die Lotti2 Interfaces sind so umfangreich, wie möglich kommentiert, um als Vorlage dienen zu können.  
Im Ordner `include` werden die Header-Files der Interfaces angelegt. Sie benennen die Art des Interfaces und etablieren Variablen, die durch die ganze Klasse verwendung finden.


&nbsp;  
<!-- ###############################################################################-->
### ros2_control.xml

Hier werden alle in diesem Package erstellten Hardware Interfaces aufgelistet, damit der ROS2 Controller Manager erkennt, dass er sie exportieren und damit "zur Verfügung stellen" muss.

**Beispiel**

    <library path="lotti2_control">
        <!-- list all hardware interfaces that are specified in this package-->

        <class name="lotti2_flipper_interface/FlipperInterface"
          type="lotti2_flipper_interface::FlipperInterface"
          base_class_type="hardware_interface::SystemInterface">
            <description>
                Interface to drive Lotti's flippers
            </description>
        </class>

        <class name="lotti2_drive_interface/DriveInterface"
          type="lotti2_drive_interface::DriveInterface"
          base_class_type="hardware_interface::SystemInterface">
            <description>
                Interface to drive Lotti's tracks
            </description>
        </class>
  
    </library>


&nbsp;  
<!-- ###############################################################################-->
### controllers.yaml

Die controllers.yaml speichert welche Controller im Roboter zum Einsatz kommen und deren Einstellungen und Parameter. Die Grundstruktur sieht immer gleich aus:

	controller_manager:
	  ros__parameters:
	    update_rate: 50 #Hz

	    joint_state_broadcaster:
	      type: joint_state_broadcaster/JointStateBroadcaster
	
	    Bewegungsgruppe1_controller:
	      type: joint_trajectory_controller/JointTrajectoryController

	    Bewegungsgruppe2_controller:
	      type: lotti2_flipper_controller/FlipperController


	Bewegungsgruppe1_controller:
	  ros__parameters:
	    joints:
	    - joint_name_1
	    - joint_name_2
	    - joint_name_3
	    command_interfaces:
	    - position
	    - effort
	    state_interfaces:
	    - position
	    - effort
	    - velocity
	    - temp

	    parameter1: 50
	    parameter2: 2.0


	Bewegungsgruppe2_controller:
	  ros__parameters:
		joints:
		- joint_name_1
		- joint_name_2
		command_interfaces:
	    - velocity
	    state_interfaces:
	    - position
	    - effort
	    - velocity
	    - temp

	    parameter1: 50
	    parameter2: 2.0


Der Abschnitt

	controller_manager:
	  ros__parameters:
	    update_rate: 50 #Hz

	    joint_state_broadcaster:
	      type: joint_state_broadcaster/JointStateBroadcaster

ist in jedem controller.yaml file gleich. Die default-update-Rate für alle Controller wird benannt und der joint_state_broadcaster wird iniziiert.  

Danach folgen alle Controller, die beim Start des Roboters geladen werden sollen. Sie werden beim Namen genannt (Dieser dient allein der eindeutigen Identifizierung) und ihr Typ wird definiert. Der Typ muss mit einem existierenden Controllertyp übereinstimmen. Auch selbstgeschriebene Controller können hier genannt werden. (siehe z.B. lotti2_flipper_controller)

Danach werden unter den Controllernamen die ihnen zugeordneten Gelenke (Joints) sowie die Command- und State-Interfaces und ihre Parameter aufgelistet.  

Die Formatierung dieser Datei ist entscheidend und muss der Vorlage entsprechen. Beim Builden werden Formatierungsfehler und falsche Bezeichnungen in der Regel nicht besonders gut benannt. Auch beim Start des Packages wird oft nur der Hinweis 'Error in controlers.yaml' oder etwas vergleichbares eingeblendet und der Prozess schlägt fehl.

&nbsp;  
<!-- ###############################################################################-->
### launch-File erstellen

<!-- TODO -->


&nbsp;  
<!-- ###############################################################################-->
### CMakeLists.txt anpassen

<!-- TODO -->





<!-- ##################################################################################
#######################################################################################
####################################################################################-->
&nbsp;  
&nbsp;  
&nbsp;  
&nbsp;  
## Moveit2 Integration

<!--TODO
	Packagename muss "Robotername_moveit_config" sein
	setupassist starten
	package builden
	moveit_config.yaml in ros2_control Package einbinden
	service call für servo_node 
	>