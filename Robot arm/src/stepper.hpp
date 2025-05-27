#include <Arduino.h>

// Configurazione dei driver
const float angle_for_step = 1.8;  // 1.8° per step con i nema 17
const int microsteps = 16;  // 1=full step, 2=half step, 4=quarter step...
const float reduction_ratio = 1.45;
const float steps_per_degree = (1.0 / angle_for_step) * microsteps * reduction_ratio;

class StepperMotor {
  private:
    int step_pin, dir_pin;
    unsigned int maxSpeed = 1000 * microsteps;   // Max speed in steps/sec
    unsigned int minSpeed = 100 * microsteps;    // Min speed (starting speed)
    unsigned int accel = 200;       // Acceleration in steps/sec^2

    int positionStep = 0;
    float positionDegree = 0;
    int targetSteps = 0;
    unsigned long lastStepTime = 0;

    float currentSpeed = 0;         // In steps/sec
    float stepInterval = 0;         // In ms

    int direction = 1;              // 1: avanti, -1: indietro
    bool running = false;

    void step() {
      digitalWrite(dir_pin, direction == 1 ? HIGH : LOW);
      digitalWrite(step_pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(step_pin, LOW);
      delayMicroseconds(10);

      positionStep += direction;
      positionDegree += direction * (1.0 / steps_per_degree);
    }

    void computeSpeed() {
      int stepsRemaining = abs(targetSteps - positionStep);
      float targetSpeed = maxSpeed;

      // Accelerazione
      float accelSteps = (float)(currentSpeed * currentSpeed) / (2.0 * accel);
      if (stepsRemaining < accelSteps * 2) {
        // Decelera
        targetSpeed = sqrt(2.0 * accel * (stepsRemaining / 2.0));
        if (targetSpeed < minSpeed) targetSpeed = minSpeed;
      } else if (currentSpeed < maxSpeed) {
        // Accelera
        targetSpeed = currentSpeed + accel * ((millis() - lastStepTime) / 1000.0);
        if (targetSpeed > maxSpeed) targetSpeed = maxSpeed;
      }

      currentSpeed = targetSpeed;
      stepInterval = 1000.0 / currentSpeed;
    }

  public:
    StepperMotor(int step_pin, int dir_pin) {
      this->step_pin = step_pin;
      this->dir_pin = dir_pin;

      pinMode(step_pin, OUTPUT);
      pinMode(dir_pin, OUTPUT);
      digitalWrite(step_pin, LOW);
      digitalWrite(dir_pin, LOW);
    }

    void setMaxSpeed(unsigned int speed) { if (speed > 0) maxSpeed = speed * microsteps; }
    void setMinSpeed(unsigned int speed) { if (speed > 0) minSpeed = speed * microsteps; }
    void setAcceleration(unsigned int a) { if (a > 0) accel = a; }

    void setTargetDegrees(float angleDeg) {
      int steps = lround(angleDeg * steps_per_degree);
      targetSteps = steps;
      direction = (targetSteps > positionStep) ? 1 : -1;
      running = true;
      currentSpeed = minSpeed;
      stepInterval = 1000.0 / currentSpeed;
      lastStepTime = millis();
    }

    void run() {
      if (!running) return;

      if ((millis() - lastStepTime) >= stepInterval) {
        if (positionStep == targetSteps) {
          running = false;
          return;
        }
        step();
        computeSpeed();
        lastStepTime = millis();
      }
    }

    void stop() {
      running = false;
      targetSteps = positionStep;
    }

    bool isRunning() { return running; }
    int getPositionStep() { return positionStep; }
    float getPositionDegree() { return positionDegree; }
};
