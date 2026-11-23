# lotti2_audio: Zwei-Wege-Audio für ROS 2 Jazzy

Audio läuft per GStreamer (Opus über RTP/UDP). Pro Richtung gibt es eine Pipeline, gestartet über ein ROS-2-Launch-File.

- Roboter-Mikrofon → Operator (Port 5000)
- Operator-Mikrofon → Roboter-Lautsprecher (Port 5001)

## 1. Voraussetzungen

- Ubuntu 24.04 mit ROS 2 Jazzy und colcon
- PipeWire (Standard unter Ubuntu 24.04)
- Mikrofon und Lautsprecher am Roboter-PC, Netzwerkverbindung zum Operator-Rechner

## 2. Installation (auf Roboter **und** Operator-Rechner)

```bash
sudo apt update
sudo apt install gstreamer1.0-tools gstreamer1.0-plugins-base \
  gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
  gstreamer1.0-pulseaudio pulseaudio-utils pavucontrol
```

`pavucontrol` ist optional (Lautstärke prüfen). `pactl`, `parecord` und `paplay` stammen aus `pulseaudio-utils`.

## 3. Code übernehmen und bauen

Den Ordner `lotti2_audio` nach `~/Lotti2.0/src/` kopieren. Er muss so aussehen:

```
lotti2_audio/
├── launch/robot_audio.launch.py
├── lotti2_audio/__init__.py
├── resource/lotti2_audio        (leere Datei)
├── package.xml
├── setup.cfg
└── setup.py
```

Bauen:

```bash
cd ~/Lotti2.0
colcon build --packages-select lotti2_audio --symlink-install
source install/setup.bash
```

Fehlt `resource/lotti2_audio` oder `__init__.py`, mit `touch` anlegen, sonst schlägt der Build fehl.

## 4. Audiogeräte einstellen (wichtig!)

Die Gerätenamen sind **auf jedem Rechner anders**. Der Code funktioniert auf dem neuen Gerät erst nach dieser Anpassung.

```bash
pactl list short sources   # Mikrofone
pactl list short sinks     # Lautsprecher
```

- Einträge mit `.monitor` am Ende sind **keine** Mikrofone, die ignorieren.
- Der Gerätename steht in der zweiten Spalte (z. B. `alsa_input.pci-...`).
- Mikrofon und Lautsprecher mit `parecord` und `paplay` testen:

```bash
timeout 5 parecord --device=<MIC_NAME> test.wav
paplay --device=<SPEAKER_NAME> test.wav
```

Die Namen dann eintragen, entweder in `launch/robot_audio.launch.py` bei `DEFAULT_MIC` und `DEFAULT_SPEAKER`, oder beim Start per Argument:

```bash
ros2 launch lotti2_audio robot_audio.launch.py mic:=<MIC_NAME> speaker:=<SPEAKER_NAME>
```

Aktuelle Einstellung auf dem Test-Notebook:

| Funktion | Gerät |
|---|---|
| Mikrofon | `alsa_input.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp_6__source` (eingebautes 4-Kanal-Array) |
| Lautsprecher | `alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink` (Kopfhörerausgang) |

## 5. Netzwerk einstellen

- IP-Adresse des Operator-Rechners herausfinden: `hostname -I`
- Firewall auf beiden Rechnern für UDP freigeben, falls `ufw` aktiv ist:

```bash
sudo ufw allow 5000:5001/udp
```

- Beide Rechner müssen sich im selben Netz (oder VPN) befinden.

## 6. Starten

**Roboter:**

```bash
ros2 launch lotti2_audio robot_audio.launch.py operator_ip:=<OPERATOR_IP>
```

**Operator-Rechner** (zwei Terminals; Gegenstück zu den Roboter-Pipelines):

```bash
# Empfangen (Roboter-Mikrofon hören)
gst-launch-1.0 udpsrc port=5000 \
  caps="application/x-rtp,media=audio,clock-rate=48000,encoding-name=OPUS,payload=96" \
  ! rtpjitterbuffer latency=50 ! rtpopusdepay ! opusdec \
  ! audioconvert ! autoaudiosink sync=false

# Senden (eigenes Mikrofon zum Roboter)
gst-launch-1.0 autoaudiosrc ! audioconvert ! audioresample \
  ! audio/x-raw,rate=48000,channels=1 \
  ! opusenc bitrate=32000 frame-size=20 \
  ! rtpopuspay ! udpsink host=<ROBOTER_IP> port=5001
```

## 7. Test auf einem einzelnen Rechner (Loopback)

**Kopfhörer aufsetzen**, sonst gibt es Rückkopplung:

```bash
ros2 launch lotti2_audio robot_audio.launch.py \
  operator_ip:=127.0.0.1 send_port:=5001 recv_port:=5001
```

Man hört sich selbst mit etwa 0,1 s Verzögerung.

## 8. Fehlersuche

| Problem | Lösung |
|---|---|
| Stille, keine Fehler | Lautstärke und Geräte in `pavucontrol` prüfen, Gerätenamen aus Schritt 4 kontrollieren |
| Rückkopplung/Pfeifen | Kopfhörer benutzen oder Echo-Unterdrückung: `pactl load-module module-echo-cancel` |
| Fehler zu Kanälen beim 4-Kanal-Mikro | Anderes Mikrofon testen oder Pipeline auf einen Kanal umstellen |
| `no element "opusenc"` | Pakete aus Schritt 2 installieren |
| Build-Fehler "setup() was never called" | `setup.py` prüfen (`python3 setup.py --version` muss `0.0.1` ausgeben) |
| Kein Ton über das Netzwerk | IP, Ports und Firewall prüfen |
