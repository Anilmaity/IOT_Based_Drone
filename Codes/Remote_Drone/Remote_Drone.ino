 #include <Wire.h>
 
  #include <Kalman.h> 
  //#define RESTRICT_PITCH // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf
  Kalman kalmanX; // Create the Kalman instances
  Kalman kalmanY;

/* IMU Data */
  
  double accX, accY, accZ , loopt , roll , pitch ;
  double gyroX, gyroY, gyroZ;
  int16_t tempRaw;
  double gyroYrate , gyroXrate ;

  double gyroXangle, gyroYangle; // Angle calculate using the gyro only
  double compAngleX, compAngleY; // Calculated angle using a complementary filter
  double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter

  uint32_t timer;
  uint8_t i2cData[14]; // Buffer for I2C data
   double dt, passtime  , batt  , erroryaw; 
 // int v1, v2, v3, v4, v5 ,v6 ,v7 , v8 , v9 ,v10 , v11 , v12 ,v13 ,v14  ;
  float UP ;

  float PITCH , SETPITCH =0 , ER_PITCH , P_PITCH , I_PITCH , D_PITCH , ER_LAST_PITCH , PITCH_OUTPUT , gyc ,EROLL;
  float ROLL , SETROLL = 0 , ER_ROLL , P_ROLL , I_ROLL , D_ROLL , ER_LAST_ROLL , ROLL_OUTPUT , gxc , EPITCH;
  float YAW , SETYAW = 0 , ER_YAW , P_YAW , I_YAW , D_YAW , ER_LAST_YAW , YAW_OUTPUT , gzc ;
  float IMAX = 60 ;
  float   TEST , TEST2 = 0 , TEST3 , TEST4 , TEST5, TEST6 ,te ;

  double timex = 0 ;
  float PR = 4.5 ; // 1.6
  double IR = 0.01 ; // 0.0001
  float DR = 700 ;  // 250S

  float PP = 4 ; // 1.6
  double IP = 0.01 ; // 0.0001
  float DP = 700 ;  // 250

  
  float PY = 2.5 ;
  float IY = 0.0 ;
  float DY = 500 ;

  void angles();

 #include <ESP32Servo.h> 
  Servo M1 ;
  Servo M2 ;
  Servo M3 ;
  Servo M4 ;
  int  m1 , m2 , m3 , m4 ;
/*
#include <BlynkSimpleEsp32.h>
    char auth[] = "390928c0c76a4f29a14f5016fe638214";
    char ssid[] = "ANILMAITY";
    char pass[] = "ANIL1234";
    float v1 , v5 ;
  BLYNK_WRITE(V1) { v1 = param.asInt(); }
  BLYNK_WRITE(V2) { D = param.asInt(); }
  BLYNK_WRITE(V3) { P = param.asFloat(); }
  BLYNK_WRITE(V4) { I = param.asDouble(); }
  BLYNK_WRITE(V5) { v5 = param.asInt(); }
  BLYNK_WRITE(V6) { UP = param.asFloat(); }
//  BLYNK_WRITE(V7) {  v7 = param.asFloat();}
//  BLYNK_WRITE(V8) {  v8 = param.asFloat(); }
//  BLYNK_WRITE(V9) {  v9 = param.asFloat(); }
 // BLYNK_WRITE(V10){  v10 = param.asFloat(); }
 // BLYNK_WRITE(V11){  v11 = param.asFloat(); }
//  BLYNK_WRITE(V12){  TEST6 = param.asFloat(); }
  BLYNK_WRITE(V13){  SETROLL = param.asFloat(); }
  BLYNK_WRITE(V14){ SETPITCH = param.asFloat();  }

*/
  TaskHandle_t Task1;
 //External Interrupts
volatile int cha1 , cha2 , cha3 , cha4 , cha5 , cha6;
volatile int tempcha1 , tempcha2 , tempcha3 , tempcha4 , tempcha5 , tempcha6;

volatile int CT1 , CT2 ,CT3 , CT4 , CT5 , CT6 ;
int TTK ,TK, MODE ;
  void Task(void * parameter)
  { 
  pinMode(32 , INPUT); 
  pinMode(35 , INPUT);
  pinMode(33 , INPUT);
  pinMode(26 , INPUT);
  pinMode(27 , INPUT);
  pinMode(25 , INPUT);
  
  attachInterrupt(35, ch1, RISING);
  attachInterrupt(32, ch2, RISING);
  attachInterrupt(33, ch3, RISING);
  attachInterrupt(25, ch4, RISING);
  attachInterrupt(26, ch5, RISING);
  attachInterrupt(27, ch6, RISING);

  for(;;){
    if (cha3 > 1000 && cha3 < 2000 ){ UP = (int)(0.9*UP + 0.1*(cha3 - 1100)) ;}
    if (cha2 > 1000 && cha2 < 2000 ){ SETPITCH = int(map(cha2 , 1000 , 2000 , -30 , 32));}
    if (cha1 > 1000 && cha1 < 2000 ){   SETROLL = int(map(cha1 , 1000 , 2100 , -30 , 40));}
     if (cha4 > 1000 && cha1 < 2000 ){   SETYAW += (0.001*(cha4-1500));}
       if (cha5 > 1400 && cha5  < 1700 ){ MODE = 2 ;}
   if (cha5 > 1700 && cha5  < 2200 ){ MODE = 1 ;}
   else {MODE = 0;}
    
 vTaskDelay(10);}
}
  
  
  
