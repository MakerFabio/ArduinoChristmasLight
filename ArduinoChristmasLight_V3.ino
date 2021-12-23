// Christmas Light V3.0 -  DIY Arduino Christmaas light "Maker Fabio" project on Youtube
// Simple 4 channels Christmas light Sketch
// it starts automatically with Lights ON when powered or reset
// it will keep light on for "TIME_KEEP_ON_HR" hours, and then remains off for
// the remaining part of the day (24-TIME_KEEP_ON_HR hours)

// LED output PWMs
#define LED1_PIN 6
#define LED2_PIN 9
#define LED3_PIN 10
#define LED4_PIN 11

#define CH_NUMBER 4 // number of LED strings, in my case I have 4 LED strings
#define FULL_FADETIME 400// [ms]
#define FADE_PERIOD 10//[ms] dupdate rate of duty cycles during fading

#define TIME_KEEP_ON_HR   5
  
#define TIME_OUT_OFF  (unsigned long)1000*60*60*TIME_KEEP_ON_HR  //[ms] standby time for Module Off
#define HOURS24 (unsigned long)1000*60*60*24

extern volatile unsigned long timer0_millis;

void setup() {
  // set PIN as in/out
  pinMode(LED1_PIN,OUTPUT);
  pinMode(LED2_PIN,OUTPUT);
  pinMode(LED3_PIN,OUTPUT);
  pinMode(LED4_PIN,OUTPUT);

  // start with LED off
  digitalWrite(LED1_PIN, HIGH); 
  digitalWrite(LED2_PIN, HIGH); 
  digitalWrite(LED3_PIN, HIGH); 
  digitalWrite(LED4_PIN, HIGH); 

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // turn off LED to lower power consumption
  
  Serial.begin(230400);       // Hardware serial for MCLD reading only 

}

void loop() {
  static int invBrightness=0;
  static int fadeAmount =5;

  
  
  if (millis() < TIME_OUT_OFF )
  {
     Serial.print("millis=");
     Serial.println(millis());
     Serial.print("TIME_OUT_OFF=");
     Serial.println(TIME_OUT_OFF);
  }
  else 
  {  
     Serial.print("millis=");
     Serial.println(millis());
     Serial.print("TIME_OUT_OFF EXPIRED ******* =");
     Serial.println(TIME_OUT_OFF);
  }
  


  if(timerStillOn()){ // perform light games only when timer is not expired
    
    // light game 1 , random perform some times
    for(int i = 0; i<10;i++)
    {
      // fadeToScenario( scenario, fadeTime, targetMin, targetMax );
      fadeToScenario(random (0,0b00001111), random (200,500),random (1,10),random (150,255) );
    }
  
    // light game 2 ,perform some times
    for(int i = 0; i<3;i++)
    {
      fadeLED(LED1_PIN);
      fadeLED(LED2_PIN);
      fadeLED(LED3_PIN);
      fadeLED(LED4_PIN);
    }
  }
  else{
        //  light off
  digitalWrite(LED1_PIN, HIGH); 
  digitalWrite(LED2_PIN, HIGH); 
  digitalWrite(LED3_PIN, HIGH); 
  digitalWrite(LED4_PIN, HIGH); 
  }
}

// update only the duty cyle of the channels passed in the chParalMask, set other channels duty to 0
// "duty" value expressed as %oo so 100% = 10000
// fade time [ms] is the time needed to go from 0% to fullduty
// chMask is a CH_NUMBER bit mask, that will be faded to 100% duty
void fadeToScenario(int chMask, int fadeTimeMs , int targetMin, int targetMax )
{
  int singleMask=0x01;     // one bit mask 
  static int dutyMemory[CH_NUMBER];

  int fadeCycles;
  int fadeDimStep; // ho
  
  fadeCycles=fadeTimeMs/FADE_PERIOD;

  fadeDimStep = 255 / (FULL_FADETIME/FADE_PERIOD);
  
  for (int x= 0; x<fadeCycles ; x++)
  {
    for (int i = 0 ; i<CH_NUMBER; i++){ //for each channel
       if ( chMask & (singleMask << i)){ //if channel is ON, increase its duty to the max 
         if (dutyMemory[i]<targetMax-fadeDimStep )
           dutyMemory[i] = dutyMemory[i]+fadeDimStep;
         else 
           dutyMemory[i] = targetMax;
       }
       else{  // function should is off in the new fadeScenario request
         if (dutyMemory[i]> targetMin+fadeDimStep )
           dutyMemory[i] = dutyMemory[i]-fadeDimStep;
         else 
           dutyMemory[i] = targetMin;
       }
    }
    analogWrite(LED1_PIN, dutyMemory[0] );
    analogWrite(LED2_PIN, dutyMemory[1] );
    analogWrite(LED3_PIN, dutyMemory[2] );
    analogWrite(LED4_PIN, dutyMemory[3] );
    delay(FADE_PERIOD);
  }
}


// Fades the LED from full off to full on 
void fadeLED(int pin) {
  static int invBrightness=0;

 // start with light off (remember OUT 0 => light on)
  for (invBrightness=255; invBrightness>=0 ; invBrightness--)
  {
    analogWrite(pin, invBrightness);
    invBrightness = invBrightness -1;
    delayMicroseconds (2000);
  }
  
  digitalWrite(pin, LOW); // be sure to have 100% duty without glitches
  delay (20); 

  for (invBrightness=0; invBrightness<=255 ; invBrightness++)
  {
    analogWrite(pin, invBrightness);
    invBrightness = invBrightness +1;
    delayMicroseconds (2000);
  }


  //digitalWrite(pin, HIGH); // be sure to have 100% duty without glitches
}

// escAutoPowerOff: check if auto power off is needed becasue of: 
// -no activity from the remote 
// -low battery : smaller than BATT_LOW
bool timerStillOn()
{
  if (millis() < TIME_OUT_OFF )
    return true;
  else 
  {  
     if (millis() > HOURS24)
       timer0_millis = 0;
     return false;  
  }
}
