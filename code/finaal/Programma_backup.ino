#include "SerialCommand.h"
#include "EEPROMAnything.h"

#define SerialPort Serial1
#define Baudrate 9600


#define  BIN2 9 // MotorRightForward (Benable)
#define  BIN1 10 // MotorRightBackward (Bphase)
#define  AIN2 3 // MotorLeftForward(Aenable)
#define  AIN1 11 // MotorLeftForward (Aphase)

SerialCommand sCmd(SerialPort);
bool run = false;
bool debug;
unsigned long previous, calculationTime;
const int sensor[] = {A0, A1, A2, A3, A4, A5};

float debugPosition;
int normalised[6];
struct param_t


{
  unsigned long cycleTime;
  unsigned int power;
  float kd;
  float ki;
  float kp;
  float diff;
  int black[6];
  int white[6];


  
  /* andere parameters die in het eeprom geheugen moeten opgeslagen worden voeg je hier toe ... */
} params;

void setup()
{
  SerialPort.begin(Baudrate);

  sCmd.addCommand("set", onSet);
  sCmd.addCommand("debug", onDebug);
  sCmd.addCommand("calibrate",onCalibrate);
  sCmd.setDefaultHandler(onUnknownCommand);
  
  EEPROM_readAnything(0, params);

  pinMode(13, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(AIN1, OUTPUT);
  SerialPort.println("ready");
}

void loop()
{
  sCmd.readSerial();
 
 
 
  unsigned long current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;

    

 
    for (int i = 0; i < 6; i++)normalised[i] = map(analogRead(sensor[i]), params.black[i], params.white[i], 0,1000);
      
    


    int index = 0;
    for (int i = 1; i < 6; i++) if (normalised[i] < normalised[index]) index = i;

 //   SerialPort.print("index: ");
 //  SerialPort.println(index);

    if (normalised[index] > 700) run =false;


    float position;
    if (index == 0)position = -30;
    else if (index == 5)position = 30;
    else{
     int sNul = normalised[index];
     int sMinEen = normalised [index-1];
     int sPlusEen = normalised[index+1];

     float b = sPlusEen - sMinEen;
     b = b / 2;

     float a = sPlusEen - b - sNul;

     position = -b / (2 * a); 
     position += index;
     position -= 2.5;

     position *= 15;


  
    } 
        //  SerialPort.print("position: ");
  // SerialPort.println(position);
      debugPosition = position;
      float lastErr;
      float error = -position;
      float output = error * params.kp;
      output += params.kd * (error - lastErr);
      lastErr = error;



      output = constrain(output, -510, 510);
   //   SerialPort.print("output: ");
 //  SerialPort.println(output);

      int powerLeft = 0;
      int powerRight = 0;

      if (run) if (output >= 0)
      {
        powerLeft = constrain (params.power + params.diff * output, -255, 255);
        powerRight = constrain(powerLeft - output, -255, 255);
        powerLeft = powerRight + output;
      }
      else
      {
        powerRight = constrain(params.power - params.diff * output, -255, 255);
        powerLeft = constrain(powerRight + output, -255, 255);
        powerRight = powerLeft - output;
      }

 //     SerialPort.print("powerLeft: ");
 //  SerialPort.println(powerLeft);
  //       SerialPort.print("powerRight: ");
  // SerialPort.println(powerRight);


    analogWrite(AIN1, powerLeft > 0 ? powerLeft : 0);
    analogWrite(AIN2, powerLeft < 0 ? -powerLeft : 0);
    analogWrite(BIN2, powerRight > 0 ? powerRight : 0);
    analogWrite(BIN1, powerRight < 0 ? -powerRight : 0);
    /* code die cyclisch moet uitgevoerd worden programmeer je hier ... */

    /* normaliseren en interpoleren sensor */

    /* pid regeling */

    /* aansturen motoren */
  }

  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}

void onUnknownCommand(char *command)
{
  SerialPort.print("unknown command: \"");
  SerialPort.print(command);
  SerialPort.println("\"");
}

void onSet()
{
  char* param = sCmd.next();
  char* value = sCmd.next();  

  if (strcmp(param, "debug") == 0) debug = (strcmp(value, "on")==0);
  else if (strcmp(param, "cycle") == 0) params.cycleTime = atol(value);
  else if (strcmp(param, "kd") == 0) params.kd = atof(value);
  else if (strcmp(param, "ki") == 0) params.ki = atof(value);
  else if (strcmp(param, "kp") == 0) params.kp = atof(value);
  else if (strcmp(param, "diff") == 0) params.diff = atof(value);
  else if (strcmp(param, "power") == 0) params.power = atol(value);
  else if (strcmp(param, "run") == 0) run = true;
  else if (strcmp(param, "stop") == 0) run = false;


  /* parameters een nieuwe waarde geven via het set commando doe je hier ... */
  
  EEPROM_writeAnything(0, params);
}

void onDebug()
{
  SerialPort.print("cycle time: ");
  SerialPort.println(params.cycleTime);

  SerialPort.print("kd: ");
  SerialPort.println(params.kd);

  SerialPort.print("ki: ");
  SerialPort.println(params.ki);

  SerialPort.print("kp: ");
  SerialPort.println(params.kp);

  SerialPort.print("diff: ");
  SerialPort.println(params.diff);
  
  SerialPort.print("power: ");
  SerialPort.println(params.power);

  SerialPort.print("white: ");
  for (int i = 0; i < 6; i++)
  {
    SerialPort.print(params.white [i]);
    SerialPort.print(" ");
  }SerialPort.println();
  
 SerialPort.print("black: ");
  for (int i = 0; i < 6; i++)
  {
    SerialPort.print(params.black [i]);
    SerialPort.print(" ");
  }SerialPort.println();

   SerialPort.print("normalised: ");
  for (int i = 0; i < 6; i++)
  {
    SerialPort.print(normalised[i]);
    SerialPort.print(" ");
  }SerialPort.println();

   SerialPort.print("Position: ");
   SerialPort.println(debugPosition);
   
   SerialPort.print("run: ");
   SerialPort.println(run);
  
 


  /* parameters weergeven met behulp van het debug commando doe je hier ... */
  
  SerialPort.print("calculation time: ");
  SerialPort.println(calculationTime);
  calculationTime = 0;
}

void onCalibrate()
{
  char* param = sCmd.next();

  if (strcmp(param, "black") == 0)
  {
    SerialPort.print("start calibrating black... ");
    for (int i = 0; i < 6; i++) params.black[i]=analogRead(sensor[i]);
    SerialPort.println("done");
  }
  else if (strcmp(param, "white") == 0)
  {
    SerialPort.print("start calibrating white... ");    
    for (int i = 0; i < 6; i++) params.white[i]=analogRead(sensor[i]);  
    SerialPort.println("done");      
  }

  EEPROM_writeAnything(0, params);
}
void stop(){
  run = false;
}
