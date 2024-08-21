void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
}
void loop() {
  Serial.println("Hello");
  digitalWrite(2, !digitalRead(2)); // toggle LED
  delay(1000);
}
