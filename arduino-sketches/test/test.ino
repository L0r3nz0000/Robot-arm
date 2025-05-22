const int stepPin = 3;    // Y-STEP
const int dirPin = 6;     // Y-DIR
const int enablePin = 8;  // EN (Enable)

const int steps = 50;     // Numero di passi per direzione
const int stepDelay = 1000; // Microsecondi tra i passi

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  digitalWrite(enablePin, LOW); // Abilita i driver
}

void loop() {
  // Movimento in una direzione
  digitalWrite(dirPin, HIGH);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }

  delay(500); // Pausa di mezzo secondo

  // Movimento nella direzione opposta
  digitalWrite(dirPin, LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }

  delay(500); // Pausa di mezzo secondo
}
