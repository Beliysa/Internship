#include <MPU9250_asukiaaa.h>
#include "WiFi.h"

hw_timer_t * timer = NULL;       //Timer read object has 4 values. 
uint64_t timeRead = 0;            //timeRead() function returns uint64_t type. We call timeRead() with our object and return value is our time. 

MPU9250_asukiaaa mySensor;
const char* ssid = "Via";
const char* password =  "viA.Via_2018";



bool detectFallX (float aX, float avgaX){
  int diff;
  diff = aX - avgaX;
  if ( aX > 10 || aX < -10){
   return true;
  }
  else{ 
    return false;
  
  }
}
bool detectFallY (float aY, float avgaY){
  int diff;
  diff = aY - avgaY;
  if ( aY > 10  || aY < -10){
   return true; 
  }
  else{ 
   return false; 
  
  }
   
}
bool detectFallZ (float aZ, float avgaZ){
  int diff;
  diff = aZ - avgaZ;
  if ( aZ > 9.5  || aZ < -10.5){
   return true; 
  }
  else{
   return false; 
  
  }
}

WiFiServer server(80);
double cyclecount = 1;
double cycletime = 0;
double cycleV1 = 0;
double cycleV2 = 0;
double creset = 0;
int delvalue = 0;

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




void setup() {
  
  
  
  Serial.begin(500000);     //baud rate
   
  WiFi.mode(WIFI_STA);   
  WiFi.begin(ssid, password);
  Wire.begin();                                      
  mySensor.setWire(&Wire);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  
  mySensor.beginGyro(GYRO_FULL_SCALE_1000_DPS);    //GYRO setup, diğer değerlere kurulum için "asukiaaa.h"
  mySensor.beginAccel(ACC_FULL_SCALE_16_G);        //ACC setup, +-2g, +-4g, +-8g, +-16g (proje için +-16g seçildi, ani düşmeler 8g'yi aşıyor.)
  mySensor.beginMag();                             //MAGNETO setup, tek şekilde ölçüm yapıyor(mikroTesla).
  server.begin();
  
  timer = timerBegin(0, 1, true);     //prescaler. 80MHz internal timer(1 tick = 12.5ns*80 = 1000ns = 0.001ms).
}

void loop() {
  
  WiFiClient client = server.available();   // listen for incoming clients

   
  
  
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            
            creset = timerRead(timer);
            client.print("Device ready.");
            client.print("<br>");
            while(1){
            // the content of the HTTP response follows the header:
            
            
            /*
            if(cycletime<10000000){
              delvalue = (int)cycletime + 1;
              delay(10000000-delvalue);
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

            
            
            /*
            client.print(aX);
            client.print(",");
            client.print(aY);
            client.print(",");
            client.print(aZ);
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

             /*
              client.print("AvgX: ");
              client.print(avgaX);
              client.print("AvgY: ");
              client.print(avgaY);
              client.print("AvgZ: ");
              client.print(avgaZ);
              client.print("<br>");
            */
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
              
              if (flagX == true && flagY == false && flagZ == false){
                client.print("Fall detected on X axis.");
                client.print("<br>");
              }
              if (flagX == false && flagY == true && flagZ == false){
                client.print("Fall detected on Y axis.");
                client.print("<br>");
              }
              if (flagX == false && flagY == false && flagZ == true){
                client.println("Fall detected on Z axis.");
                client.print("<br>");
              }
               if (flagX == true && flagY == true && flagZ == false){
                client.print("Fall detected on X and Y axis.");
                client.print("<br>");
              }
              if (flagX == true && flagY == false && flagZ == true){
                client.print("Fall detected on X and Z axis.");
                client.print("<br>");
              }
              if (flagX == false && flagY == true && flagZ == true){
                client.print("Fall detected on Y and Z axis.");
                client.print("<br>");
              }
              if (flagX == true && flagY == true && flagZ == true){
                client.print("Fall detected on X,Y and Z axis.");
                client.print("<br>");
              }
              
              
              client.print("Standby");
              delay(500);
              client.print(".");
              delay(500);
              client.print(".");
              delay(500);
              client.print(".");
              client.print("<br>");
              
              flagX = false;
              flagY = false;
              flagZ = false;
              client.print("Device ready.");
              client.print("<br>");
            }
            
            
            
            //cycletime = (timerRead(timer)-creset)/cyclecount;
            //client.print(cycletime);
            //cyclecount++;
            


            
            /*           
            client.print("<b>");
            client.print("  ");
            
           
            client.print("</b>");
            client.print("<br>");
            */
            
            
            
            
                
            }
            
            
            

            
            
            
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
          if(!client.connected())
            break;
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }




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


//Virgül ile ayrılmış serial grafik için çıktı.
/*
Serial.print(gX);         //Gyro
Serial.print(",");
Serial.print(gY);
Serial.print(",");
Serial.print(gZ);


Serial.print(",");
*/



/*
Serial.print(aX);         //acceleration
Serial.print(",");
Serial.print(aY);
Serial.print(",");
Serial.println(aZ);
*/
/*
Serial.print(",");

Serial.print(mX);           //Magnet
Serial.print(",");
Serial.print(mY);
Serial.print(",");
Serial.println(mZ);
 */

  //client.stop();
   // Serial.println("Client Disconnected.");
}
}
