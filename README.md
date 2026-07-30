#  Physical Space Flight Simulator

A hybrid physical/digital interactive orbital flight simulator. This project projects a physical "spaceship" (a steel ball) into a field using a custom dual-servo flywheel and stepper-driven launch turret. 

The ball's flight path is influenced by permanent magnets acting as "planets," while real-time trajectory vector calculations, system statuses, and target objectives are handled via a multi-microcontroller architecture over Serial communication. You are free to place the magnets as you please, keep in mind that the 11-node rib strip we are using (though you can have more or less - edit the const int for them in the display-controller code) will not light the last 3 nodes, as in our setup they were unreachable. We recommend you too place a big magnet-planet on the left side of your field.


##  How It Works

1. **Aiming & Power**: Two analog potentiometers control the platform:
   * **Angle Potentiometer**: Controls a 4-wire stepper motor to rotate the launcher platform from 0° to 180°.
   * **Speed Potentiometer**: Sets the delay timing for the launching flywheel servos, altering the launch velocity.
2. **Real-time Telemetry**: The main controller calculates Vx and Vy velocity vectors in real time based on angle and power inputs and sends telemetry over Serial (`9600 baud`) to the display controller.
3. **Targeting**: A WS2812B RGB NeoPixel LED strip acts as a target grid. A single LED lights up in **red** (the target zone) among **blue** ambient lights. Every time a launch occurs, a new random target LED is generated.
4. **Launch Cycle**: Hitting the physical launch button triggers the synchronized sweeping of two contra-rotating servo wheels that spin up and propel the steel ball forward. The display highlights the vector in red and switches state to `LAUNCHING!`.


##  Circuit & Hardware Wiring

The system uses **2x Arduinos** communicating over standard Hardware Serial (`Tx` to `Rx`).

### 1. Main Controller (Launcher & Controls)

 Hardware Component - Arduino Pin - Notes 

 Launch Button - D12 - Wired with internal `INPUT_PULLUP` (Active LOW) 
 Angle Potentiometer - A1 - Reads input for Stepper rotation 
 Speed Potentiometer - A0 - Reads input for flywheel spin speed 
 Servo 1 (Flywheel Left) - D6 - Standard PWM Servo signal 
 Servo 2 (Flywheel Right) - D7 - Standard PWM Servo signal 
 Stepper Motor IN1 - D2 - Phase sequence control pin 1 
 Stepper Motor IN2 - D4 - Phase sequence control pin 2 
 Stepper Motor IN3 - D3 - Phase sequence control pin 3 
 Stepper Motor IN4 - D5 - Phase sequence control pin 4 
 Serial Output (Tx) - TX (D1) - Connected to Rx pin on Display Arduino 



### 2. Display & Target Controller

 Hardware Component - Arduino Pin - Notes 

 2.4" TFT Display Shield - Standard Shield Pins - Driven via `MCUFRIEND_kbv` & `Adafruit_GFX` 
 RGB LED Strip (NeoPixel) - D10 - Signal line for target selection strip 
 Serial Input (Rx) - RX (D0) - Reads data stream from Main Controller 



## Components Used

* Microcontrollers: 2x Arduino Uno (or Nano / Mega equivalents)
* Motors:
  * 1x 4-wire Stepper Motor (with driver board like ULN2003 / A4988)
  * 2x Micro Servos (SG90 or MG996R)
* Display & Visuals:
  * 1x 2.4-inch TFT LCD Shield (MCUFRIEND compatible)
  * 1x WS2812B RGB NeoPixel Strip (10 LEDs)
* Inputs & Controls:
  * 2x 10k Ohm Linear Potentiometers
  * 1x Heavy-Duty Push Button (Big Red Launch Button)
* Physical Materials:
  * Steel ball bearing ("Spaceship")
  * High-strength Neodymium Magnets ("Planets")
  * Custom launcher chassis with mounted wheels



##  Communication Protocol

Data is transmitted continuously from the launcher board to the display board every ~60ms using standard `Serial.print()` formatted strings:

* Telemetry Packet: `[angle],[speedDelay],[vx],[vy]` (e.g., `90,5,0.0,75.0`)
* Launch Event: `LAUNCH`
* Reset Event: `READY`



##  Required Libraries

To compile and upload these sketches, install the following libraries via the Arduino Library Manager:

* `Servo.h` (Built-in)
* `Adafruit_GFX.h`
* `MCUFRIEND_kbv.h`
* `Adafruit_NeoPixel.h`