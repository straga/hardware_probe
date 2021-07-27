
#include <RBDdimmer.h> // fixe by self lib from git not works
#include <I2C_Anything.h>
// esp
#define outputPin  D6 
#define zerocross  D5

////stm32
//#define outputPin  PC14 
//#define zerocross  PC15
//#include<Wire_slave.h> // for i2c slave

struct message_t {
    byte commandType;
    int data;
};
message_t msg ;
dimmerLamp dimmer(outputPin, zerocross);

#include<Wire.h>
#define USE_SERIAL  Serial
int msg_repply;

byte cmd;
int i;
int power_max;
int power_min;

void setup()
{
  USE_SERIAL.begin(9600);

  dimmer.begin(NORMAL_MODE, ON);
  dimmer.setPower(0);
   
  Wire.begin (10);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendEvent);

  power_min = 0;
  power_max = 100;
  msg_repply = -1;
}

void loop()  {
  
  }

void setPowerTriac(int power_val) {
  if (power_val >= 0 && power_val <=100){
    dimmer.setPower(map(power_val, 0, 100, power_min, power_max));
   }
}

void receiveEvent(int size_msg)
{
  
//USE_SERIAL.println(size_msg);

//I2C_readAnything (msg.commandType);
//I2C_readAnything (msg.data);
//
//USE_SERIAL.println(msg.commandType);
//USE_SERIAL.println(msg.commandType, BIN);
//USE_SERIAL.println("");
//USE_SERIAL.println(msg.data);
//USE_SERIAL.println(msg.data, BIN);
//USE_SERIAL.println("");
//USE_SERIAL.println("");

  if (size_msg >= (sizeof msg.commandType))
  {
    I2C_readAnything (msg.commandType);
  }
    
  if (size_msg >= (sizeof msg.commandType) + (sizeof msg.data))
  {    

    I2C_readAnything (msg.data);   
  }  

  switch (msg.commandType) {

    case 0x31: // get power  - command 49
      msg_repply = dimmer.getPower();
      USE_SERIAL.print("get Power:"+ String(msg_repply));
      break;
    
    case 0x32: // set_power - command 50
      USE_SERIAL.println("setPower:" + String(msg.data) );
      setPowerTriac(msg.data);
      break;

    case 0x44:  // power_min - command 68
      power_min = msg.data;
      USE_SERIAL.print("power_min:"+ String(power_min));
      break;

    case 0x45:  // power_max - command 69
      power_max = msg.data;
      USE_SERIAL.print("power_max:"+ String(power_max));
      break;

    case 0x54: // get power_max - command 84
      msg_repply = power_max; 
      USE_SERIAL.print("power_max:"+ String(msg_repply));
      break;

    case 0x55: // get power_min - command 85
      msg_repply = power_min; 
      USE_SERIAL.print("power_min:"+ String(msg_repply));
      break;

    case 0x21: // get state  - command 133
      msg_repply = dimmer.getState();
      USE_SERIAL.print("get State:"+ String(msg_repply));
      break;
      
    case 0x22: // set_state - command 50
      USE_SERIAL.println("set State:" + String(msg.data) );
      if (msg.data == 1){ dimmer.setState(ON); }
      if (msg.data == 0){ dimmer.setState(OFF); }
      
      break;

    default:  // unknown command, blank all the data
      USE_SERIAL.println("Default");
      msg_repply = -1;
  }
}


void sendEvent()
{
    USE_SERIAL.println("Send: "+String(msg_repply));
    I2C_writeAnything(msg_repply);
    msg_repply = -1;    
}
