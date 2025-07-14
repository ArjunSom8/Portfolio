# Build-a-Rocket Telemetry System

A high-speed, two-way telemetry system for student-built rockets, featuring:

- **Arduino RF protocol** using 32-byte nRF24L01+ packets with CRC-based acknowledgments  
- **Python/PyQt6 ground station** with USB-Serial QThreads for sub-5% CPU load and 2 Mbps throughput  
- **ESP32 flight-computer variant** for future upgrades  
- **Test-proven**: 99% sensor accuracy across three launches; 67% latency reduction; optimized nozzle angle cut pitch-over by 18%

---

## 📁 Repository Layout

```
.
├── Capstone Project Report Realest.pdf
└── Projects/
    └── Build-a-Rocket spacc-gui main Rocket Club/
        ├── ESP32/
        │   └── ESP-Starter/
        │       ├── ESP-Starter.ino       # ESP32 flight code + helpers
        │       └── radio_helpers.h       # RF24 packet framing & CRC
        ├── RF24_TwoWay/
        │   └── RF24_TwoWay/
        │       └── RF24_TwoWay.ino       # Arduino nRF24L01+ two-way protocol
        └── GUI/
            ├── main.py                   # PyQt6 UI & plotting
            └── serial_thread.py          # QThread for USB-Serial I/O
```

---

## 🚀 Getting Started

### 1. Hardware Requirements

- Two nRF24L01+ radio modules  
- Arduino Uno (or compatible) for flight; USB-Serial adapter for ground station  
- (Optional) ESP32 dev board for alternate flight computer  
- Sensors: barometer, accelerometer, etc. per your nosecone design  

### 2. Arduino Setup

1. Install the [RF24 library](https://github.com/nRF24/RF24) in the Arduino IDE.  
2. Open `Projects/RF24_TwoWay/RF24_TwoWay/RF24_TwoWay.ino`.  
3. Wire your nRF24L01+ pins to the Arduino per the header comments.  
4. Upload to your flight board.

### 3. ESP32 Setup (Optional)

1. Open `Projects/ESP32/ESP-Starter/ESP-Starter.ino` in the Arduino IDE (set to ESP32 board).  
2. Verify wiring matches `radio_helpers.h`.  
3. Upload to the ESP32.

### 4. Ground Station (Python)

```bash
cd Projects/Build-a-Rocket\ spacc-gui\ main\ Rocket\ Club/GUI
pip install -r requirements.txt
python main.py
```

- **Requirements**:  
  - Python 3.7+  
  - PyQt6  
  - pyserial  

---

## 🔧 Architecture & Protocol

- **Packet format**: 32 bytes total (header, payload, CRC)  
- **Acknowledgments**: CRC-verified two-way handshake  
- **Throughput**: 2 Mbps over 2.4 GHz link  
- **Threading**:  
  - `serial_thread.py` uses QThreads to keep the UI at <5% CPU  
  - Telemetry plotting in real time via PyQt6’s `QChartView`  

---

## 📊 Performance Highlights

- **Latency improvement**: –67% vs. baseline  
- **Sensor accuracy**: 99% across 3 flights  
- **Pitch-over reduction**: –18% after nozzle-angle optimization

---

## 🤝 Contributing

1. Fork the repo  
2. Create a feature branch (`git checkout -b feature/xyz`)  
3. Commit your changes (`git commit -m "Add xyz"`)  
4. Push (`git push origin feature/xyz`)  
5. Open a Pull Request

---

## 📄 License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.
