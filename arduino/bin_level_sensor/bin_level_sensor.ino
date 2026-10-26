/*
  Bin Level Sensor
  Arduino Uno R3 + HC-SR04

  Required:
    HC-SR04 TRIG -> D9
    HC-SR04 ECHO -> D10

  Optional:
    Green LED -> D4
    Yellow LED -> D5
    Red LED -> D6
    Active buzzer -> D7
*/

const byte TRIG_PIN = 9;
const byte ECHO_PIN = 10;
const byte GREEN_LED_PIN = 4;
const byte YELLOW_LED_PIN = 5;
const byte RED_LED_PIN = 6;
const byte BUZZER_PIN = 7;

// Calibrate these values for the installed bin.
const float EMPTY_DISTANCE_CM = 30.0;
const float FULL_DISTANCE_CM = 5.0;
const unsigned long MEASUREMENT_INTERVAL_MS = 1000;
const unsigned long ECHO_TIMEOUT_US = 30000;

enum BinStatus {
  STATUS_EMPTY,
  STATUS_OK,
  STATUS_FULL,
  STATUS_UNKNOWN
};

BinStatus previousStatus = STATUS_UNKNOWN;
unsigned long lastMeasurementMs = 0;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(TRIG_PIN, LOW);
  clearIndicators();
  Serial.begin(9600);
}

void loop() {
  if (millis() - lastMeasurementMs < MEASUREMENT_INTERVAL_MS) {
    return;
  }
  lastMeasurementMs = millis();

  float distanceCm = measureDistanceCm();
  if (distanceCm < 0) {
    clearIndicators();
    Serial.println(F("BIN,NA,NA,UNKNOWN"));
    previousStatus = STATUS_UNKNOWN;
    return;
  }

  int fillPercent = calculateFillPercent(distanceCm);
  BinStatus status = getStatus(fillPercent);
  updateIndicators(status);

  Serial.print(F("BIN,"));
  Serial.print(distanceCm, 2);
  Serial.print(',');
  Serial.print(fillPercent);
  Serial.print(',');
  printStatus(status);
  Serial.println();

  if (status == STATUS_FULL && previousStatus != STATUS_FULL) {
    tone(BUZZER_PIN, 2200, 250);
  }
  previousStatus = status;
}

float measureDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long durationUs = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
  if (durationUs == 0) {
    return -1;
  }
  return durationUs / 58.0;
}

int calculateFillPercent(float distanceCm) {
  float percent = (EMPTY_DISTANCE_CM - distanceCm) * 100.0 /
                  (EMPTY_DISTANCE_CM - FULL_DISTANCE_CM);
  percent = constrain(percent, 0.0, 100.0);
  return (int)(percent + 0.5);
}

BinStatus getStatus(int fillPercent) {
  if (fillPercent <= 25) {
    return STATUS_EMPTY;
  }
  if (fillPercent <= 75) {
    return STATUS_OK;
  }
  return STATUS_FULL;
}

void updateIndicators(BinStatus status) {
  digitalWrite(GREEN_LED_PIN, status == STATUS_EMPTY ? HIGH : LOW);
  digitalWrite(YELLOW_LED_PIN, status == STATUS_OK ? HIGH : LOW);
  digitalWrite(RED_LED_PIN, status == STATUS_FULL ? HIGH : LOW);
}

void clearIndicators() {
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

void printStatus(BinStatus status) {
  if (status == STATUS_EMPTY) {
    Serial.print(F("EMPTY"));
  } else if (status == STATUS_OK) {
    Serial.print(F("OK"));
  } else if (status == STATUS_FULL) {
    Serial.print(F("FULL"));
  } else {
    Serial.print(F("UNKNOWN"));
  }
}
