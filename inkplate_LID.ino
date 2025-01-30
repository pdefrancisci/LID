#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <time.h>
#include <SdFat.h>
#define NAME_SIZE 20

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 3 Bit mode
SdFile file;                     // Create SdFile object used for accessing files on SD card
HTTPClient http;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

/*
jesus. ok. quite a bit to do!
+we need to download the images from syvvys
+we need to render the clock associated with each picture

*/
const int WIDTH = 800;
const int HEIGHT = 600;
const int TIMEZONE = -5;
//please do not go on my wifi
const char* ssid="Verizon_DX66BK";
const char* password="hives-vex6-gage";
const char* imageURL="http://syvvys:8008/dither/radarImage";
int lo;
int hi;
int lastRead;
time_t now;

bool downloadNewRadarImage(){
  Serial.println("in the problem area...");
  HTTPClient http;
  // Set parameters to speed up the download process.
  http.getStream().setNoDelay(true);
  // http.getStream().setTimeout(1);

  http.begin(imageURL);

  // Check response code.
  int httpCode = http.GET();
  if (httpCode == 200)
  {
      // Get the response length and make sure it is not 0.
      int32_t len = http.getSize();
      Serial.printf("response length=%i\n",len);
      if (len > 0)
      {
          if (display.sdCardInit()){
            SdFile file;
            static char name[NAME_SIZE];
            hi++;
            //untested!
            if(hi>9999){
              hi=10;
              lo=1;
            }
            snprintf(name, sizeof(name),"image%d.bmp",hi);
            Serial.printf("what's the matter with %s\n",name);
            if(file.open(name,O_WRITE|O_CREAT|O_TRUNC)){
              Serial.println("opened the file???");
              // Write the image data to the file
                uint8_t buffer[512]; // Buffer to hold image data
                int bytesRead = 0;
                while ((bytesRead = http.getStream().read(buffer, sizeof(buffer))) > 0) {
                    Serial.printf("Bytes read=%i\n",bytesRead);
                    file.write(buffer, bytesRead); // Write to internal flash
                }
                file.flush();
                file.close(); // Close the file after writing
                http.end();
                Serial.println("wrote the file???");
                return true;
                // display.partialUpdate();
            }
            else
            {
                Serial.println("File write failed!");
            }
            Serial.println("out of the problem area...");
          }
      }
      else
      {
          Serial.println("Invalid response length");
          http.end();
          return false;
      }
  }
  else
  {
      Serial.println("HTTP error");
      http.end();
      return false;
  }
  http.end();
  return false;
}

void setup()
{
    lo=1;
    hi=10;
    time(&now);
    lastRead = millis();
    Serial.begin(9600);
    WiFi.begin(ssid,password);
    timeClient.begin();
    Serial.print("Connecting to wifi...\n");
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print("Waiting...\n");
    }
    Serial.print("Connected!\n");

    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setTextColor(BLACK); // Set text color to black
    

    if (display.sdCardInit())
    {
        display.println("SD Card OK! Reading image...");
        display.display();
        // char fname[20];
        //   snprintf(fname, sizeof(fname),"image%d.bmp",i);
        //   if (!display.drawImage(fname, 0, 0, 0))
        //   {
        //       display.println("Image open error");
        //   }
        // display.partialUpdate();
        // delay(100);
    }
    else
    {
        display.clearDisplay();
        display.println("SD Card error!");
        display.display();
        display.clearDisplay();
        display.display();
        return;
    }
    display.setTextSize(18);      // Set font size to 3
}

// Function for drawing current time
void drawTime(int i, NTPClient client)
{
    // Drawing current time
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(4);
    char currentTime[100];
    // snprintf(currentTime,sizeof(currentTime),"currentTime %i:%i",i);
    int diff = hi-i;
    int min = client.getMinutes()-(diff*5);
    int hour = client.getHours()+TIMEZONE;
    while(min<0){
      min=min+60;
      hour=hour-1;
    }
    if(hour<0){
      hour=hour+24;
    }
    char* ampm = "AM";
    if(hour>12){
      hour=hour-12;
      ampm = "PM";
    }
    snprintf(currentTime,sizeof(currentTime),"%02i:%02i %s",hour,min,ampm);
    display.setCursor(450 - 20 * strlen(currentTime), 570);
    display.println(currentTime);
}

void loop()
{
  timeClient.update();
  int lb = lo;
  if(hi-12>lo){
    lb=hi-12;
  }
  //iterate through all of our images
  for(int i=lb; i<=hi; i++){
    char fname[20];
    snprintf(fname, sizeof(fname),"image%d.bmp",i);
    if (!display.drawImage(fname, 0, 0, 0))
    {
        display.println("Image open error");
    }
    drawTime(i, timeClient);
        
    display.partialUpdate();
  }
  //If more than 5 minutes have passed, go query the server for a new image
  if(millis()-lastRead>=5*60*1000){
    if(downloadNewRadarImage()){
      display.clearDisplay();
      display.print("Succeeded!");
      Serial.println("Added %i"+hi);
      display.display();
    }
    else{
      display.clearDisplay();
      display.print("Failed somehow!");    
      display.display();
    }
    lastRead = millis();
  }
  //Otherwise, just do a full refresh (for now)
  else{
    display.clearDisplay();
    display.display();
  }
}