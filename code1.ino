#include <PS2X_lib.h>  //for v1.6
#include "ramps14.h"

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you conect the controller, 
//or call config_gamepad(pins) again after connecting the controller.
int error = 0; 
byte type = 0;
byte vibrate = 0;

int R1PWM_Output = 2; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int R2PWM_Output = 3; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)

int L1PWM_Output = 4; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int L2PWM_Output = 5; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)

int defaultSpeed = 255;
String inString = ""; 
int motorPWM = 0;
int leftAdd = 0; 

bool cutting = LOW;

//int motor1Pin1 = 43;
//int motor1Pin2 = 47; 
//int motor1En   = 45;

int relayControl = 47;

int defaultCuttingSpeed = 40;
int currentCuttingSpeed = 40;


#define DELAY_X      1500 
#define STEP         20
#define MAX_CUTTING_SPEED 230

void setup(){
  
   Serial.begin(115200);

   pinMode(R1PWM_Output, OUTPUT);
   pinMode(R2PWM_Output, OUTPUT);

   pinMode(L1PWM_Output, OUTPUT);
   pinMode(L2PWM_Output, OUTPUT);

   pinMode(relayControl, OUTPUT);
 //  pinMode(motor1Pin2, OUTPUT);

 //  pinMode(motor1En, OUTPUT); 


   pinMode(  Axis1DirPin , OUTPUT);
   pinMode(  Axis1StepPin , OUTPUT);
   pinMode( Axis1_EN , OUTPUT);

   pinMode(  Axis2DirPin , OUTPUT);
   pinMode(  Axis2StepPin , OUTPUT);
   pinMode( Axis2_EN , OUTPUT);
  
   pinMode(  Axis3DirPin , OUTPUT);
   pinMode(  Axis3StepPin , OUTPUT);
   pinMode( Axis3_EN , OUTPUT);

   digitalWrite(Axis1_EN , HIGH);
   digitalWrite(Axis2_EN , HIGH);
   digitalWrite(Axis3_EN , HIGH);
   

 //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
 error = ps2x.config_gamepad(52,51,53,50, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
 
 if(error == 0){
   Serial.println("Found Controller, configured successful");
   Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
  Serial.println("holding L1 or R1 will print out the analog stick values.");
  Serial.println("Go to www.billporter.info for updates and to report bugs.");
 }
   
  else if(error == 1)
   Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
   Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
  else if(error == 3)
   Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
   
   //Serial.print(ps2x.Analog(1), HEX);
   
   type = ps2x.readType(); 
     switch(type) {
       case 0:
        Serial.println("Unknown Controller type");
       break;
       case 1:
        Serial.println("DualShock Controller Found");
       break;
       case 2:
         Serial.println("GuitarHero Controller Found");
       break;
     }
  
}

void loop(){
   /* You must Read Gamepad to get new values
   Read GamePad and set vibration values
   ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
   if you don't enable the rumble, use ps2x.read_gamepad(); with no values
   
   you should call this at least once a second
   */
   
   
   
 if(error == 1){ //skip loop if no controller found

  stopAll();
  return; 
 } 
 
 else { //DualShock Controller
  
    ps2x.read_gamepad(false, vibrate);          //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START)){                   //will be TRUE as long as button is pressed
         Serial.println("Start is being held");
       
    }     
    if(ps2x.Button(PSB_SELECT))
         Serial.println("Select is being held");
         

         
     if(ps2x.Button(PSB_PAD_UP)) {         //will be TRUE as long as button is pressed
       Serial.print("Up held this hard: ");
       Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
      
         if( ps2x.Analog(PSAB_PAD_UP) == 255)
            runForward();
        
      }
      if(ps2x.Button(PSB_PAD_RIGHT)){
       Serial.print("Right held this hard: ");
        Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
            if( ps2x.Analog(PSAB_PAD_RIGHT) == 255)
                turnRight();
      }
      if(ps2x.Button(PSB_PAD_LEFT)){
       Serial.print("LEFT held this hard: ");
        Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
            if( ps2x.Analog(PSAB_PAD_LEFT) == 255)
                turnLeft();
      }
      if(ps2x.Button(PSB_PAD_DOWN)){
       Serial.print("DOWN held this hard: ");
       Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
           if( ps2x.Analog(PSAB_PAD_DOWN) == 255)
            runBackward();
      }  

     if(ps2x.Button(PSB_PINK)){
           Serial.print("Square held this hard: ");
          Serial.println(ps2x.Analog(PSAB_PINK), DEC);
          if( ps2x.Analog(PSAB_PINK) == 255){
             armRight_to_Left();
          }
     }
     if(ps2x.Button(PSB_RED)){
           Serial.print("Circle held this hard: ");
          Serial.println(ps2x.Analog(PSAB_RED), DEC);
          if( ps2x.Analog(PSAB_RED) == 255){
             armLeft_to_Right();
          }
     }

     if(ps2x.Button(PSB_L1)){
           Serial.print("L1 held this hard: ");
          Serial.println(ps2x.Analog(PSAB_L1), DEC);
          if( ps2x.Analog(PSAB_L1) == 255){
             armUp();
          }
     }

     if(ps2x.Button(PSB_L2)){
           Serial.print("L2 held this hard: ");
          Serial.println(ps2x.Analog(PSAB_L2), DEC);
          if( ps2x.Analog(PSAB_L2) == 255){
             armDown();
          }
     }
  
    
             //this will set the large motor vibrate speed based on 
                                              //how hard you press the blue (X) button    
    
    if (ps2x.NewButtonState())               //will be TRUE if any button changes state (on to off, or off to on)
    {
     
       
         
        if(ps2x.Button(PSB_L3))
         Serial.println("L3 pressed");
        if(ps2x.Button(PSB_R3))
         Serial.println("R3 pressed");
        if(ps2x.Button(PSB_L2))
         Serial.println("L2 pressed");
        if(ps2x.Button(PSB_R2))
         Serial.println("R2 pressed");
        if(ps2x.Button(PSB_GREEN))
          Serial.println("Triangle pressed");

      //  if( ps2x.Button(PSB_PAD_UP))  Serial.println("PAD UP pressed");  
         
    }   
         
    
    if(ps2x.ButtonPressed(PSB_RED))             //will be TRUE if button was JUST pressed
         Serial.println("Circle just pressed");
         
    if(ps2x.ButtonReleased(PSB_PINK)){             //will be TRUE if button was JUST released
         Serial.println("Square just released");
    }       
    
    if(ps2x.NewButtonState(PSB_BLUE))            //will be TRUE if button was JUST pressed OR released
         Serial.println("X just changed");  

    if(ps2x.ButtonReleased(PSB_PAD_UP) || ps2x.ButtonReleased(PSB_PAD_DOWN) || ps2x.ButtonReleased(PSB_PAD_LEFT) || ps2x.ButtonReleased(PSB_PAD_RIGHT) )             //will be TRUE if button was JUST released
         stopRun();            
    
    
    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) // print stick values if either is TRUE
    { /*
        Serial.print("Stick Values:");
        Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
        Serial.print(",");
        Serial.print(ps2x.Analog(PSS_LX), DEC); 
        Serial.print(",");
        Serial.print(ps2x.Analog(PSS_RY), DEC); 
        Serial.print(",");
        Serial.println(ps2x.Analog(PSS_RX), DEC); 
        */
    } 

     

      if( ps2x.Button(PSB_R1) )
      {
          if( (ps2x.Analog(PSAB_R1) == 255) ){
             startCutting();
          }
      }

       if( ps2x.Button(PSB_R2) )
      {
          if( (ps2x.Analog(PSAB_R2) == 255) ){
             stopCutting();
          }
      }

       if( ps2x.Button(PSB_GREEN) )
      {
          if( (ps2x.Analog(PSAB_GREEN) == 255) ){
                currentCuttingSpeed+=10;
                if(  currentCuttingSpeed >= MAX_CUTTING_SPEED ){
                     currentCuttingSpeed = MAX_CUTTING_SPEED;
                     vibrate = ps2x.Analog(PSAB_BLUE);
                }else{
                     vibrate = 0;  
                }
               // analogWrite(motor1En, currentCuttingSpeed);      
          }
      }
      if( ps2x.Button(PSB_BLUE) )
      {  
          vibrate = 0; 
          if( (ps2x.Analog(PSB_BLUE) == 255) ){
                currentCuttingSpeed-=10;
                if(  currentCuttingSpeed <= defaultCuttingSpeed )
                     currentCuttingSpeed = defaultCuttingSpeed;
               // analogWrite(motor1En, currentCuttingSpeed);      
          }
      }
    
 }
 
 
 delay(50);
     
}

