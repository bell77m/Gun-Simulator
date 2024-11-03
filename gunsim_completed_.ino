#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "BleMouse.h"
#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"
#include "Adafruit_VL53L0X.h"
#include <esp_task_wdt.h>

// calibration
#define MOVE_SENSITIVITY 75
#define DISTANT_AIM 180
#define DEAD_ZONE_ANGLE 20
#define TILT_MOVE_SPEED 1.2

// WDT
esp_task_wdt_config_t wdt_config = {
  .timeout_ms = 5 * 1000,
  .idle_core_mask = 0,
  .trigger_panic = true
};

//I2c
#define SDA_I2C 3
#define SCL_I2C 4

// mouse and keyboard
USBHIDMouse Mouse;
USBHIDKeyboard Keyboard;
#define mouseButton 2
#define rButton 38
#define responseDelay 5

//่ joystick
#define xJoy 7
#define yJoy 5
#define jumpButton 1
unsigned long lastMoveTimeJoy = 0;

// distance laser sensor (LOX)
#define INTERVAL_TIME 10
unsigned long time_1 = 0;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;

// bluetooth
BleMouse bleMouse;

// gyro
Adafruit_MPU6050 mpu;

unsigned long lastTimeGyro = 0;
unsigned long lastTimeAcc = 0;
unsigned long lastTimeTemp = 0;

unsigned long gyroDelay = 10;  //millisec
unsigned long accDelay = 200;
unsigned long tempDelay = 1000;

sensors_event_t a, g, temp;

float gyroX, gyroY, gyroZ;
float accX, accY, accZ;
float temperature;

float gyroXerror = 0.07;
float gyroYerror = 0.03;
float gyroZerror = 0.01;

float gyroXOffset = 0.0;
float gyroYOffset = 0.0;
float gyroZOffset = 0.0;

// gyro setting
void initMPU() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  for (int i = 0; i < 100; i++) {
    mpu.getEvent(&a, &g, &temp);
    gyroXOffset += g.gyro.x;
    gyroYOffset += g.gyro.y;
    gyroZOffset += g.gyro.z;
    delay(10);
  }
  gyroXOffset /= 100;
  gyroYOffset /= 100;
  gyroZOffset /= 100;
}

// distance sensor
void printRange(int range) {
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != range) {
    Serial.print("Distance (mm): ");
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println("Out of range");
  }
}

// joystick direction
void moveJoyStick(int x, int y) {
  const int deadZoneJoy = 200;
  if (x > 2000 - deadZoneJoy && x < 2000 + deadZoneJoy && y > 2000 - deadZoneJoy && y < 2000 + deadZoneJoy) {
    Keyboard.releaseAll();
  }
  else if (x > 3000 && y > 1000 && y < 2000) {
    Keyboard.press('d');
  }
  else if (x < 1000 && y > 1000 && y < 2000) {
    Keyboard.press('a');
  }
  else if (y > 3000 && x > 1000 && x < 3000) {
    Keyboard.press('s');
  }
  else if (y < 1000 && x > 1000 && x < 3000) {
    Keyboard.press('w');
  }
  else if (y < 400 && x < 400) {
    Keyboard.press('w');
    Keyboard.press('a');
  }
  else if (y < 400 && x > 3600) {
    Keyboard.press('w');
    Keyboard.press('d');
  }
  else if (y > 3600 && x < 400) {
    Keyboard.press('s');
    Keyboard.press('a');
  }
  else if (y > 3600 && x > 3600) {
    Keyboard.press('s');
    Keyboard.press('d');
  }
  else{
    Keyboard.releaseAll();
  }
}

void setup() {
  Serial.begin(115200);

  // WDT
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);

  Wire.begin(SDA_I2C, SCL_I2C);


  // LOX (Right click)
  if (!lox.begin()) {
    Serial.println("Failed to find VL53L0X sensor!");
    while (1) delay(10);
  }

  //gyro
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(10);
  }
  initMPU();
  Serial.println("MPU6050 Found!");

  //clicks
  pinMode(mouseButton, INPUT_PULLUP);
  pinMode(rButton, INPUT_PULLUP);
  pinMode(jumpButton, INPUT_PULLUP);

  Mouse.begin();
  Keyboard.begin();
  USB.begin();

  //bluetooth
  Serial.println("Starting MPU6050 and BLE setup!");
  bleMouse.begin();
  Serial.println("BLE Mouse Initialized");
}

void loop() {
  esp_task_wdt_reset();

  // left click otg
  int LeftClickState = digitalRead(mouseButton);
  int rPressedState = digitalRead(rButton);

  if (LeftClickState == HIGH) {
    if (!bleMouse.isPressed(MOUSE_LEFT)) {
      bleMouse.press(MOUSE_LEFT);
    }
  } else {
    if (bleMouse.isPressed(MOUSE_LEFT)) {
      bleMouse.release(MOUSE_LEFT);
    }
  }

  // keyboard button
  if (rPressedState == HIGH) {
    Keyboard.press('r');
    delay(responseDelay);
    Keyboard.release('r');
  }

  // joystick
  int jumpVal = digitalRead(jumpButton);
  int xJoyVal = analogRead(xJoy);
  int yJoyVal = analogRead(yJoy);

  if (millis() - lastMoveTimeJoy > responseDelay) {
    moveJoyStick(xJoyVal, yJoyVal);
    lastMoveTimeJoy = millis();
  }

  if (jumpVal == LOW) {
    Keyboard.press(' ');
  } else {
    Keyboard.release(' ');
  }

  // right click (lox)
  int distant = measure.RangeMilliMeter;

  if (distant <= DISTANT_AIM) {
    if (!bleMouse.isPressed(MOUSE_RIGHT)) {
      bleMouse.press(MOUSE_RIGHT);
    }
  } else {
    if (bleMouse.isPressed(MOUSE_RIGHT)) {
      bleMouse.release(MOUSE_RIGHT);
    }
  }
  if (millis() - time_1 > INTERVAL_TIME) {
    time_1 = millis();
    printRange(4);
  }

  // bluetooth mouse
  if (bleMouse.isConnected()) {
    mpu.getEvent(&a, &g, &temp);

    int moveX = -g.gyro.z * MOVE_SENSITIVITY;
    int moveY = g.gyro.y * MOVE_SENSITIVITY;
    float tiltX = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;  // Calculate X-axis tilt angle (in degrees)
    int speed = 0;

    if (tiltX > DEAD_ZONE_ANGLE || tiltX < -DEAD_ZONE_ANGLE) {  // if tilted over deadzone, movement based on tilt angle (X, Y axis)
      speed = tiltX * TILT_MOVE_SPEED;
      bleMouse.move(speed, moveY);
    } else {
      bleMouse.move(moveX, moveY);
    }
    delay(responseDelay);
  } else {
    Serial.println("Waiting for BLE connection...");
  }
}