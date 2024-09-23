# Import necessary modules
from machine import Pin
import time, bluetooth, ujson
from ble_simple_peripheral import BLESimplePeripheral
# Create a Bluetooth Low Energy (BLE) object
ble = bluetooth.BLE()
# Create an instance of the BLESimplePeripheral class with the BLE object
sp = BLESimplePeripheral(ble)
# Create a Pin object for the onboard LED and configure it as an output

#default gestures stored
mappings = {}
with open("defaults.json", 'r') as file:
    mappings = ujson.load(file)  # Load JSON data into a dictionary

led = Pin("LED", Pin.OUT)
# Initialize the LED state to OFF
led_state = 0

# Set the debounce time to 0. Used for switch debouncing
debounce_time=0

# Create a Pin object for Pin 0, configure it as an input with a pull-up resistor
button = Pin(12, Pin.IN, Pin.PULL_UP)


from mpu6050 import MPU6050
from machine import I2C,Pin
import time

i2c = I2C(id=0, sda=Pin(16), scl=Pin(17), freq=400000)
mpu = MPU6050(i2c)

x=0
y=0
z=0

led = Pin("LED", Pin.OUT)
# Initialize the LED state to OFF
led_state = 0

led_state1 = 0
led1 = Pin(28, Pin.OUT)


# Define a callback function to handle received data
def on_rx(data):
    print("Data received: ", data)  # Print the received data
    global led_state  # Access the global variable led_state
    if data == b't':  # Check if the received data is "toggle"
        led.value(not led_state)  # Toggle the LED state (ON/OFF)
        led_state = 1 - led_state  # Update the LED state

#############################
# we want a send data function
# infinite loop will go at all time,
#  power button will either be on or off,
#
#  if ON: call send data function
#  if OFF: keep going
#############################

# Start an infinite loop
while True:
    xAccel=mpu.accel.x
    yAccel=mpu.accel.y
    zAccel=mpu.accel.z
    
    #print('x: ', int(xAccel*10), ' G ', 'y: ', int(yAccel*10), ' G', 'z: ', int(zAccel*10))
    time.sleep(0.1)
    
    
    
    if button.value() == 0:
        time.sleep(0.5)
        print("Not collecting data")
        while True:
            if button.value() == 0:
                time.sleep(0.5)
                print("Out of loop")
                break
            time.sleep(0.1)
            
    if (y - yAccel) > 0.75 or y - yAccel < -0.75:
            
        if y-yAccel > 0.75:
            # Check if the BLE connection is established
            if sp.is_connected():
                # Create a message string
                msg=f"y axis moved {y-yAccel}."
                # Send the message via BLE
                sp.send(msg)
                    
        if y-yAccel < -0.75:
            if sp.is_connected():
                # Create a message string
                msg=f"y axis moved {y-yAccel}."
                # Send the message via BLE
                sp.send(msg)
        
    
    
    if (x - xAccel) > 1.25 or x - xAccel < -1.25:
            
        if x-xAccel > 1.25:
            led.value(not led_state)  # Toggle the LED state (ON/OFF)
            led_state = 1 - led_state  # Update the LED state
            # Check if the BLE connection is established
            if sp.is_connected():
                # Create a message string
                msg="LED is toggled."
                # Send the message via BLE
                sp.send(msg)
                print(f"X: {x-xAccel}, Y: {y-yAccel}, Z: {z-zAccel}")
                    
        if x-xAccel < -1.25:
            led1.value(not led_state1)
            led_state1 = 1 - led_state1
            if sp.is_connected():
                # Create a message string
                msg="White light is toggled."
                # Send the message via BLE
                sp.send(msg)
                print(f"X: {x-xAccel}, Y: {y-yAccel}, Z: {z-zAccel}")
        #wait after action is completed
        #time.sleep(0.5)
        
            
        # Update the debounce time    
        debounce_time=time.ticks_ms()
       
    x = xAccel
    y = yAccel
    z = zAccel
    
    if sp.is_connected():  # Check if a BLE connection is established
        sp.on_write(on_rx)  # Set the callback function for data reception
    
    