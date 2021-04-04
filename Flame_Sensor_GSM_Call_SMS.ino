#include <EEPROM.h>
#include <SoftwareSerial.h> //Create software serial object to communicate with SIM800L
SoftwareSerial GSM(8, 9);   //SIM800L Tx & Rx is connected to Arduino #8 & #9

char phone_no[]="+923378655465"; //change +92 with country code and 3378655465 with phone number to sms

#define sensorPin A0 // choose the input pin (for Fire sensor)  

#define buzzer 2 // choose the pin for the Buzzer
#define led_M  3 // choose the pin for the Green LED Message active indication
#define led_C  4 // choose the pin for the Green LED Call Active indication
#define led_S  5 // choose the pin for the Red LED Fire detection indication

int read_value; // variable for reading the sensorpin status

int sms_Status, call_Status;
int flag=0;
int var_1=0, var_2=0;
char input_string[15]; // Will hold the incoming character from the GSM shield

void setup(){ // put your setup code here, to run once

Serial.begin(9600);//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)  
GSM.begin(9600);   //Begin serial communication with Arduino and SIM800L
  
pinMode(sensorPin, INPUT); // declare sensor as input

pinMode(buzzer,OUTPUT); // declare Buzzer as output 
pinMode(led_M,OUTPUT); // declare Green LED as output
pinMode(led_C,OUTPUT); // declare Green LED as output
pinMode(led_S,OUTPUT); // declare Red LED as output

Serial.println("Initializing....");
initModule("AT","OK",1000);                //Once the handshake test is successful, it will back to OK
initModule("ATE1","OK",1000);              //this command is used for enabling echo
initModule("AT+CPIN?","READY",1000);       //this command is used to check whether SIM card is inserted in GSM Module or not
initModule("AT+CMGF=1","OK",1000);         //Configuring TEXT mode
initModule("AT+CNMI=2,2,0,0,0","OK",1000); //Decides how newly arrived SMS messages should be handled  
Serial.println("Initialized Successfully"); 

sendSMS(phone_no,"Start GSM Fire Alert System");


sms_Status  = EEPROM.read(1);
call_Status = EEPROM.read(2);
}

void loop(){
readSMS();
  
read_value = digitalRead(sensorPin); // Digital input value

if(read_value==1){ //check if the Fire variable is High  
digitalWrite(buzzer, LOW); // Turn LED off.  
digitalWrite(led_S, LOW);  // Turn LED off.
flag=0;
}
else{ //check if the Fire variable is Low
digitalWrite(buzzer, HIGH); // Turn LED on.  
digitalWrite(led_S, HIGH);  // Turn LED on.
if(flag==0){flag=1;
 if(sms_Status==1){sendSMS(phone_no,"Fire is Detected Alert....!!!");}
 delay(1000);
 if(call_Status==1){callUp(phone_no);}  
}
delay(1000);
}

  if (var_1 == 1){
     if (!(strncmp(input_string, "Sms On", 6))){sms_Status=1; 
  EEPROM.write(1, sms_Status);
  sendSMS(phone_no,"Message is Active");
  }
else if (!(strncmp(input_string, "Sms Off", 7))){sms_Status=0;
  EEPROM.write(1, sms_Status);
  sendSMS(phone_no,"Message is Deactivate");
  }

else if (!(strncmp(input_string, "Call On", 7))){call_Status=1;
  EEPROM.write(2, call_Status);
  sendSMS(phone_no,"Call is Active");
  }
else if (!(strncmp(input_string, "Call Off", 8))){call_Status=0;
  EEPROM.write(2, call_Status);
  sendSMS(phone_no,"Call is Deactivate");
  }  
  
    var_1 = 0;
    var_2 = 0;
  }

digitalWrite(led_M, sms_Status);  //LED On SMS Active,  LED Off SMS Deactivate
digitalWrite(led_C, call_Status); //LED On Call Active, LED Off Call Deactivate

}

void sendSMS(char *number, char *msg){
GSM.print("AT+CMGS=\"");GSM.print(number);GSM.println("\"\r\n"); //AT+CMGS=”Mobile Number” <ENTER> - Assigning recipient’s mobile number
delay(500);
GSM.println(msg); // Message contents
delay(500);
GSM.write(byte(26)); //Ctrl+Z  send message command (26 in decimal).
delay(5000);  
}

void callUp(char *number){
GSM.print("ATD + "); GSM.print(number); GSM.println(";"); //Call to the specific number, ends with semi-colon,replace X with mobile number
delay(1000);       // wait for 1 seconds...
}


void readSMS(){
  while(GSM.available()>0){
    //--------- PIN HERE -------//
    if (GSM.find("/786")) // <<< '/786' where786 is 3 digit PIN.
      //--------- PIN HERE -------//
    {
      delay(1000);
      while (GSM.available())
      {
        char input_char = GSM.read();
        input_string[var_2++] = input_char;
        if (input_char == '/')
        {
          var_1 = 1;
          return;
        }
      }
    }
 }
}


void initModule(String cmd, char *res, int t){
while(1){
    Serial.println(cmd);
    GSM.println(cmd);
    delay(100);
    while(GSM.available()>0){
       if(GSM.find(res)){
        Serial.println(res);
        delay(t);
        return;
       }else{Serial.println("Error");}}
    delay(t);
  }
}