void runBackward(){
       
       motorPWM =defaultSpeed ;
       analogWrite(R1PWM_Output, 0);
       analogWrite(R2PWM_Output, motorPWM);

       analogWrite(L1PWM_Output, 0);
       analogWrite(L2PWM_Output, motorPWM);
       
}
void  runForward() {
       
       motorPWM =defaultSpeed ;
       analogWrite(R2PWM_Output, 0);
       analogWrite(R1PWM_Output, motorPWM);

       analogWrite(L2PWM_Output, 0);
       analogWrite(L1PWM_Output, motorPWM);
       
}
void turnLeft(){

       motorPWM =defaultSpeed ;
       analogWrite(R1PWM_Output, 0);
       analogWrite(R2PWM_Output, motorPWM);

       analogWrite(L2PWM_Output, 0);
       analogWrite(L1PWM_Output, motorPWM+leftAdd);
  
}
void turnRight(){

       motorPWM =defaultSpeed ;
       analogWrite(R2PWM_Output, 0);
       analogWrite(R1PWM_Output, motorPWM);

       analogWrite(L1PWM_Output, 0);
       analogWrite(L2PWM_Output, motorPWM+leftAdd);
  
}
void stopRun(){
       motorPWM =0;
       analogWrite(R1PWM_Output, 0);
       analogWrite(R2PWM_Output, motorPWM);

       analogWrite(L1PWM_Output, 0);
       analogWrite(L2PWM_Output, motorPWM);
}
void armRight_to_Left(){
         EN_arm(true);
         digitalWrite( Axis1DirPin , LOW);
         digitalWrite( Axis2DirPin , HIGH);
         
         for(int x = 0; x < STEP; x++) {
           digitalWrite(Axis1StepPin,HIGH); 
           delayMicroseconds(DELAY_X); 
           digitalWrite(Axis1StepPin,LOW); 
           delayMicroseconds(DELAY_X); 

           digitalWrite(Axis2StepPin,HIGH); 
           delayMicroseconds(DELAY_X); 
           digitalWrite(Axis2StepPin,LOW); 
           delayMicroseconds(DELAY_X); 
         }
}
void armLeft_to_Right(){
  
         EN_arm(true);
         digitalWrite( Axis1DirPin , HIGH);
         digitalWrite( Axis2DirPin , LOW);
         
         for(int x = 0; x < STEP; x++) {
           digitalWrite(Axis1StepPin,HIGH); 
           delayMicroseconds(DELAY_X); 
           digitalWrite(Axis1StepPin,LOW); 
           delayMicroseconds(DELAY_X); 

           digitalWrite(Axis2StepPin,HIGH); 
           delayMicroseconds(DELAY_X); 
           digitalWrite(Axis2StepPin,LOW); 
           delayMicroseconds(DELAY_X); 
         }
}
void armUp(){
  
         digitalWrite(Axis3_EN , LOW);
         
         digitalWrite( Axis3DirPin , LOW);

     
         for(int x = 0; x < STEP; x++) {
           digitalWrite(Axis3StepPin,HIGH); 
           delayMicroseconds(DELAY_X); 
           digitalWrite(Axis3StepPin,LOW); 
           delayMicroseconds(DELAY_X); 
            
         }
}

