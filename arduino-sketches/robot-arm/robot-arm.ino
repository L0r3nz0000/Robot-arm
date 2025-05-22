#include <AccelStepper.h>
#include <math.h>
#include <stdlib.h>

/*    MODIFICARE I NUMERI DEI PIN   */
#define STEPPER_0_STEP 2
#define STEPPER_0_DIR 5
#define STEPPER_1_STEP 3
#define STEPPER_1_DIR 6
#define STEPPER_2_STEP 0
#define STEPPER_2_DIR 0
#define STEPPER_3_STEP 0
#define STEPPER_3_DIR 0

#define ENABLE 8

#define NUM_MOTORS 4

AccelStepper steppers[NUM_MOTORS] = {
  AccelStepper(AccelStepper::DRIVER, STEPPER_0_STEP, STEPPER_0_DIR),  // Motore 0: STEP su 2, DIR su 3
  AccelStepper(AccelStepper::DRIVER, STEPPER_1_STEP, STEPPER_1_DIR),  // Motore 1: STEP su 4, DIR su 5
  AccelStepper(AccelStepper::DRIVER, STEPPER_2_STEP, STEPPER_2_DIR),  // Motore 2: STEP su 6, DIR su 7
  AccelStepper(AccelStepper::DRIVER, STEPPER_3_STEP, STEPPER_3_DIR)   // Motore 3: STEP su 8, DIR su 9
};

struct ArmPosition {
  float theta0;  // Base
  float theta1;  // Primo snodo (alla base)
  float theta2;  // Secondo snodo
};

// Configurazione dei driver
const float angle_for_step = 1.8;  // 1.8° per step con i nema 17
const int microsteps = 1;  // 1=full step, 2=half step, 4=quarter step.... 
const float reduction_ratio = 1.45;  // DA VERIFICARE!!
const float steps_per_degree = (1.0 / angle_for_step) * microsteps * reduction_ratio; // Step per grado
long max_speed = 200;  // Velocità massima (step/secondo)
long max_accel = 200;  // Accelerazione (step/secondo²)

const float L1_size = 13.2, L2_size = 14.6;

// Angoli dei motori
ArmPosition angles = (ArmPosition) {0, 0, 0};

bool isValidInt(String str, int &num) {
  char *endptr;
  long val = strtol(str.c_str(), &endptr, 10);
  
  if (*endptr == '\0') { // Controlla che tutta la stringa sia stata convertita
    num = (int)val;
    return true;
  }
  return false;
}

bool isValidFloat(String str, float &num) {
  char *endptr;
  num = atof(str.c_str()); // Controllare errori , &endptr);
  
  return (*endptr == '\0'); // Controlla che tutta la stringa sia stata convertita
}

ArmPosition calcolaAngoli(float X, float Y, float Z, float L1, float L2) {
  ArmPosition angoli;

  // Calcola angolo della base
  angoli.theta0 = atan2(Y, X) * 180.0 / M_PI;

  // Distanza della punta nel piano verticale
  float R = sqrt(X * X + Z * Z);

  // Controllo se la posizione è raggiungibile
  if (R > (L1 + L2) || R < fabs(L1 - L2)) {
    angoli.theta1 = angoli.theta2 = NAN; // Non raggiungibile
    return angoli;
  }

  // Angolo theta1
  float cosTheta1 = (L1 * L1 + R * R - L2 * L2) / (2 * L1 * R);
  float sinTheta1 = sqrt(1 - cosTheta1 * cosTheta1);
  angoli.theta1 = (atan2(Z, X) + atan2(sinTheta1, cosTheta1)) * 180.0 / M_PI;

  // Angolo theta2
  float cosTheta2 = (L1 * L1 + L2 * L2 - R * R) / (2 * L1 * L2);
  angoli.theta2 = acos(cosTheta2) * 180.0 / M_PI;

  return angoli;
}

void setAngle(AccelStepper &s, float &positionDeg, float degrees) {
  // passi totali corrispondenti all'angolo assoluto
  long targetSteps  = lround(degrees * steps_per_degree);
  // passi corrispondenti alla posizione corrente
  long currentSteps = lround(positionDeg * steps_per_degree);
  // differenza da fare (può essere positivo o negativo)
  long deltaSteps   = targetSteps - currentSteps;

  s.move(deltaSteps);
  positionDeg = degrees;  // ora la posizione "corrente" è proprio quell'angolo
}

void setArmPosition(ArmPosition angles) {
  setAngle(steppers[0], angles.theta0, angles.theta0);
  setAngle(steppers[1], angles.theta1, angles.theta1);
  setAngle(steppers[2], angles.theta2, angles.theta2);
}

String command = "";
float fval;
long ival;

bool somethingIsMoving = false;

void setup() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    steppers[i].setMaxSpeed(max_speed);
    steppers[i].setAcceleration(max_accel);
  }

  pinMode(ENABLE, OUTPUT);
  digitalWrite(ENABLE, LOW);
  Serial.begin(115200);
}

void loop() {
  angles.theta0 = Serial.readStringUntil('\t').toFloat();
  angles.theta1 = Serial.readStringUntil('\t').toFloat();
  angles.theta2 = Serial.readStringUntil('\t').toFloat();
  setArmPosition(angles);
}
