#include <MPU9250_asukiaaa.h>
#include <SimpleKalmanFilter.h>
#include <Tone32.h>

#define RED1 18
#define RED2 5
#define GREEN 4
#define BUZZER 17

MPU9250_asukiaaa mySensor;
SimpleKalmanFilter gX_kalman(5, 5, 0.005);          //Kalman nesneleri,
SimpleKalmanFilter gY_kalman(5, 5, 0.005);          //ilk değer ölçümün oynama tahmini
SimpleKalmanFilter gZ_kalman(5, 5, 0.005);          //ikinci değer başlangıç olarak veriliyor ve 1. değerle aynı
                                                    //üçüncü değer 0.001 ile 1 arasında ölçüm hızı
                                                    //(1Hz = 1, 100Hz = 0.01)
SimpleKalmanFilter aX_kalman(0.01, 0.01, 0.005);      
SimpleKalmanFilter aY_kalman(0.01, 0.01, 0.005);
SimpleKalmanFilter aZ_kalman(0.01, 0.01, 0.005);

SimpleKalmanFilter mX_kalman(1, 1, 0.005);
SimpleKalmanFilter mY_kalman(1, 1, 0.005);
SimpleKalmanFilter mZ_kalman(1, 1, 0.005);


bool detectFallX (float aX, float avgaX){
  int diff;
  diff = aX - avgaX;
  if ( diff > 4 || diff < -4){
   return true;
  }
  else{ 
    return false;
  
  }
}
bool detectFallY (float aY, float avgaY){
  int diff;
  diff = aY - avgaY;
  if ( diff > 4  || diff < -4){
   return true; 
  }
  else{ 
   return false; 
  
  }
   
}
bool detectFallZ (float aZ, float avgaZ){
  int diff;
  diff = aZ - avgaZ;
  if ( diff > 4  || diff < -4){
   return true; 
  }
  else{
   return false; 
  
  }
}

hw_timer_t * timer = NULL;       //Timer read object has 4 values. 
uint64_t timeRead = 0;           //timeRead() function returns uint64_t type.
                                 //We call timeRead() with our object and return value is our time.


float aXarr[25] = {0};
float aYarr[25] = {0};
float aZarr[25] = {0};
float sumaX = 0;
float sumaY = 0;
float sumaZ = 0;
float avgaX = 0;
float avgaY = 0;
float avgaZ = -1;
int i = 0;

bool flagX = false;
bool flagY = false;
bool flagZ = false;

float gX, gY, gZ;            //Gyro X, Y, Z
float aX, aY, aZ;            //Acceleration X, Y, Z
float mX, mY, mZ;            //Magneto X, Y, Z
char buffer[30];             //output format için string

double cycletime = 0;
double cycletimems = 0;
double cyclestarttime = 0;
double cycleendtime = 0;

void setup() {


  pinMode(RED1, OUTPUT);   //RED
  pinMode(RED2, OUTPUT);   //RED2
  pinMode(GREEN, OUTPUT);  //GREEN
  
  Serial.begin(500000);                             //baud rate   
  Wire.begin();                                      
  mySensor.setWire(&Wire);
  mySensor.beginGyro(GYRO_FULL_SCALE_2000_DPS);    //GYRO setup, diğer değerlere kurulum için "asukiaaa.h"
  mySensor.beginAccel(ACC_FULL_SCALE_16_G);        //ACC setup, +-2g, +-4g, +-8g, +-16g (proje için +-16g seçildi, ani düşmeler 8g'yi aşıyor.)
  mySensor.beginMag();                             //MAGNETO setup, tek şekilde ölçüm yapıyor(mikroTesla).
  timer = timerBegin(0, 80, true);     //prescaler. 80MHz internal timer(1 tick = 12.5ns*80000 = 1ms).
  digitalWrite(GREEN, HIGH);
  digitalWrite(RED1, LOW);
  digitalWrite(RED2, LOW);
  /*
  mySensor.magXOffset = 2;                         //Asukiaaa kütüphanesi magnetometer calibration değerleri. 
  mySensor.magYOffset = -45;
  mySensor.magZOffset = 63;
  */ 
}

void loop() {
 
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


  
  
  gX = gX_kalman.updateEstimate(gX);
  gY = gY_kalman.updateEstimate(gY);
  gZ = gZ_kalman.updateEstimate(gZ);

  aX = aX_kalman.updateEstimate(aX);
  aY = aY_kalman.updateEstimate(aY);
  aZ = aZ_kalman.updateEstimate(aZ);

  mX = mX_kalman.updateEstimate(mX);
  mY = mY_kalman.updateEstimate(mY);
  mZ = mZ_kalman.updateEstimate(mZ);

  /*
  gX = gX - 0.98;           //GYRO OFFSET
  gY = gY + 1.69;
  gZ = gZ + 0.92;


  if (-0.50 < gX && gX < +0.50){
    gX = 0;
  }

  if (-0.50 < gY && gY < +0.50){
    gY = 0;
  }

  if (-0.50 < gZ && gZ < +0.50){
    gZ = 0;
  }

  aY = aY + 0.01;            // ACC OFFSET
  aZ = aZ - 0.02;

  if (-0.01 < aX && aX < +0.01){
    aX = 0;
  }
  
  if (-0.01 < aY && aY < +0.01){
    aY = 0;
  }

  if (-0.01 < aZ && aZ < +0.01){
    aZ = 0;
  }

  */
  
     aXarr[i] = aX; 
     aYarr[i] = aY;
     aZarr[i] = aZ;
     i++;
            if (i == 25){
              i = 0;
              for ( int j = 0 ; j<25 ; j++){
                sumaX = sumaX + aXarr[j];
                sumaY = sumaY + aYarr[j];
                sumaZ = sumaZ + aZarr[j];
                }
              avgaX = sumaX/25;
              avgaY = sumaY/25;
              avgaZ = sumaZ/25;
              sumaX = 0;
              sumaY = 0;
              sumaZ = 0;
            }

          if (detectFallX(aX,avgaX)) {
             
              flagX = true;
              
              for ( int j = 0 ; j<25 ; j++){
                aXarr[j] = 0;
                
                }
                aX=0;
                
            }
            if (detectFallY(aY,avgaY)) {
               
               flagY = true;
               
               for ( int j = 0 ; j<25 ; j++){
                aYarr[j] = 0;
                
                }
                aY=0;
            }
            if (detectFallZ(aZ,avgaZ)) {
              
              flagZ = true;
              
              for ( int j = 0 ; j<25 ; j++){
                aZarr[j] = 0;
                
                }
                aZ=0;
                
            }
            
            if(flagX == true || flagY == true || flagZ == true){
              
              
              
              digitalWrite(GREEN, LOW);
              digitalWrite(RED1, HIGH); 
              digitalWrite(RED2, HIGH);
              pinMode(BUZZER,OUTPUT);      //Buzzer
              digitalWrite(BUZZER, HIGH);
              flagX = false;
              flagY = false;
              flagZ = false;
              
            }

  /*
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
/*
Serial.print("     CTime:");

cycletime = (timerRead(timer)-creset)/cyclecount;
Serial.print(cycletime);
Serial.println("ms");
++cyclecount;
*/ 


}
