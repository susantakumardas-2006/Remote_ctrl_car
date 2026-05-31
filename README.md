<div align="center">

```
██████╗  ██████╗    ██╗██╗     ██████╗████████╗██████╗ ██╗
██╔══██╗██╔════╝   ██╔╝██║    ██╔════╝╚══██╔══╝██╔══██╗██║
██████╔╝██║       ██╔╝ ██║    ██║        ██║   ██████╔╝██║
██╔══██╗██║      ██╔╝  ██║    ██║        ██║   ██╔══██╗██║
██║  ██║╚██████╗██╔╝   ███████╗╚██████╗  ██║   ██║  ██║███████╗
╚═╝  ╚═╝ ╚═════╝╚═╝    ╚══════╝ ╚═════╝  ╚═╝   ╚═╝  ╚═╝╚══════╝
```

**A browser-based RC car controller running on ESP8266 NodeMCU over Wi-Fi.**  
*No app. No Bluetooth. No cloud. Just open a URL and drive.*

---

![ESP8266](https://img.shields.io/badge/ESP8266-NodeMCU-00e5ff?style=for-the-badge&logo=espressif&logoColor=white)
![WebSocket](https://img.shields.io/badge/WebSocket-Raw%20ws%3A%2F%2F-7B2FBE?style=for-the-badge)
![HTML5](https://img.shields.io/badge/HTML5-Controller%20UI-E34F26?style=for-the-badge&logo=html5&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-00ff88?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active%20Build-ff6b35?style=for-the-badge)

</div>

---

## ◈ What is this?

**RC//CTRL** is a futuristic, cyberpunk-styled remote control car system built entirely from scratch. The car is driven by an **ESP8266 NodeMCU** running a custom WebSocket server in AP mode. The controller is a **single HTML file** — open it in any browser on your phone, tap **LINK**, and you are driving.

No Android app. No iOS app. No third-party cloud service. No Bluetooth pairing. Just a Wi-Fi connection and a browser.

---

## ◈ System Architecture

```
┌─────────────────────────────────┐        ┌──────────────────────────────┐
│         PHONE / LAPTOP          │        │         ESP8266 NodeMCU      │
│                                 │        │                              │
│  ┌───────────────────────────┐  │  WiFi  │  AP: RC_CTRL / 12345678      │
│  │  controller-v4.2.html     │◄─┼───────►│  HTTP  → 192.168.4.1:80     │
│  │  Joystick + Pivot UI      │  │        │  WS    → 192.168.4.1:81/ws  │
│  │  Differential Steering    │  │        │                              │
│  │  25Hz WebSocket Send      │  │        │  ┌──────────────────────┐   │
│  └───────────────────────────┘  │        │  │  Motor Driver        │   │
│                                 │        │  │  L298N / DRV8833     │   │
└─────────────────────────────────┘        │  │  Left  Motor ◄──ENA  │   │
                                           │  │  Right Motor ◄──ENB  │   │
                                           │  └──────────────────────┘   │
                                           └──────────────────────────────┘
```

---

## ◈ Features

- **2D Joystick Puck** — drag to steer with arcade-style differential drive mixing
- **Pivot Mode** — spin in place CCW / CW with a single tap
- **Speed Control** — `+` / `−` buttons to ramp speed up or down
- **Failsafe** — motors stop automatically if the signal is lost for > 500ms
- **Auto-Reconnect** — controller retries WebSocket connection on drop
- **25Hz command rate** — smooth, responsive control at 40ms intervals
- **Landscape lock** — UI enforces landscape orientation for best driving experience
- **Cyberpunk UI** — dark, neon-lit, monospace aesthetic designed for night driving
- **Zero dependencies** — no npm, no build step, no server, one `.html` file

---

## ◈ Hardware

| Component | Details |
|---|---|
| Microcontroller | ESP8266 NodeMCU 1.0 (ESP-12E Module) |
| Motor Driver | L298N or DRV8833 |
| Motors | DC BO motors (left pair + right pair) |
| Power (car) | 7.4V–12V Li-ion battery pack |
| Power (ESP) | 5V via USB or onboard regulator |

### Pin Mapping

| NodeMCU Pin | Function |
|---|---|
| D1 (GPIO5) | ENA — Left motor PWM |
| D2 (GPIO4) | ENB — Right motor PWM |
| D5 (GPIO14) | IN1 — Left motor direction A |
| D6 (GPIO12) | IN2 — Left motor direction B |
| D7 (GPIO13) | IN3 — Right motor direction A |
| D0 (GPIO16) | IN4 — Right motor direction B |
| GND | Common ground with motor driver |

> ⚠️ Always share a common GND between the ESP and the motor driver.

---

## ◈ Software Stack

| Layer | Technology |
|---|---|
| MCU Firmware | Arduino C++ (ESP8266 core) |
| Networking | ESPAsyncWebServer + ESPAsyncTCP |
| Protocol | Raw WebSocket `ws://` — no JSON, no overhead |
| Controller UI | Vanilla HTML5 / CSS3 / JavaScript |
| Motor Command Format | `L<pwm>R<pwm>` e.g. `L200R-150` |

---

## ◈ Getting Started

### 1. Flash the ESP

**Required libraries** (install via Arduino IDE → Sketch → Add .ZIP Library):
- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP/archive/refs/heads/master.zip)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer/archive/refs/heads/master.zip)

**Arduino IDE Settings:**
```
Board:         NodeMCU 1.0 (ESP-12E Module)
CPU Frequency: 160 MHz
Upload Speed:  921600
Flash Size:    4MB
```

Open `RC_CTRL_v2_1.ino` and upload to the board.

### 2. Connect

- Power on the car.
- On your phone or laptop, connect to Wi-Fi: **`RC_CTRL`** / **`12345678`**
- Open browser → `http://192.168.4.1`
- Tap **LINK** on the controller.

> 📱 **Phone users:** Open the controller page from `http://192.168.4.1` in the browser — do NOT open the HTML file from your file manager. The browser must load the page over `http://` for WebSocket to work.

### 3. Drive

- Drag the **puck** to steer.
- Use `+` / `−` to adjust speed.
- Tap **CCW** / **CW** for pivot turns.
- Release puck to center = motors stop.

---

## ◈ Protocol

Commands are sent as raw WebSocket text frames at up to **25Hz**:

```
L<left_pwm>R<right_pwm>
```

PWM range: `-255` (full reverse) to `+255` (full forward).

Examples:
```
L255R255    →  full speed forward
L-255R-255  →  full speed reverse
L255R-255   →  pivot clockwise
L0R0        →  stop (also triggered by failsafe)
```

The ESP parses this with:
```cpp
sscanf(msg, "L%dR%d", &l, &r);
```

No JSON parser needed. No external library needed. Minimal latency.

---

## ◈ Failsafe Behaviour

The ESP monitors the last received command timestamp. If no command arrives within **500ms**, it automatically calls `stopAll()` and brakes both motors. This ensures:

- If the phone disconnects → car stops
- If the browser tab closes → car stops
- If the puck is centered → no signal sent → car stops

---

## ◈ Project Structure

```
RC_CTRL/
├── RC_CTRL_v2_1.ino          ← ESP8266 firmware
├── controller-v4.2.html      ← Browser controller UI
└── README.md                 ← This file
```

---

## ◈ Roadmap

- [ ] Embed controller HTML directly into ESP PROGMEM (LittleFS)
- [ ] Battery voltage telemetry via ADC
- [ ] Headlight control via extra GPIO
- [ ] Speed display on controller UI
- [ ] OTA firmware updates

---


---

## ◈ How to Build & Use This Repo

Follow these steps exactly to build your own RC car from scratch.

---

### Step 1 — Build the Circuit

Wire the components as per the pin mapping table above.

```
ESP8266 NodeMCU
     │
     ├── D1 (ENA) ──────► L298N ENA  (Left PWM)
     ├── D2 (ENB) ──────► L298N ENB  (Right PWM)
     ├── D5 (IN1) ──────► L298N IN1  (Left dir A)
     ├── D6 (IN2) ──────► L298N IN2  (Left dir B)
     ├── D7 (IN3) ──────► L298N IN3  (Right dir A)
     ├── D0 (IN4) ──────► L298N IN4  (Right dir B)
     └── GND ───────────► L298N GND  (Common ground) ← CRITICAL

L298N
     ├── OUT1 / OUT2 ──► Left motor(s)
     ├── OUT3 / OUT4 ──► Right motor(s)
     └── 12V / GND   ──► Battery pack
```

> ⚠️ **Never skip the common GND.** ESP and L298N must share the same GND or nothing will work.

---

### Step 2 — Flash the Firmware

**2.1 Install the ESP8266 board in Arduino IDE**

Go to `File → Preferences` and add this to Additional Board Manager URLs:
```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
Then go to `Tools → Board → Boards Manager`, search **esp8266**, and install.

**2.2 Install required libraries**

Download and install both ZIPs via `Sketch → Include Library → Add .ZIP Library`:
- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP/archive/refs/heads/master.zip)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer/archive/refs/heads/master.zip)

**2.3 Open the firmware file**

Open [`ESP remote controll car.ino`](https://github.com/susantakumardas-2006/Remote_ctrl_car/blob/main/ESP%20remote%20controll%20car.ino) from the repo in Arduino IDE.

**2.4 Set the correct board settings**

```
Board:          NodeMCU 1.0 (ESP-12E Module)
CPU Frequency:  160 MHz
Upload Speed:   921600
Flash Size:     4MB (FS: 2MB, OTA: ~1019KB)
```

**2.5 Select the correct COM port**

Go to `Tools → Port` and select the port where your NodeMCU is connected.  
If you don't see a port, install the **CH340 driver** for your OS.

**2.6 Upload**

Click the **Upload** button (→ arrow) in Arduino IDE.  
Wait for `Done uploading.` in the console.

**2.7 Verify boot**

Open `Tools → Serial Monitor`, set baud rate to **115200**, and press **RST** on the board.  
You should see:

```
=== RC//CTRL ESP8266 Boot ===
[WiFi] AP started, IP: 192.168.4.1
[HTTP] Server started on port 80
[WS] WebSocket at ws://192.168.4.1:81/ws
```

If you see this, the ESP is ready. ✅

---

### Step 3 — Open the Controller

Open the deployed controller in your browser:

🌐 **[https://susantakumardas-2006.github.io/Remote_ctrl_car/](https://susantakumardas-2006.github.io/Remote_ctrl_car/)**

Keep this tab open. Do not close it.

---

### Step 4 — Connect to the ESP

Open a **new tab** in the **same browser** and do the following:

1. Go to your device's **Wi-Fi settings**
2. Connect to the network: **`RC_CTRL`**
3. Password: **`12345678`**
4. Open the new browser tab and go to: **`http://192.168.4.1`**
5. Confirm the ESP firmware page loads

---

### Step 5 — Link and Drive

1. Switch back to the controller tab: [https://susantakumardas-2006.github.io/Remote_ctrl_car/](https://susantakumardas-2006.github.io/Remote_ctrl_car/)
2. Tap the **LINK** button
3. If the status shows **CONNECTED** — you are ready 🟢
4. Drag the joystick puck to steer
5. Use `+` / `−` to adjust speed
6. Use **CCW** / **CW** buttons for pivot turns
7. Release the puck to center position to stop the car

> 📱 **Phone tip:** Make sure mobile data is turned **off** while connected to `RC_CTRL`.  
> The browser must stay on the ESP Wi-Fi — mobile data can override the connection silently.

---

## ◈ Author

Built by **Susanta Kumar Das** — first-year B.Tech CSE student  
Part of an ongoing hardware + software learning project.

> *"The fastest way to learn embedded systems is to build something that moves."*

---

<div align="center">

**RC//CTRL** — *Drive from your browser. No limits.*

⚡ Built with curiosity. Powered by ESP8266. Controlled by you. ⚡

</div>
