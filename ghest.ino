#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup(void) {
  //start serial communication, baud rate 115,200
  Serial.begin(115200);
  while (!Serial)
    delay(10); //wait for serial port to open on some boards

  //set physical pico pins and init mpu
  Wire.setSDA(16);
  Wire.setSCL(17);
  if (!mpu.begin()) {
    Serial.println("Error: Failed to find MPU6050 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 initialized...\n");

  //configure sensor settings
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);  // Set accelerometer range to +/- 2g
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);       // Set gyroscope range to +/- 250 deg/s
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);    // Set filter bandwidth to 21 Hz

  delay(100);
}

void loop() {
  //create sensor event objects for accelerometer, gyro, and temperature
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;

  //get the current sensor readings
  mpu.getEvent(&accel, &gyro, &temp);
  //discard temp readings

  //print accelerometer values (in m/s^2)
  Serial.printf("Accel X: %d, Y: %d, Z: %d m/s^2\n\n", accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);

  //print gyroscope values (in degrees per second)
  Serial.printf("Gyro X: %d, Y: %d, Z: %d deg/s\n\n", accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);

  //delay before the next reading
  delay(50); //in milliseconds
}

