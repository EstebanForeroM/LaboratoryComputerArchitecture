#include <Arduino.h>

#define LED1_PIN 14
#define LED2_PIN 27
#define LED3_PIN 26
#define LED4_PIN 25
#define BUTTON_PIN 33

volatile int currentStep = 0;     
volatile bool updateSerial = false; 
volatile bool buttonPressed = false;

hw_timer_t *timerSequence = NULL;         
hw_timer_t *timerButton = NULL;

void applyStateToAllLeds(int state) {
  digitalWrite(LED1_PIN, state);
  digitalWrite(LED2_PIN, state);
  digitalWrite(LED3_PIN, state);
  digitalWrite(LED4_PIN, state);
}

void IRAM_ATTR Timer0_ISR() {
  currentStep = (currentStep + 1) % 4;

  applyStateToAllLeds(LOW);

  switch (currentStep) {
    case 0:
      digitalWrite(LED1_PIN, HIGH);
      break;
    case 1:
      digitalWrite(LED2_PIN, HIGH);
      break;
    case 2:
      digitalWrite(LED3_PIN, HIGH);
      break;
    case 3:
      digitalWrite(LED4_PIN, HIGH);
      break;
  }

  updateSerial = true;
}

void IRAM_ATTR ButtonPressed() {
  buttonPressed = true;
  timerAlarmDisable(timerSequence);
  timerAlarmEnable(timerButton);
  applyStateToAllLeds(HIGH);
}

void IRAM_ATTR reactivateSequence() {
  timerAlarmEnable(timerSequence);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED1_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);

  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED2_PIN, LOW);

  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED3_PIN, LOW);

  pinMode(LED4_PIN, OUTPUT);
  digitalWrite(LED4_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT);

  timerSequence = timerBegin(0, 80, true); 
  timerAttachInterrupt(timerSequence, &Timer0_ISR, true);
  timerAlarmWrite(timerSequence, 1500000, true);
  timerAlarmEnable(timerSequence);

  timerButton = timerBegin(1, 80, true);
  timerAttachInterrupt(timerButton, &reactivateSequence, true);
  timerAlarmWrite(timerButton, 2000000, true);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), &ButtonPressed, RISING);
}

void loop() {
  if (updateSerial) {
    noInterrupts();
    updateSerial = false; 
    int step = currentStep;
    interrupts();

    Serial.print("LED1: ");
    Serial.print(step == 0 ? "ON" : "OFF");
    Serial.print(" / LED2: ");
    Serial.print(step == 1 ? "ON" : "OFF");
    Serial.print(" / LED3: ");
    Serial.print(step == 2 ? "ON" : "OFF");
    Serial.print(" / LED4: ");
    Serial.println(step == 3 ? "ON" : "OFF");
  }

  if (buttonPressed) {
    noInterrupts();
    buttonPressed = false;
    interrupts();

    Serial.println("The button was pressed");
  }
}