void angles() {
  /* Update all the values */
  while (i2cRead(0x3B, i2cData, 14));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
  tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]);
  gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
  gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
  gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);;

  dt = (double)(micros() - timer) / 1000000; // Calculate delta time
  timer = micros();


#ifdef RESTRICT_PITCH // Eq. 25 and 26
   roll  = atan2(accY, accZ) * RAD_TO_DEG  + 0.5;
  pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG + 2.80 ;
  
#else // Eq. 28 and 29
   roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG + 0.5;
   pitch = atan2(-accX, accZ) * RAD_TO_DEG   + 2.80 ;
#endif

   gyroXrate = 0.5*(gyroX - gxc) / 131.0 + 0.5*gyroXrate ; // Convert to deg/s
   gyroYrate = 0.5*(gyroY - gyc) / 131.0 + 0.5*gyroYrate ; // Convert to deg/s
   
   
#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
    kalmanX.setAngle(roll);
    compAngleX = roll;
    kalAngleX = roll;
    gyroXangle = roll;
  } else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
#else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
   
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
  } else
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY) > 90)
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
#endif

  gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
  gyroYangle += gyroYrate * dt;
  
  //gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
  //gyroYangle += kalmanY.getRate() * dt;

   
  compAngleX += gyroXrate * dt ; // Calculate the angle using a Complimentary filter
  compAngleY += gyroYrate * dt ;
  
  compAngleY -=  compAngleX*sin(((gyroZ-gzc)*dt)*0.0001332);  
  compAngleX +=  compAngleY*sin(((gyroZ-gzc)*dt)*0.0001332);                 
 

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;

  
   roll = 0.5*EROLL + 0.5*roll;      EROLL = roll ; 
  pitch = 0.5*EPITCH + 0.5*pitch;   EPITCH = pitch ;

  
  compAngleX = 0.9999*compAngleX +  0.0001* (roll) ;  
  compAngleY = 0.99999*compAngleY + 0.00001 * (pitch) ; 

  ROLL  =  0.5*ROLL + 0.5*(compAngleX) ;
  PITCH =  0.5*PITCH + 0.5*(compAngleY) ;
  
  erroryaw = 0.5*(gyroZ - gzc )/131 + 0.5*erroryaw ;
  YAW +=  ( erroryaw * dt);

 // Serial.print(ROLL);Serial.print("\t");
 // Serial.println(YAW);Serial.print("\t"); 

}
