void ESC()
{
    if ((ROLL + PITCH) < 90 && (ROLL + PITCH) > -90 && (ROLL - PITCH) < 90 && (ROLL - PITCH) > -90 && TEST2 < 500  && ROLL < 90  )
 {

    M1.writeMicroseconds(m1);
    M3.writeMicroseconds(m3);
    M2.writeMicroseconds(m2);
    M4.writeMicroseconds(m4);
}
else 
{
        M1.writeMicroseconds(1000);
        M2.writeMicroseconds(1000);
        M3.writeMicroseconds(1000);
        M4.writeMicroseconds(1000);
        Serial.println("DRONE CRASH");
        TEST2 = 800;
  }
  
}