void setup() {
  // put your setup code here, to run once:
 
  Serial.begin(250000);
  delay(10);
    pinMode(2, OUTPUT);
   digitalWrite(2, HIGH); 
   delay(100);
   digitalWrite(2, LOW);
   delay(100);
      digitalWrite(2, HIGH);
    
   // SerialBT.begin("ESP32test"); //Bluetooth device name
  xTaskCreatePinnedToCore(
      Task,
      
      "T1",
      1000,  //STACK
      NULL,
      1,    // PRIORITY
      &Task1,
      0 ); //core
     
//-----------------------------------------------------------



pinMode(25, INPUT);
pinMode(26, INPUT);
pinMode(27, INPUT);
pinMode(32, INPUT);
pinMode(33, INPUT);
pinMode(35, INPUT);


  M1.setPeriodHertz(400);
  M2.setPeriodHertz(400);
  M3.setPeriodHertz(400);
  M4.setPeriodHertz(400);

  M1.attach(19); // v1
  M2.attach(18); // v2
  M3.attach(13); // v3
  M4.attach(12); // v4

  M1.writeMicroseconds(1000);
  M2.writeMicroseconds(1000);
  M3.writeMicroseconds(1000);
  M4.writeMicroseconds(1000);

//---------------------------------------------------------
  Wire.begin();
  #if ARDUINO >= 157
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz
  #else
  TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
  #endif

  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
  while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode

  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
    Serial.print(F("Error reading sensor"));
    loopt = micros();
    while (1);
  }

  delay(100); // Wait for sensor to stabilize

  /* Set kalman and gyro starting angle */
  while (i2cRead(0x3B, i2cData, 6));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);

#ifdef RESTRICT_PITCH // Eq. 25 and 26
  double roll  = atan2(accY, accZ) * RAD_TO_DEG + 0.50 ;
  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG + 2.80;
#else // Eq. 28 and 29
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG  + 0.50;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG + 2.80;
#endif
  kalmanX.setAngle(roll); 
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;

 // Blynk.begin(auth, ssid, pass);

   for (int c = 1 ; c <= 2000 ; c++)
    {
        while (i2cRead(0x3B, i2cData, 14));
    accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
    accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
    accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
    tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]);
    gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
    gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
    gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);;
       gzc +=  gyroZ; gxc +=  gyroX ;  gyc += gyroY ;
    } 
      gzc /= 2000; gxc /= 2000;  gyc /= 2000;
         digitalWrite(2, LOW);
      }

void loop() {
  
 //   Blynk.run();
    timex = micros();
         for (int d = 1 ; d <= 2 ; d++){ angles(); }

          pid();
          
          if (MODE == 1 )
          { 
            safe();
            ESC(); 
            digitalWrite(2, HIGH); 
 
             }
                   
         else  
          { I_PITCH = 0;
          I_ROLL = 0;
          YAW = 0 ;
          SETYAW = 0;
          digitalWrite(2, LOW);  
    // Serial.print(cha1);Serial.print("\t");
    // Serial.print(cha2);Serial.print("\t"); // UP 
    //Serial.print(UP);Serial.print("\t"); 
     //Serial.print((0.001*(cha4-1500)));Serial.print("\t"); // UP
     //Serial.print(MODE);Serial.print("\t"); 
     Serial.print(YAW);Serial.print("\t"); // UP// UP
     Serial.print(ROLL);Serial.print("\t"); // UP// UP
     Serial.print(PITCH);Serial.print("\t"); // UP// UP
     Serial.print(SETROLL);Serial.print("\t"); // 
     Serial.print(SETPITCH);Serial.print("\t"); // PID 
     Serial.println(SETYAW);Serial.print("\t"); 
          
        M1.writeMicroseconds(1000);
        M2.writeMicroseconds(1000);
        M3.writeMicroseconds(1000);
        M4.writeMicroseconds(1000); 
        } 
        
        passtime = (micros() - timex );
        while(passtime < 2600)
        {   passtime = (micros() - timex ); }

       

 

}


  
