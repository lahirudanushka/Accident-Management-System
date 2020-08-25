
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <elapsedMillis.h>

SoftwareSerial gprsSerial(7, 8);
SoftwareSerial gpsSerial(3, 4);

TinyGPSPlus gps;

//Exact location
double lat,lng;

//Last known location
double latlast,lnglast;

//Last known location update period
int period = 15000; // 15Seconds



//Device Serial Number
     String SN = "TRK10002000";

//Configuration of devices
void setup() {
  // Config Buzzer
   pinMode(13, OUTPUT);

   Serial.begin(9600);
  
  // Configuring GSM Module
      gprsSerial.begin(9600);
      Serial.begin(9600);
  
  gprsSerial.flush();
  Serial.flush();
  delay(2000);
  
 // Configuring GPS Module    
    gpsSerial.begin(9600);

}


//*************************************Main Loop******************************************************

void loop() {
  //Waiting For Impact
  int impact = 0;
  int RecomendedImpact = 200;
  
  unsigned long time_now = 0;
  while(impact<RecomendedImpact){
    impact = analogRead(A0);
    Serial.println(impact);

     if(millis() > time_now + period && impact<RecomendedImpact){
        time_now = millis();
        LastKnownLocation();
    }
    
                                }

  
  //Accident alert tone
  if(impact==RecomendedImpact || impact>RecomendedImpact){
      tone(13, 2000);

     delay(200);

  //Getting GPS Location
        
        getLocation();
        
   //Send database to DataBase
        updateDB();
    
                                                         }
                                                         }
//********************************Last Known Location Function*****************************************

void LastKnownLocation(){
  if(gpsSerial.available()>0){
    gps.encode(gpsSerial.read());
                 latlast = (gps.location.lat());
                 lnglast = (gps.location.lng());
  }
}


//************************************Location Function************************************************

void getLocation(){

  //Waiting Period for get Exact location
        int period = 5000;
        unsigned long time_now = millis();;
 
  boolean ExactGPSFailflag= true;

      while(millis() < time_now + period && ExactGPSFailflag ){
        while(gpsSerial.available()>0){
             gps.encode(gpsSerial.read());
             
             if(gps.location.isUpdated()){
                 lat = (gps.location.lat());
                 lng = (gps.location.lng());
                 ExactGPSFailflag=false;
                                          }
  
                                        }

                   }

      //In case of Error in identification of Exact Location
      if(ExactGPSFailflag){
        Serial.println("Location Fail");
        lat=latlast;
        lng=lnglast;    
        Serial.println(lat);
        Serial.println(lng);         
      }
                   }

//****************************************GPRS Serial **************************************************

void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}

//***********************************Server Update Function********************************************

void updateDB(){

   //Listen to GPRS Serial  
     gprsSerial.listen();

   //attach or detach from GPRS service 
      gprsSerial.println("AT+CGATT?");
      delay(2000);
      toSerial();

    //bearer settings
      gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
      delay(2000);
      toSerial();

    //bearer settings
       gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"mobitel\"");
       delay(2000);
       toSerial();

    //bearer settings
       gprsSerial.println("AT+SAPBR=1,1");
       delay(6000);
       toSerial();
  
    //initialize http service
       gprsSerial.println("AT+HTTPINIT");
       delay(2000); 
       toSerial();

       gprsSerial.println("AT+HTTPPARA=\"CID\",1");
       delay(2000);


    
    //set http param value by calling the API
       gprsSerial.print("AT+HTTPPARA=\"URL\",\"http://yourhost.com/save_data_to_db.php?");
       gprsSerial.print("data1=");
       gprsSerial.print(lng);
       gprsSerial.print("&data2=");
       gprsSerial.print(lat);
       gprsSerial.print("&data3=");
       gprsSerial.print(SN);
       gprsSerial.println("\"");
       delay(7000);
       toSerial();

    //set http action type 0 = GET, 1 = POST, 2 = HEAD
       gprsSerial.println("AT+HTTPACTION=0");
       delay(6000);
       toSerial();

    //read server response
       gprsSerial.println("AT+HTTPREAD"); 
       delay(2000);
       toSerial();

       gprsSerial.println("");
       gprsSerial.println("AT+HTTPTERM");
       toSerial();
       
       while(true);
  
    }

//********************************************************************************************************