void armDown(){
  
         digitalWrite(Axis3_EN , LOW);
         
         digitalWrite( Axis3DirPin , HIGH);

     
         for(int x = 0; x < STEP; x++) {
           digitalWrite(Axis3StepPin,HIGH); 
           delayMicroseconds(DELAY_X); 
           digitalWrite(Axis3StepPin,LOW); 
           delayMicroseconds(DELAY_X); 
            
         }
}
void stopCutting(){

    digitalWrite( relayControl , LOW);
  //analogWrite(motor1En, 0); //ENA pin
  
  //Controlling spin direction of motors:
 // digitalWrite(motor1Pin1, LOW);
 // digitalWrite(motor1Pin2, LOW);

}
void startCutting(){

   currentCuttingSpeed = defaultCuttingSpeed;

    digitalWrite( relayControl , HIGH);
  // analogWrite(motor1En, defaultCuttingSpeed); //ENA pin
  
  //Controlling spin direction of motors:
  //digitalWrite(motor1Pin1, HIGH);
  //digitalWrite(motor1Pin2, LOW);
}
void stopAll(){
  
     stopRun();
     stopCutting();
     
     digitalWrite(Axis1_EN , HIGH);
     digitalWrite(Axis2_EN , HIGH);
     digitalWrite(Axis3_EN , HIGH);
   
}
void EN_arm( bool en)
{
   digitalWrite(Axis1_EN , LOW);
   digitalWrite(Axis2_EN , LOW);
}
void EN_updown( bool en)
{
   digitalWrite(Axis3_EN , !en);
}
