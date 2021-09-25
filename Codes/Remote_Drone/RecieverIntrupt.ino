
void ch1() { attachInterrupt(35, c1, FALLING); CT1 = micros();} 
void c1() {
  attachInterrupt(35, ch1, RISING);
  tempcha1 = micros()-CT1;  
  if (tempcha1 > 900 && tempcha1 < 2100 ) {cha1 = tempcha1 ;}
  else {cha1 = cha1 ; }}


void ch2() { attachInterrupt(32, c2, FALLING); CT2 = micros();} 
void c2() {
  attachInterrupt(32, ch2, RISING); 
  tempcha2 = micros()-CT2;  
  if (tempcha2 > 900 && tempcha2 < 2100 ) {cha2 = tempcha2 ;}
  else {cha2 = cha2 ; }}


void ch3() { attachInterrupt(33, c3, FALLING); CT3 = micros();} 
void c3() {
  attachInterrupt(33, ch3, RISING);
  tempcha3 = micros()-CT3;
  if (tempcha3 > 900 && tempcha3 < 2100 ) {cha3 = tempcha3 ;}
  else {cha3 = cha3 ; }}


void ch4() { attachInterrupt(25, c4, FALLING); CT4 = micros();} 
void c4() {
  attachInterrupt(25, ch4, RISING);
  tempcha4 = micros()-CT4;
   if (tempcha4 > 900 && tempcha4 < 2100 ) {cha4 = tempcha4 ;}
  else {cha4 = cha4 ; }}

void ch5() { attachInterrupt(26, c5, FALLING); CT5 = micros();} 
void c5() { 
  attachInterrupt(26, ch5, RISING); 
  tempcha5 = micros()-CT5;
   if (tempcha5 > 900 && tempcha5 < 2100 ) {cha5 = tempcha5 ;}
  else {cha5 = cha5 ; }
/*  TTK = micros() - TK;
  TK = micros();
  Serial.print(TTK/1000);Serial.print("\t");
  Serial.println(xPortGetCoreID());Serial.print("\t");
  */}

void ch6() { attachInterrupt(27, c6, FALLING); CT6 = micros();} 
void c6() {
  attachInterrupt(27, ch6, RISING); 
  tempcha6 = micros()-CT6;
   if (tempcha6 > 900 && tempcha6 < 2100 ) {cha6 = tempcha6 ;}
  else {cha6 = cha6 ; }}


