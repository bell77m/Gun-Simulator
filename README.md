# Game Controller with ESP32

This project creates a wireless input controller using an ESP32 microcontroller, simulating mouse and keyboard inputs via Bluetooth (BLE). It combines motion control, joystick navigation, and proximity-based actions, making it ideal for gaming, assistive technologies, or remote control applications.

## Features
- **Motion-Based Mouse Control**:  
  Utilises the MPU6050 gyroscope and accelerometer to control mouse movements based on device orientation.
  
- **Proximity-Based Right Click**:  
  Uses the VL53L0X distance sensor to trigger right-click actions when objects come within a defined range.

- **Joystick-Based Navigation**:  
  A 2-axis joystick simulates keyboard inputs (`W`, `A`, `S`, `D`) for navigation or gaming controls.

- **Button Inputs**:  
  Buttons simulate:
  - Left mouse click (`mouseButton`)
  - Spacebar (jump)
  - 'R' key (reload or custom action)

- **Bluetooth Wireless Communication**:  
  The ESP32 communicates with host devices (PCs, smartphones) via Bluetooth, acting as a wireless mouse and keyboard.

- **Watchdog Timer (WDT)**:  
  A watchdog timer is implemented to reset the ESP32 if the system becomes unresponsive.

## Components
- ESP32 Development Board
- MPU6050 Gyroscope and Accelerometer
- VL53L0X Distance Sensor
- Analog Joystick
- Push Buttons
- USB Cable (for flashing and power)

## Getting Started

### Hardware Requirements
- ESP32 Development Board
- MPU6050 Gyroscope and Accelerometer
- VL53L0X Distance Sensor
- Analog Joystick
- Push Buttons
- USB Cable (for flashing the ESP32 and power)

### Software Requirements
- Arduino IDE with ESP32 Board support.
- Adafruit MPU6050 Library
- Adafruit VL53L0X Library
- BleMouse Library
  
Install the necessary libraries via the Arduino Library Manager or from their respective GitHub repositories.

### Wiring
Here’s a quick reference for wiring your components:

| Component       | ESP32 Pin  |
|-----------------|------------|
| MPU6050 (SDA)   | GPIO 3     |
| MPU6050 (SCL)   | GPIO 4     |
| VL53L0X (SDA)   | GPIO 3     |
| VL53L0X (SCL)   | GPIO 4     |
| Joystick X      | GPIO 7     |
| Joystick Y      | GPIO 5     |
| Left-Click      | GPIO 2     |
| Right-Click     | GPIO 38    |
| Jump Button     | GPIO 1     |

### Installation

1. **Set up the Arduino IDE for ESP32**:  
   Follow [this guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) to install the ESP32 board package in the Arduino IDE.

2. **Install Required Libraries**:  
   - Open the Arduino IDE.
   - Navigate to `Sketch -> Include Library -> Manage Libraries`.
   - Search for and install:
     - **Adafruit MPU6050**
     - **Adafruit VL53L0X**
     - **BleMouse**

3. **Upload the Code**:  
   - Connect your ESP32 to your computer using a USB cable.
   - Open the provided `controller.ino` file in the Arduino IDE.
   - Select your ESP32 board and the correct COM port.
   - Click the **Upload** button to flash the code onto the ESP32.

4. **Bluetooth Pairing**:  
   - Once powered, the ESP32 will start advertising as a Bluetooth device.
   - Pair the ESP32 with your computer or smartphone.
   - The ESP32 will now act as a Bluetooth mouse and keyboard.

## Usage
1. **Mouse Movement**:  
   Tilt the device to move the mouse pointer. The motion sensitivity can be adjusted by tweaking the `MOVE_SENSITIVITY` and `TILT_MOVE_SPEED` values in the code.

2. **Right Click**:  
   When an object is detected within the proximity range (configured by `DISTANT_AIM`), a right-click action is triggered.

3. **Joystick Navigation**:  
   Move the joystick to simulate directional keyboard presses (`W`, `A`, `S`, `D`). You can also press the spacebar using the `jumpButton` for jumping actions.

4. **Left Click**:  
   Press the `mouseButton` to simulate a left mouse click.

5. **'R' Key Press**:  
   Press the `rButton` to simulate pressing the 'R' key.

## Customization
- **Mouse Sensitivity**:  
  Adjust the mouse sensitivity by modifying the `MOVE_SENSITIVITY` and `TILT_MOVE_SPEED` variables.
  
- **Dead Zone**:  
  Adjust the `DEAD_ZONE_ANGLE` to set the threshold angle for mouse movements based on tilt.

- **Proximity Sensitivity**:  
  Modify the `DISTANT_AIM` value to set how close an object needs to be to trigger a right-click.

- **Watchdog Timer**:  
  The watchdog timeout is set to 5 seconds. You can adjust this if necessary by modifying the `timeout_ms` value in the WDT configuration.

## Troubleshooting
- **No Bluetooth Connection**:  
  If the ESP32 is not connecting over Bluetooth, ensure that it is powered on, properly flashed, and your device supports Bluetooth Low Energy (BLE).
  
- **Unresponsive Buttons**:  
  Check the wiring and ensure that the buttons are properly connected to the correct GPIO pins. Also, make sure the buttons are set to `INPUT_PULLUP`.

- **Gyro Calibration Issues**:  
  If the mouse pointer is too jittery or unresponsive, recalibrate the MPU6050 by resetting the `gyroXOffset`, `gyroYOffset`, and `gyroZOffset` values.

## Future Improvements
- Implement button debouncing to prevent multiple unintended key presses.
- Add additional sensors or input methods for more diverse control.
- Enhance Bluetooth stability and performance for faster response times.



