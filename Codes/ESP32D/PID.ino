   void pid() 
 {
  //   UP = v6  ;
 // PY = v10 ;
 //  IY = v11 ;
 //  DY = v12 ;
 //  P  = v7  ;
 //  I  = v8  ;
 //  D  = v9  ;

  // H_ERROR = HEIGHT - H_SET;
  // if (H_ERROR < 150){  P_HEIGHT = P_H * H_ERROR;}
  // else{ P_HEIGHT = 0 ;}
  // HEIGHT_OUTPUT = P_HEIGHT ;
   
  ER_ROLL = ROLL - SETROLL;
  ER_PITCH = PITCH - SETPITCH;
  ER_YAW = YAW - SETYAW;
  
  
   P_ROLL  = P * ER_ROLL; 
   I_ROLL += (I* ER_ROLL);
   if(I_ROLL > IMAX ) {I_ROLL = IMAX;}
   else if (I_ROLL < - IMAX) {I_ROLL = -IMAX ;} 
   D_ROLL  = D * (ER_ROLL - ER_LAST_ROLL); 
  
  ROLL_OUTPUT = 0.5*(P_ROLL + I_ROLL + D_ROLL) + 0.5*ROLL_OUTPUT ; 
  
  ER_LAST_ROLL = ER_ROLL;
  
  
  
  
   P_YAW  = PY * ER_YAW; 
   I_YAW += IY * ER_YAW ;
   if(I_YAW > IMAX) {I_YAW = IMAX;}
   if (I_YAW < -1 * IMAX) {I_YAW = -1 * IMAX;} 
   D_YAW  = DY * (ER_YAW - ER_LAST_YAW); 
 
  
  YAW_OUTPUT = P_YAW + I_YAW + D_YAW ;
  
  ER_LAST_YAW = ER_YAW;
  
  
  
   
   P_PITCH  = P * ER_PITCH; 
   I_PITCH += I * ER_PITCH;
   if(I_PITCH > IMAX)  {I_PITCH = IMAX;}
   else if (I_PITCH < -IMAX ){I_PITCH = -IMAX;}
   D_PITCH  = D * (ER_PITCH - ER_LAST_PITCH); 
   
   PITCH_OUTPUT = 0.5*(P_PITCH + I_PITCH + D_PITCH) + 0.5*PITCH_OUTPUT ;
   
   
  ER_LAST_PITCH = ER_PITCH;
  

        /*            R
              -( )-   -( )-
               1 \     /2 
                  \ _ /     
                  |\ /|    P 
                  |/_\|
                  /   \
               4 /     \3
              -( )-   -( )-     
 */

 
    m1 =  1000 + UP - PITCH_OUTPUT + ROLL_OUTPUT + YAW_OUTPUT; //Calculate the pulse for esc 1 (front-right - CCW)4
    m2 =  1000 + UP - PITCH_OUTPUT - ROLL_OUTPUT - YAW_OUTPUT; //Calculate the pulse for esc 2 (rear-right - CW)4
    m3 =  1000 + UP + PITCH_OUTPUT - ROLL_OUTPUT + YAW_OUTPUT; //Calculate the pulse for esc 3 (rear-left - CCW)0
    m4 =  1000 + UP + PITCH_OUTPUT + ROLL_OUTPUT - YAW_OUTPUT; //Calculate the pulse for esc 4 (front-left - CW)5
 
  }
 
