unsigned long last = 0;
bool blinkState = false;

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
}

void loop() {
  byte count = 0;
  if (digitalRead(4)) {
    count += 4;
  }

  if (digitalRead(5)) {
    count += 5;
  }

  

  while (count) {
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW);
    delay(100);
    count--;
  }

  delay(2000);
}
