int BIN_1 = 11;
int BIN_2 = 3;
int AIN_1 = 10;
int AIN_2 = 9;

void setup() {
    Serial.begin(9600); 
    pinMode(BIN_1, OUTPUT);
    pinMode(BIN_2, OUTPUT);
    pinMode(AIN_1, OUTPUT);
    pinMode(AIN_2, OUTPUT);
}

void loop() {



    
    //Voorwaarts 
 
    for(int i = 0; i <= 255; i++) 
    {
      analogWrite(BIN_1, i);
      analogWrite(BIN_2, 0);
      analogWrite(AIN_1, i);
      analogWrite(AIN_2, 0);
      delay(20);
    }
    delay(3000);
    
 
    for(int i = 255; i >= 0; i--) 
    { 
      analogWrite(BIN_1, i);
      analogWrite(BIN_2, 0);
      analogWrite(AIN_1, i);
      analogWrite(AIN_2, 0);
      delay(20);
    }
    delay(3000); 

     for(int i = 0; i <= 255; i++) 
    {
      analogWrite(BIN_1, 0);
      analogWrite(BIN_2, i);
      analogWrite(AIN_1, 0);
      analogWrite(AIN_2, i);
      delay(20);
    }
    delay(3000);
    
 
    for(int i = 255; i >= 0; i--) 
    { 
      analogWrite(BIN_1, 0);
      analogWrite(BIN_2, i);
      analogWrite(AIN_1, 0);
      analogWrite(AIN_2, i);
      delay(20);
    }
    delay(3000); 

  
  }
