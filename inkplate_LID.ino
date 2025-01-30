#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <SdFat.h>
#define NAME_FORMAT "%I%M.bmp"
#define NAME_SIZE 32

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"            // Include Inkplate library to the sketch
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 3 Bit mode
SdFile file;                     // Create SdFile object used for accessing files on SD card
// SdFile wfile;
HTTPClient http;
time_t now;

/*
jesus. ok. quite a bit to do!
+we need to download the images from syvvys
+we need to sort those images by time
+we need to render those images
  +so we need to *find* those pictures...

*/
const int WIDTH = 800;
const int HEIGHT = 600;
//please do not go on my wifi
const char* ssid="Verizon_DX66BK";
const char* password="hives-vex6-gage";
const char* imageURL="http://syvvys:8008/dither/radarImage";

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
            strftime(name, sizeof(name), NAME_FORMAT, localtime(&now));
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
    now = time(0);
    Serial.begin(9600);
    WiFi.begin(ssid,password);
    Serial.print("Connecting to wifi...\n");
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print("Waiting...\n");
    }
    Serial.print("Connected!\n");

    display.begin();             // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();      // Clear frame buffer of display
    display.setTextColor(BLACK); // Set text color to black
    display.setTextSize(3);      // Set font size to 3
    

    if (display.sdCardInit())
    {
        display.println("SD Card OK! Reading image...");
        display.display();
        
        if (!display.drawImage("image1.bmp", 0, 0, 0))
        {
            display.println("Image open error");
        }
        display.partialUpdate();
        delay(1000);
        
        if (!display.drawImage("image2.bmp", 0, 0, 0))
        {
            display.println("Image open error");
        }
        display.partialUpdate();
        delay(1000);

        if(downloadNewRadarImage()){
          display.clearDisplay();
          display.println("Succeeded!");
          display.display();
        }
        else{
          display.clearDisplay();
          display.println("Failed somehow!");    
          display.display();
        }
    }
    else
    {
        display.clearDisplay();
        display.println("SD Card error!");
        display.display();
        display.clearDisplay();
        display.display();
    }
}

void loop()
{
    // Nothing...
}
