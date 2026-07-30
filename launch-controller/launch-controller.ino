#include <Servo.h>

#define BUTTON_PIN   12
#define POT_SPEED_PIN A0
#define POT_ANGLE_PIN A1
#define SERVO1_PIN   6
#define SERVO2_PIN   7

#define STEP_PIN1 2
#define STEP_PIN2 4
#define STEP_PIN3 3
#define STEP_PIN4 5

Servo myServo1;
Servo myServo2;

int currentStep = 256; 
unsigned long lastSerialTime = 0;
unsigned long lastStepperTime = 0; 


const byte stepSequence[4] = {
  B1000, // IN1 High
  B0100, // IN2 High
  B0010, // IN3 High
  B0001  // IN4 High
};
int phaseIndex = 0;

void moveOneStep(int direction) {
  phaseIndex += direction;
  if (phaseIndex > 3) phaseIndex = 0;
  if (phaseIndex < 0) phaseIndex = 3;
  
  byte currentPhase = stepSequence[phaseIndex];
  
  digitalWrite(STEP_PIN1, bitRead(currentPhase, 3));
  digitalWrite(STEP_PIN2, bitRead(currentPhase, 2));
  digitalWrite(STEP_PIN3, bitRead(currentPhase, 1));
  digitalWrite(STEP_PIN4, bitRead(currentPhase, 0));
}

void disableStepperOutputs() {
  digitalWrite(STEP_PIN1, LOW);
  digitalWrite(STEP_PIN2, LOW);
  digitalWrite(STEP_PIN3, LOW);
  digitalWrite(STEP_PIN4, LOW);
}

void setup() {
  Serial.begin(9600); 
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(STEP_PIN1, OUTPUT);
  pinMode(STEP_PIN2, OUTPUT);
  pinMode(STEP_PIN3, OUTPUT);
  pinMode(STEP_PIN4, OUTPUT);
  
  myServo1.attach(SERVO1_PIN);
  myServo2.attach(SERVO2_PIN);
  myServo1.write(0);       
  myServo2.write(180); 
}

void loop() {
  int anglePot = analogRead(POT_ANGLE_PIN);
  int targetStep = map(anglePot, 0, 1023, 0, 512);
  
  int speedPot = analogRead(POT_SPEED_PIN);
  int speedDelay = map(speedPot, 0, 1023, 2, 15); 

  unsigned long stepperInterval = 10; //a deadzone so stepper doesnt jitter

  if (abs(currentStep - targetStep) > 5) { 
    if (millis() - lastStepperTime >= stepperInterval) { 
      lastStepperTime = millis();
      
      if (currentStep < targetStep) { 
        moveOneStep(1); 
        currentStep++; 
      }
      else if (currentStep > targetStep) { 
        moveOneStep(-1); 
        currentStep--; 
      }
    }
  } else {
    disableStepperOutputs(); // Turns off coils when target is reached 
  }

  int stepperAngle = map(currentStep, 0, 512, 0, 180);

  // math
  float velocityMagnitude = map(speedDelay, 2, 15, 95, 35); 
  float radians = stepperAngle * 0.01745329; 
  float vx = velocityMagnitude * cos(radians);
  float vy = velocityMagnitude * sin(radians);

  // packet send
  if (millis() - lastSerialTime > 60) { 
    lastSerialTime = millis();
    Serial.print(stepperAngle);
    Serial.print(",");
    Serial.print(speedDelay);
    Serial.print(",");
    Serial.print(vx, 1); 
    Serial.print(",");
    Serial.println(vy, 1); 
  }

  // launch
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); 
    if (digitalRead(BUTTON_PIN) == LOW) { 
      Serial.println("LAUNCH");
      
      for (int pos = 0; pos <= 180; pos++) {
        myServo1.write(pos);
        myServo2.write(180 - pos);
        delay(speedDelay); 
      }
      delay(150); 
      for (int pos = 180; pos >= 0; pos--) {
        myServo1.write(pos);
        myServo2.write(180 - pos);
        delay(3); 
      }

      delay(2000); 

      Serial.println("READY");
    }
  }
}