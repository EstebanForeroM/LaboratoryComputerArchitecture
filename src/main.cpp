#include <Arduino.h>

#define LED1_PIN 14
#define LED2_PIN 27
#define LED3_PIN 26
#define LED4_PIN 25

volatile int currentStep = 0;     
volatile bool updateSerial = false; 

hw_timer_t *timer = NULL;         

void IRAM_ATTR Timer0_ISR() {
  currentStep = (currentStep + 1) % 4;

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED4_PIN, LOW);

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

  timer = timerBegin(0, 80, true); // Timer 0, prescaler 80, count up
  timerAttachInterrupt(timer, &Timer0_ISR, true);
  timerAlarmWrite(timer, 1500000, true);
  timerAlarmEnable(timer);
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
}
