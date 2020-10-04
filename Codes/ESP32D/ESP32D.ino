 #include <Wire.h>
 #include<esp_int_wdt.h>
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
  int v1, v2, v3, v4, v5 ,v6 ,v7 , v8 , v9 ,v10 , v11 , v12 ,v13 ,v14  ;
  float UP ;

  float PITCH , SETPITCH =0 , ER_PITCH , P_PITCH , I_PITCH , D_PITCH , ER_LAST_PITCH , PITCH_OUTPUT , gyc ,EROLL;
  float ROLL , SETROLL = 0 , ER_ROLL , P_ROLL , I_ROLL , D_ROLL , ER_LAST_ROLL , ROLL_OUTPUT , gxc , EPITCH;
  float YAW , SETYAW = 0 , ER_YAW , P_YAW , I_YAW , D_YAW , ER_LAST_YAW , YAW_OUTPUT , gzc ;
  float IMAX = 100 ;
  float   TEST , TEST2 = 0 , TEST3 , TEST4 , TEST5, TEST6 ,te ;

  double timex = 0 ;
  float P = 1.4 ;
  float I = 0.0 ;
  float D = 18 ;
  float PY = 3 ;
  float IY = 0.0 ;
  float DY = 0 ;

  void angles();

 #include <ESP32Servo.h> 
  Servo M1 ;
  Servo M2 ;
  Servo M3 ;
  Servo M4 ;
  int  m1 , m2 , m3 , m4 ;


#include <BlynkSimpleEsp32.h>
    char auth[] = "390928c0c76a4f29a14f5016fe638214";
    char ssid[] = "ANILMAITY";
    char pass[] = "ANIL1234";
  BLYNK_WRITE(V1) { v1 = param.asInt(); }
  BLYNK_WRITE(V2) { D = param.asInt(); }
  BLYNK_WRITE(V3) { P = param.asFloat(); }
  BLYNK_WRITE(V4) { I = param.asFloat(); }
  BLYNK_WRITE(V5) { v5 = param.asInt(); }
  BLYNK_WRITE(V6) { UP = param.asFloat(); }
//  BLYNK_WRITE(V7) {  v7 = param.asFloat();}
//  BLYNK_WRITE(V8) {  v8 = param.asFloat(); }
//  BLYNK_WRITE(V9) {  v9 = param.asFloat(); }
 // BLYNK_WRITE(V10){  v10 = param.asFloat(); }
 // BLYNK_WRITE(V11){  v11 = param.asFloat(); }
//  BLYNK_WRITE(V12){  TEST6 = param.asFloat(); }
//  BLYNK_WRITE(V13){  SETROLL = param.asFloat(); }
//  BLYNK_WRITE(V14){ SETPITCH = param.asFloat();  }
  

/*
  #include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
 */
//esp_err_t esp_task_wdt_reset(void); 
  TaskHandle_t Task1;
 int tes;
  void Task(void * parameter)
  { 

 // Serial.println("The device started, now you can pair it with bluetooth!");
    for(;;)
    {  // if (Serial.available()) {SerialBT.write(Serial.read());}
      //  if (SerialBT.available()) { Input = SerialBT.read();} //  Serial.write(Input); }
       // Serial.println(xPortGetCoreID());
  tes = pulseIn(25, HIGH, 25000);
   vTaskDelay(10);
    }
  }

  
void setup() {
  // put your setup code here, to run once:
 
  Serial.begin(115200);
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

  M1.setPeriodHertz(250);
  M2.setPeriodHertz(250);
  M3.setPeriodHertz(250);
  M4.setPeriodHertz(250);

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
  double roll  = atan2(accY, accZ) * RAD_TO_DEG;
  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  kalmanX.setAngle(roll); 
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;

    Blynk.begin(auth, ssid, pass);

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
      }

void loop() {
  
    Blynk.run();
    timex = micros();
         for (int d = 1 ; d <= 3 ; d++){ angles(); }

          pid();
          
          if (v5 > 300)
          { ESC(); }       
         else{ 
          I_PITCH = 0;
          I_ROLL = 0;
          YAW = 0 ;
          
        M1.writeMicroseconds(1000);
        M2.writeMicroseconds(1000);
        M3.writeMicroseconds(1000);
        M4.writeMicroseconds(1000); 
        } 
        
        passtime = (micros() - timex );
        while(passtime < 4000)
        {   passtime = (micros() - timex ); }

        
    // Serial.print(P_ROLL);Serial.print("\t"); // UP
    // Serial.print(I_ROLL);Serial.print("\t");
    // Serial.print(D_ROLL);Serial.print("\t"); // UP
    // Serial.print(ROLL_OUTPUT);Serial.print("\t"); // UP
    // Serial.println(passtime);Serial.print("\t"); // PID 

}


  
