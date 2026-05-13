# GPS IoT Tracker with ESP8266 and Supabase

A real-time GPS tracking system using **ESP8266**, **TinyGPS++**, and **Supabase**.  
This project reads GPS coordinates from a GPS module and uploads the data to a Supabase database over Wi-Fi.

---

# 📌 Features

- 📍 Real-time GPS tracking
- 🌐 Wi-Fi connectivity using ESP8266
- ☁️ Cloud database integration with Supabase
- 🔄 Automatic Wi-Fi reconnection
- 🛰️ GPS data parsing using TinyGPS++
- 🔒 HTTPS secure requests
- 🗺️ Google Maps location preview
- 📊 Cloud-stored GPS history

---

# 🛠️ Hardware Requirements

| Component | Description |
|---|---|
| ESP8266 (ESP-12E / NodeMCU) | Main microcontroller |
| GPS Module (NEO-6M recommended) | GPS receiver |
| Jumper Wires | Connections |
| USB Cable | Programming and power |
| Power Supply | Optional external power |

---

# 📡 Wiring Diagram

| GPS Module | ESP8266 |
|---|---|
| TX | D7 (GPIO13) |
| RX | Not Connected |
| GND | GND |
| VCC | 3.3V / 5V |

---

# 📚 Required Libraries

Install the following libraries from the Arduino Library Manager:

- TinyGPS++
- EspSoftwareSerial
- ESP8266WiFi
- ESP8266HTTPClient

---

# ⚙️ Arduino IDE Setup

## 1. Install ESP8266 Board Manager

Open Arduino IDE and go to:

```text
File > Preferences
```

Add this URL inside **Additional Board URLs**:

```text
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

Then go to:

```text
Tools > Board > Boards Manager
```

Search for:

```text
ESP8266
```

Install the package.

---

# ☁️ Supabase Setup

## 1. Create a Supabase Project

Visit:

```text
https://supabase.com
```

Create a new project.

---

## 2. Create the Database Table

Open the **SQL Editor** in Supabase and run:

```sql
create table gps_locations (
    id bigint generated always as identity primary key,
    device_id text,
    latitude double precision,
    longitude double precision,
    created_at timestamp with time zone default timezone('utc', now())
);
```

---

## 3. Get API Credentials

Navigate to:

```text
Project Settings > API
```

Copy the following:

- Project URL
- Publishable/Anon Key

---

# 🔧 Configure the Arduino Sketch

Update these values in the code:

```cpp
// Wi-Fi
static const char *WIFI_SSID = "YOUR_WIFI_NAME";
static const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Supabase
static const char *SUPABASE_URL = "YOUR_SUPABASE_URL";
static const char *SUPABASE_PUBLISHABLE_KEY = "YOUR_SUPABASE_KEY";

static const char *DEVICE_ID = "esp12e-gps-1";
```

---

# 🚀 Upload the Code

1. Connect ESP8266 to your computer
2. Select the correct board and COM port
3. Click Upload

---

# 🖥️ Serial Monitor Output

Example output:

```text
Starting GPS + Supabase tracker...
Connecting to Wi-Fi....
Wi-Fi connected. IP: 192.168.1.5

Latitude: 7.073056
Longitude: 125.612778
Google Maps: https://www.google.com/maps?q=7.073056,125.612778

Supabase insert success.
```

---

# 🗺️ Viewing GPS Data

Open Supabase:

```text
Table Editor > gps_locations
```

You will see stored GPS coordinates like this:

| id | device_id | latitude | longitude | created_at |
|---|---|---|---|---|
| 1 | esp12e-gps-1 | 7.073056 | 125.612778 | 2026-05-14 |

---

# 📂 Project Structure

```text
gps-iot-tracker/
│
├── gps_tracker.ino
├── README.md
└── wiring-diagram.png
```

---

# 🔒 Security Notes

- Do not expose your API keys publicly
- Enable Row Level Security (RLS) in production
- Replace `setInsecure()` with proper SSL certificate validation for production systems

---

# 📈 Future Improvements

- 📱 Mobile app integration
- 🗺️ Real-time tracking dashboard
- 🚨 Geofencing alerts
- 📊 Route history visualization
- 💾 Offline GPS data caching
- 📡 MQTT support
- 🔋 Low-power optimization

---

# 🧠 Technologies Used

- ESP8266
- Arduino IDE
- TinyGPS++
- Supabase
- REST API
- HTTPS

