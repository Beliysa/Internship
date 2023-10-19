#include <MPU9250_asukiaaa.h>
#include <MadgwickAHRS.h>



hw_timer_t * timer = NULL;       //Timer read object has 4 values. 
uint64_t timeRead = 0;            //timeRead() function returns uint64_t type. We call timeRead() with our object and return value is our time.

Madgwick filter;
MPU9250_asukiaaa mySensor;

float gX, gY, gZ;            //Gyro X, Y, Z
float aX, aY, aZ;            //Acceleration X, Y, Z
float mX, mY, mZ;            //Magneto X, Y, Z
char buffer[30];             //output format için string

double cyclecount = 1;
double cycletime = 0;
double cycleV1 = 0;
double cycleV2 = 0;
double creset = 0;
int delvalue = 0;

void setup() {
  
  Serial.begin(500000);                             //baud rate   
  Wire.begin();                                      
  mySensor.setWire(&Wire);
  mySensor.beginGyro(GYRO_FULL_SCALE_1000_DPS);    //GYRO setup, diğer değerlere kurulum için "asukiaaa.h"
  mySensor.beginAccel(ACC_FULL_SCALE_16_G);        //ACC setup, +-2g, +-4g, +-8g, +-16g (proje için +-16g seçildi, ani düşmeler 8g'yi aşıyor.)
  mySensor.beginMag();                             //MAGNETO setup, tek şekilde ölçüm yapıyor(mikroTesla).
  timer = timerBegin(0, 80, true);     //prescaler. 80MHz internal timer(1 tick = 12.5ns*80 = 0,001ms).
  /*
  mySensor.magXOffset = 2;                         //Asukiaaa kütüphanesi magnetometer calibration değerleri. 
  mySensor.magYOffset = -45;
  mySensor.magZOffset = 63;
  */ 
}

void loop() {
 
  /*
  if(cycletime<3){
              delvalue = (int)cycletime + 1;
              delay(3-delvalue);
  }
  */
  mySensor.gyroUpdate();      //LOOP Gyrometer Update    
  gX = mySensor.gyroX();
  gY = mySensor.gyroY();
  gZ = mySensor.gyroZ();

  mySensor.accelUpdate();     //LOOP Acceleration Update
  aX = mySensor.accelX();
  aY = mySensor.accelY();
  aZ = mySensor.accelZ();

  mySensor.magUpdate();        //LOOP Magnetometer Update
  mX = mySensor.magX();
  mY = mySensor.magY();
  mZ = mySensor.magZ();

  Serial.print(gX);         //Gyro
  Serial.print(",");
  Serial.print(gY);
  Serial.print(",");
  Serial.print(gZ);
  Serial.print("      after madge: ");
  
  
  filter.update(gX,gY,gZ,aX,aY,aZ,mX,mY,mZ);

  Serial.print(gX);         //Gyro
  Serial.print(",");
  Serial.print(gY);
  Serial.print(",");
  Serial.println(gZ);
 


  
  delay(100);
 
//Okunabilirlik için formatlı print. Saniyede 2 veri gönderiyor. (delay(500)).
/*
//GYRO PRİNT
sprintf (buffer, "gX = %7.2f    ",gX);  
Serial.print (buffer);
sprintf (buffer, "gY = %7.2f    ",gY); 
Serial.print (buffer);
sprintf (buffer, "gZ = %7.2f",gZ);
Serial.println (buffer);


//ACCEL PRINT
sprintf (buffer, "aX = %7.2f    ",aX);  
Serial.print (buffer);
sprintf (buffer, "aY = %7.2f    ",aY); 
Serial.print (buffer);
sprintf (buffer, "aZ = %7.2f",aZ);
Serial.println (buffer);


//MAG PRINT
sprintf (buffer, "mX = %7.2f    ",mX);  
Serial.print (buffer);
sprintf (buffer, "mY = %7.2f    ",mY); 
Serial.print (buffer);
sprintf (buffer, "mZ = %7.2f",mZ);
Serial.println (buffer);


Serial.println ("----------------");    // 3 Line'da bir çizgi
*/



/*
//Virgül ile ayrılmış serial grafik için çıktı.

Serial.print(gX);         //Gyro
Serial.print(",");
Serial.print(gY);
Serial.print(",");
Serial.print(gZ);


Serial.print(",");

Serial.print(aX);         //acceleration
Serial.print(",");
Serial.print(aY);
Serial.print(",");
Serial.print(aZ);


Serial.print(",");

Serial.print(mX);           //Magnet
Serial.print(",");
Serial.print(mY);
Serial.print(",");
Serial.println(mZ);

*/

Serial.print("     CTime:");

cycletime = (timerRead(timer)-creset)/cyclecount;
Serial.print(cycletime);
Serial.println("ms");
++cyclecount;



}
