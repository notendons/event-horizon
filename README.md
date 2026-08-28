# Event Horizon

## Physical Space Flight Simulator

Event Horizon is a small physical orbital simulator I built using two Arduinos, a custom launcher, a steel ball, and a bunch of permanent magnets.

The idea is that the **steel ball is the spacecraft** and the **magnets are the planets**. I can choose the direction and approximate launch speed, fire the ball into the field, and see how the magnetic forces change its trajectory.

There is also a TFT display showing the launch angle, velocity vector, and current status, plus a NeoPixel strip that randomly selects a target for each launch.

The important part is that the trajectory is actually happening in the real world. The Arduino is not simulating the whole thing and drawing an orbit on a screen.

---

## How it works

The launcher has two potentiometers and a big launch button.

The first potentiometer controls the angle. Its value is mapped to a stepper motor, which rotates the launcher between about `0°` and `180°`.

The second potentiometer controls the launch speed. It changes how quickly the two servo motors sweep the flywheels.

When the launch button is pressed, the two servos move in opposite directions and throw the steel ball out of the launcher.

Before launching, the Arduino calculates the initial velocity vector:

```text
vx = v cos(θ)
vy = v sin(θ)
```

and sends it to the second Arduino, which draws the vector on the TFT.

After the ball leaves the launcher, the Arduino doesn't control its trajectory. The magnets do.

---

## The "planets"

The magnetic field is made from permanent/neodymium magnets placed around the playing area.

The arrangement is completely changeable. Moving a magnet changes the path of the ball, so you can make different setups and see how the trajectory changes.

In my setup I found that having a larger magnet on the left side worked well.

There is no magnetic-field simulation in the code. The forces are produced physically by the magnets themselves.

---

## Electronics

I used two Arduino boards because I wanted to keep the launcher and display separate.

### Launch controller

The first Arduino handles all of the physical launcher stuff:

* angle potentiometer
* speed potentiometer
* stepper motor
* two servos
* launch button
* velocity calculation
* serial communication

Pinout:

| Part                | Pin   |
| ------------------- | ----- |
| Speed potentiometer | `A0`  |
| Angle potentiometer | `A1`  |
| Launch button       | `D12` |
| Servo 1             | `D6`  |
| Servo 2             | `D7`  |
| Stepper IN1         | `D2`  |
| Stepper IN2         | `D4`  |
| Stepper IN3         | `D3`  |
| Stepper IN4         | `D5`  |
| Serial TX           | `D1`  |

The button uses the Arduino's internal pull-up resistor, so it reads `LOW` when pressed.

### Display controller

The second Arduino controls:

* the 2.4" TFT
* the NeoPixel strip
* the display status
* the velocity vector
* target generation
* serial input

Pinout:

| Part           | Pin         |
| -------------- | ----------- |
| NeoPixel strip | `D10`       |
| Serial RX      | `D0`        |
| TFT            | Shield pins |

---

## The launcher

The launcher uses two servo motors with flywheels attached to them.

They move in opposite directions:

```cpp
myServo1.write(pos);
myServo2.write(180 - pos);
```

so that the ball gets caught between them and launched forward.

When I press the launch button, the launcher:

1. sends `LAUNCH` to the display Arduino
2. moves both flywheels through the launch sequence
3. returns them to their starting positions
4. waits
5. sends `READY`

The speed potentiometer controls the delay between servo positions. A smaller delay means the servos move faster.

---

## Velocity calculation

The speed control isn't calibrated to a real velocity in `m/s`. Instead, I use a range of values that works for controlling and displaying the launcher.

```cpp
float velocityMagnitude = map(speedDelay, 2, 15, 95, 35);
```

Then the launcher angle is used to split that into two components:

```cpp
float radians = stepperAngle * 0.01745329;

float vx = velocityMagnitude * cos(radians);
float vy = velocityMagnitude * sin(radians);
```

So the display can show which way the spacecraft is initially moving and roughly how fast.

---

## The display

The TFT starts with:

```text
Orbital Simulator

Status: READY
```

It also shows a vector from a fixed point on the screen.

The vector:

* points in the same direction as the launcher
* gets longer when the selected speed is higher
* turns red while launching
* goes back to green when the system is ready

When the launcher sends:

```text
LAUNCH
```

the display changes to:

```text
Status: LAUNCHING!
```

When it sends:

```text
READY
```

the display goes back to normal.

---

## Target LEDs

The NeoPixel strip is basically a target indicator.

Normally the LEDs are blue, and one randomly selected LED is red. The red LED is the target for that attempt. A new target is selected after every launch.
The code currently has:

```cpp
#define NUM_LEDS 10
#define SAFE_LEDS 9
```

so only the first 9 LEDs can become targets.

I did this because the last positions weren't reachable in my physical setup. You can change `SAFE_LEDS` if your layout is different.

---

## Communication

The two Arduinos communicate over regular hardware serial at `9600` baud.

The launcher sends telemetry in this format:

```text
angle,speedDelay,vx,vy
```

For example:

```text
90,5,0.0,75.0
```

It also sends:

```text
LAUNCH
```

when the button is pressed and:

```text
READY
```

when the launch cycle is finished.

The display currently mainly uses the angle and speed values to draw the vector, although `vx` and `vy` are also included in the serial packet.

---

## Libraries

The project uses:

```text
Servo
Adafruit GFX
MCUFRIEND_kbv
Adafruit NeoPixel
```

Install them through the Arduino Library Manager before compiling the display controller.

---

## Project structure

```text
event-horizon/
│
├── launch-controller/
│   └── launch-controller.ino
│
├── display-controller/
│   └── display-controller.ino
│
├── README.md
├── .gitignore
└── .gitattributes
```

`launch-controller.ino` contains the launcher code.

`display-controller.ino` contains the TFT and NeoPixel code.

---

## Setup

Upload `launch-controller.ino` to the Arduino connected to the launcher.

Upload `display-controller.ino` to the Arduino connected to the TFT and LEDs.

Then connect:

```text
Launch Arduino TX → Display Arduino RX
```

The boards also need a common ground.

Both use:

```text
9600 baud
```

Once everything is powered, the display should show `Status: READY`.

Then:

1. choose the angle
2. choose the speed
3. arrange the magnets
4. check the target LED
5. press the launch button

and see where the ball goes.

---

## Things that aren't finished

This is still an experimental project, so there are a few things I haven't implemented yet.

The displayed velocity isn't a real measured velocity. It is currently just a value derived from the speed-control setting.

There is also no automatic target detection yet. The system tells you what the target is, but it doesn't know whether the ball actually hit it.

Some possible additions would be:

* measuring the real launch velocity
* adding sensors to detect the ball
* automatically detecting whether a target was hit
* logging launches and their parameters
* calculating/predicting trajectories
* using a camera to track the ball

---

## What I was going for

I wanted this to feel more like a gamified physical experiment than a normal simulator.

You choose the initial conditions, launch the ball, and then the environment takes over.

Changing the magnet positions or the launch parameters changes what actually happens, rather than just changing a graph on a screen.

That is basically what Event Horizon is: a little physical system where the electronics set up the experiment and the physics takes it from there.
