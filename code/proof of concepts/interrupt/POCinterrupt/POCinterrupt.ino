const int StartKnop = 3;
const int LedBlauw = 4;

bool StatusLed = false;

void setup() {
  Serial.begin(9600);

  pinMode(LedBlauw, OUTPUT);
  pinMode(StartKnop, INPUT);  

  attachInterrupt(digitalPinToInterrupt(StartKnop), Interrupt, RISING);
}

void loop() {
  // Blauwe led aan of uit
  digitalWrite(LedBlauw, StatusLed);
}

void Interrupt() {
  Serial.println("Interrupt");
  StatusLed = !StatusLed;
}
