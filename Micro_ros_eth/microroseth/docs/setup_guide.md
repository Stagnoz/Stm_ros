# Setup Guide: Micro-ROS Ethernet & Dual-Core Sensor

This guide explains how to set up the hardware and software environment for the dual-core STM32H7 micro-ROS Ethernet project.

## 1. Hardware Requirements
- **Board**: STM32H743ZI2 Nucleo-144 (or compatible dual-core H7 board).
- **Sensor**: JSN-SR04T Waterproof Ultrasonic Sensor.
- **Other**: Ethernet cable, Micro-USB/USB-C cable for debugging/flashing.

## 2. Hardware Wiring
The **Cortex-M4 (CM4)** core handles the sensor interaction. Connect the JSN-SR04T as follows:

| Sensor Pin | STM32 Pin | Function |
| :--- | :--- | :--- |
| **5V** | 5V | Power Supply |
| **GND** | GND | Ground |
| **TRIG** | **PD1** | Trigger Pulse (Output from CM4) |
| **ECHO** | **PD0** | Echo Signal (Input to CM4) |

### Serial Debugging (USB-to-TTL)
To monitor logs from both cores simultaneously, you need an external USB-to-TTL adapter for the CM4.

| Component | Interface | Wiring / Connection |
| :--- | :--- | :--- |
| **CM7 Debug** | ST-Link VCP | Built-in via USB (No extra wiring) |
| **CM4 Debug** | **UART4** | **PA0 (TX)** and **PA1 (RX)** to USB-to-TTL |

> [!TIP]
> Connect the **GND** of your USB-to-TTL adapter to the STM32 GND to ensure a common reference.

> [!IMPORTANT]
> Ensure the sensor is powered by 5V. PD0/PD1 on the H7 are 5V tolerant.

## 3. Network Configuration
The **Cortex-M7 (CM7)** core handles the network stack. The current firmware uses a **static IP** configuration:

| Parameter | Value |
| :--- | :--- |
| **Device IP** | `192.168.50.2` |
| **Agent (Host) IP** | `192.168.50.1` |
| **Netmask** | `255.255.255.0` |
| **Gateway** | `192.168.50.1` |
| **UDP Port** | `8888` |

## 4. Micro-ROS Agent Setup
The micro-ROS agent must be running on your host machine (IP `192.168.50.1`).

### Using Docker (Recommended)
```bash
docker run -it --rm --net=host microros/micro-ros-agent:humble udp4 --port 8888
```

## 5. Serial Monitor Setup (Arduino IDE)
We use the Arduino IDE as a simple serial monitor to listen to the logs.

1. **Open Arduino IDE**.
2. **Select a Board**: Go to `Tools` -> `Board` and select **Arduino Uno** (this is just to enable the Serial Monitor).
3. **Monitor CM4**: 
   - Go to `Tools` -> `Port` and select the port corresponding to your **USB-to-TTL** adapter.
   - Open the **Serial Monitor** (Ctrl+Shift+M).
4. **Monitor CM7**:
   - Go to `Tools` -> `Port` and select the port corresponding to the **ST-Link Virtual COM Port**.
   - Open another instance of the Serial Monitor if needed.
5. **Baud Rate**: Set the speed to **115200** baud for both.

## 6. Future Integration
This setup is the foundation for a larger system where:
- The STM32 will publish telemetry data (distance, system status) via **micro-ROS**.
- A **NVIDIA Jetson** board will subscribe to these topics to perform higher-level processing or navigation.

## 7. Flashing the Board
1. Open the project in your IDE (STM32CubeIDE recommended).
2. Build both **CM7** and **CM4** projects.
3. **Flash the CM7 project first** (it behaves as the System Master and handles clock/power).
4. **Flash the CM4 project second**.
5. Press the **Reset** button on the board.

## 8. Verification
- **LEDs**: 
  - Green Blinking: Startup/Initializing.
  - Solid Green: System running and connected to micro-ROS agent.
  - Red Blinking/Solid: Error (Check wiring or agent connection).
- **Console**: Check the Arduino IDE Serial Monitor (115200 baud).
