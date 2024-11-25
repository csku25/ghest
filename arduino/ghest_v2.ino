/*
  Project:    Ghest
  Author(s):  K'drian Vega, Zachary Andruchowitz
  Start date: Sept. 30, 2024

  Used Libraries: Adafruit_MPU6050, Adafruit_Sensor, KeyboardBT, LittleFS
  Credit to: Earle Philhower for Raspberry Pi Pico Arduino Core API - https://github.com/earlephilhower/arduino-pico
*/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <KeyboardBT.h>
#include <BTstackLib.h>
#include <SPI.h>
#include <LittleFS.h>

#define BUTTON 20
#define SIG_GHEST_APP "ghest_app_ready"
Adafruit_MPU6050 mpu;
bool is_paused = false;
uint8_t debounce = 0;
uint8_t debounce_delay = 50;
uint8_t button_state = HIGH;
bool serial_on = true;  //toggle true/false to output serial
bool mpu_flag = true;
bool ble_flag = false;

void deviceConnectedCallback(BLEStatus status, BLEDevice *device) {
  (void) device;
  switch (status) {
    case BLE_STATUS_OK:
      Serial.println("BLE connected!");
      ble_flag = true;
      break;
    default:
      break;
  }
}

void deviceDisconnectedCallback(BLEDevice * device) {
  (void) device;
  Serial.println("BLE Disconnected.");
  ble_flag = false;
}

int gattWriteCallback(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
  (void) value_handle;
  char* msg = (char*) buffer;
  if (msg == NULL) return 1;
  Serial.write(msg);
  return 0;
}

void handle_file_stuff() {
  File f = LittleFS.open("defaults.json", "r");
  if (f) {
    size_t file_size = f.size();
    char f_buff[file_size+1]; //size + null terminator
    int count = 1;
    bzero(f_buff, file_size+1); //size + null terminator
    if (f.read((uint8_t *)f_buff, file_size))
      sscanf(f_buff, "%d", &count);
    f.close();

    // for (int idx = 0; idx < file_size; idx++) {
    //   if (serial_on) Serial.print(f_buff[idx]);
    // }
    // if (serial_on) Serial.print("\n");
  }
}

void setup() {
  if (serial_on) {
    //start serial communication, baud rate 115,200
    Serial.begin(115200);
    while (!Serial);
  }

  Wire.setSDA(16);
  Wire.setSCL(17);
  if (!mpu.begin()) {
    if (serial_on) Serial.println("Error: Failed to find MPU6050 sensor!");
    mpu_flag = false;
  }
  Serial.println("MPU6050 initialized...\n");

  //set physical pico pins and initialize mpu
  pinMode(BUTTON, INPUT_PULLUP);
  KeyboardBT.begin("Ghest Bracelet");

  BTstack.setBLEDeviceConnectedCallback(deviceConnectedCallback);
  BTstack.setBLEDeviceDisconnectedCallback(deviceDisconnectedCallback);
  BTstack.setGATTCharacteristicWrite(gattWriteCallback);

  // setup GATT Database
  BTstack.addGATTService(new UUID("B8E06067-62AD-41BA-9231-206AE80AB550"));
  BTstack.addGATTCharacteristic(new UUID("f897177b-aee8-4767-8ecc-cc694fd5fce1"), ATT_PROPERTY_READ | ATT_PROPERTY_WRITE, "For Custom Mappings");

  // startup Bluetooth and activate advertisements
  //BTstack.setup("Ghest Bracelet BLE");
  //BTstack.startAdvertising();

  LittleFS.begin();
  handle_file_stuff();

  //configure settings
  mpu.setAccelerometerRange(mpu.getAccelerometerRange());
  mpu.setGyroRange(mpu.getGyroRange());
  mpu.setFilterBandwidth(mpu.getFilterBandwidth());

  delay(100);
  if (serial_on) Serial.println("ax,ay,az,gx,gy,gz");
}

void loop() {
  BTstack.loop();
  if (!mpu_flag) exit(0);
  if (ble_flag) KeyboardBT.end();

  uint8_t reading = digitalRead(BUTTON);
  if (reading != button_state) {
    debounce = millis();
  }

  if ((millis() - debounce) > debounce_delay) {
    // Only toggle if the reading is LOW (button pressed)
    if (reading == LOW && button_state == HIGH) {
      is_paused = !is_paused;  // Toggle the paused state

      if (is_paused) {
        if (serial_on) Serial.println("Pausing data collection...");
      } else {
        if (serial_on) Serial.println("Starting data collection...");
        delay(1000);
      }
    }
  }
  button_state = reading;

  if (!is_paused && !ble_flag) {
    //create sensor event objects for accelerometer, gyro, and temperature (discard it)
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t discard;
    //get the current sensor readings
    mpu.getEvent(&accel, &gyro, &discard);
    //discard temp readings
    //delay before the next reading
    if (serial_on) {
      Serial.print("aX:");
      Serial.print(accel.acceleration.x);
      Serial.print(",");
      Serial.print("aY:");
      Serial.print(accel.acceleration.y);
      Serial.print(",");
      Serial.print("aZ:");
      Serial.print(accel.acceleration.z);
      Serial.print(",");
      Serial.print("gX:");
      Serial.print(gyro.gyro.x);
      Serial.print(",");
      Serial.print("gY:");
      Serial.print(gyro.gyro.y);
      Serial.print(",");
      Serial.print("gZ:");
      Serial.print(gyro.gyro.z);
      Serial.println("");
    }
    delay(50);  //in milliseconds

    if (abs(accel.acceleration.x) <= 1.75 && abs(accel.acceleration.y) <= 1.75 && accel.acceleration.z >= 8 && gyro.gyro.x < 0 && abs(gyro.gyro.y) >= 0.2 && abs(gyro.gyro.z >= 0.07)) {
      KeyboardBT.consumerPress(KEY_PLAY_PAUSE);
      KeyboardBT.consumerRelease();
      if (serial_on) Serial.println("Key pressed: PLAY/PAUSE");
      delay(1500);  //1.5-second buffer bewteen taps
    }

    if (!(accel.acceleration.z > 3) && accel.acceleration.y <= 7 && (gyro.gyro.z > -0.8 && gyro.gyro.z < 0.8)) {
      if (gyro.gyro.x <= -0.5 && gyro.gyro.y <= -0.5) {
        KeyboardBT.consumerPress(KEY_SCAN_PREVIOUS);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: SKIP BACK");
        delay(1000);  //1-second buffer bewteen gestures
      }
      
      else if (gyro.gyro.x >= 0.75 && gyro.gyro.y >= 0.75) {
        KeyboardBT.consumerPress(KEY_SCAN_NEXT);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: SKIP FORWARD");
        delay(1000);  //1-second buffer bewteen gestures
      }
    }
    
    else if (accel.acceleration.z >= 5.75 && (gyro.gyro.z > -0.5 && gyro.gyro.z < 0.5)) {
      if (gyro.gyro.x < -0.3 && gyro.gyro.y < -2) {
        KeyboardBT.consumerPress(KEY_VOLUME_INCREMENT);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: VOLUME UP");
        delay(1000);  //1-second buffer bewteen gestures
      }
      
      else if (gyro.gyro.x < -0.5 && gyro.gyro.y > 1.5) {
        KeyboardBT.consumerPress(KEY_VOLUME_DECREMENT);
        KeyboardBT.consumerRelease();
        if (serial_on) Serial.println("Key pressed: VOLUME DOWN");
        delay(1000);  //1-second buffer bewteen gestures
      }
    }
  }
}