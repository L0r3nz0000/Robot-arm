#include <Arduino.h>
#include "stepper.hpp"

#define STEPPER_0_STEP 2
#define STEPPER_0_DIR 5
#define STEPPER_1_STEP 3
#define STEPPER_1_DIR 6
#define STEPPER_2_STEP 4
#define STEPPER_2_DIR 7

#define ENABLE 8
#define BUZZER 11

struct ArmPosition {
  float theta0;  // Base
  float theta1;  // Primo snodo (alla base)
  float theta2;  // Secondo snodo
};

long speed = 300;  // Velocità (step/secondo)

const float L1_size = 13.2, L2_size = 14.6;
const float scale_ratio = 2.5;
const float scale_ratio_rotation = 5.65;

const int theta0_max_extension = 140;
int theta1_max_extension;

bool is_moving = false;

// Angoli dei motori
ArmPosition angles = (ArmPosition) {0, 0, 0};

StepperMotor motors[3] = {
  StepperMotor(STEPPER_0_STEP, STEPPER_0_DIR, scale_ratio),
  StepperMotor(STEPPER_1_STEP, STEPPER_1_DIR, scale_ratio),
  StepperMotor(STEPPER_2_STEP, STEPPER_2_DIR, scale_ratio_rotation)
};

void set_angles(ArmPosition target_angles) {
  if (target_angles.theta0 <= theta0_max_extension) {
    motors[0].setTargetDegrees(target_angles.theta0 * scale_ratio);
    angles.theta0 = target_angles.theta0;
  } else {
    motors[0].setTargetDegrees(theta0_max_extension);
    angles.theta0 = theta0_max_extension;
  }

  if (target_angles.theta1 < target_angles.theta0 ){
    motors[1].setTargetDegrees(-(target_angles.theta0 - target_angles.theta1) * scale_ratio);
  } else {
    motors[1].setTargetDegrees(0);
  }
  
  motors[2].setTargetDegrees(target_angles.theta2 * scale_ratio_rotation);
}

void moveTo(ArmPosition target_angles) {
  set_angles(target_angles);

  while (motors[0].isRunning() || motors[1].isRunning() || motors[2].isRunning()) {
    motors[0].run();
    motors[1].run();
    motors[2].run();
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Inizializzazione...");
  Serial.readStringUntil('\n');
  Serial.setTimeout(500);

  // Inizializza i motori
  for (int i = 0; i < 2; i++) {
    motors[i].setMinSpeed(5);
    motors[i].setMaxSpeed(speed);
    motors[i].setAcceleration(8000);
  }
  motors[2].setMinSpeed(5);
  motors[2].setMaxSpeed(1800);
  motors[2].setAcceleration(12000);


  // Inizializza il pin di abilitazione
  pinMode(ENABLE, OUTPUT);
  // Inizializza il pin del buzzer
  pinMode(BUZZER, OUTPUT);

  // digitalWrite(ENABLE, HIGH); // Disabilita i driver

  // for (int i = 0; i < 3; i++) {
  //   delay(50);
  //   digitalWrite(BUZZER, HIGH);
  //   delay(100);
  //   digitalWrite(BUZZER, LOW);
  // }

  // delay(4000);
  // digitalWrite(BUZZER, HIGH);
  // delay(600);
  // digitalWrite(BUZZER, LOW);

  digitalWrite(ENABLE, LOW); // Abilita i driver
  delay(300);

  // Inizializza la posizione iniziale
  angles.theta0 = 0;
  angles.theta1 = 0;
  angles.theta2 = 0;

  moveTo({45, 63, 0});
  moveTo({0, 0, 0});
}

void loop() {
  if (Serial.available()) {
    Serial.readStringUntil('\t');  // inizio comunicazione
    float t0 = Serial.readStringUntil(' ').toInt();
    float t1 = Serial.readStringUntil(' ').toInt();
    float t2 = Serial.readStringUntil('\n').toInt();
    
    if (t0 != 0) {
      motors[0].runContinuously(t0);
    } else {
      motors[0].stop();
    }

    if (t1 != 0) {
      motors[1].runContinuously(t1);
    } else {
      motors[1].stop();
    }

    if (t2 != 0) {
      motors[2].runContinuously(t2);
    } else {
      motors[2].stop();
    }

    // Nuovo comando ricevuto, aggiorna target e inizia il movimento
    is_moving = true;
    Serial.print("theta0: ");
    Serial.print(motors[0].getPositionDegree());
    Serial.print(" theta1: ");
    Serial.print(motors[1].getPositionDegree());
    Serial.print(" theta2: ");
    Serial.println(motors[2].getPositionDegree());
  }

  // Esegui il movimento se attivo
  if (is_moving) {
    motors[0].run();
    motors[1].run();
    motors[2].run();

    if (!motors[0].isRunning() && !motors[1].isRunning() && !motors[2].isRunning()) {
      is_moving = false; // movimento completato
    }
  }
}