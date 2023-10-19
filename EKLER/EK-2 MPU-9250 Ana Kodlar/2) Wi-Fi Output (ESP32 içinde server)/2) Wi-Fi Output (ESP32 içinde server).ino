#include <MPU9250_asukiaaa.h>
#include "WiFi.h"


MPU9250_asukiaaa mySensor;
const char* ssid = "*****";             //Bu değerler şirketin kendi Wi-Fi ağına ait özel bilgilerdir. Kendi ağ verileriniz ile değiştiriniz.
const char* password =  "******";
WiFiServer server(80);
unsigned long count=0;
double variable1 = 0;
double variable2 = 0;
double avgtime = 0;


float gX, gY, gZ;            //Gyro X, Y, Z
float aX, aY, aZ;            //Acceleration X, Y, Z
float mX, mY, mZ;            //Magneto X, Y, Z
char buffer[30];             //output format için string
double avgtimearr[100] = {0};

void setup() {
  
  
  
  Serial.begin(1000000);     //baud rate
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

            while(1){
            // the content of the HTTP response follows the header:
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
            
            client.print(aX);
            client.print(",");
            client.print(aY);
            client.print(",");
            client.print(aZ);
            variable1 = millis();
            if (count % 2 == 0){
              variable2 = millis();
            }
            if (count % 2 == 1){
              avgtimearr[count] = variable1 - variable2;
            }
            ++count;

            if(count == 100){
              for( int i =0 ; i<100 ; i++){
                avgtime = avgtime + avgtimearr[i];
                }
                count=0;
                avgtime = avgtime / 100;
                client.print("             ");
                client.print(avgtime);
                avgtime = 0;
            }
            client.print("<br>");
            
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
