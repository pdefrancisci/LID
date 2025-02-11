#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <time.h>
#include <SdFat.h>
#include <ArduinoJson.h>
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
int lastOpenWeatherMapCall;
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
    Serial.println("setup()");
    lo=1;
    hi=10;
    time(&now);
    lastRead = millis()-5*60*1000;
    lastOpenWeatherMapCall = millis()-5*60*1000;
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
void drawTime(int i, NTPClient client, bool colon)
{
    // Drawing current time
    display.setTextColor(WHITE);
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
    if(colon){
      snprintf(currentTime,sizeof(currentTime),"%02i:%02i %s",hour,min,ampm);
    }
    else{
      snprintf(currentTime,sizeof(currentTime),"%02i %02i %s",hour,min,ampm);
    }
    // Serial.printf("drawing time: %s\n",currentTime);
    display.fillRoundRect(450-(20*strlen(currentTime))-23, 550, 30*strlen(currentTime), 400, 40, BLACK);
    display.setCursor(450 - 20 * strlen(currentTime), 570);
    display.println(currentTime);
}
const int MINIMUM_SIZE = 56000;
bool isFileLargeEnough(const char *filename){
  if(!file.open(filename, O_RDONLY)){
    Serial.println("File does not exist");
    return false;
  }
  int filesize = file.fileSize();
  file.close();

  Serial.printf("File: %s, Size: %d bytes\n",filename, filesize);
  return filesize >= MINIMUM_SIZE;
}
void renderWeatherRadar(){
  timeClient.update();
  int lb = lo;
  if(hi-12>lo){
    lb=hi-12;
  }
  //iterate through all of our images
  for(int i=lb; i<=hi; i++){
    char fname[20];
    snprintf(fname, sizeof(fname),"image%d.bmp",i);
    if(isFileLargeEnough(fname)){
      if (!display.drawImage(fname, 0, 0, 0))
      {
          display.println("Image open error");
      }
    }
    drawTime(i, timeClient, true);
        
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
void renderClockImage(){
  HTTPClient http;
  // Set parameters to speed up the download process.
  http.getStream().setNoDelay(true);
  http.getStream().setTimeout(1);

  // Photo taken by: Roberto Fernandez
  http.begin("http://syvvys:8008/randImage");

  // Check response code.
  int httpCode = http.GET();
  if (httpCode == 200)
  {
      // Get the response length and make sure it is not 0.
      int32_t len = http.getSize();
      if (len > 0)
      {
          if (!display.drawBitmapFromWeb(http.getStreamPtr(), 0, 0, len))
          {
              // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
              // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
              // compression!
              display.println("Image open error");
              display.display();
          }
          display.display();
      }
      else
      {
          display.println("Invalid response length");
          display.display();
      }
  }
  else
  {
      display.println("HTTP error");
      display.display();
  }
  timeClient.update();
  bool c = true;
  for(int i=0; i<10; i++){
    drawTime(hi, timeClient, c);
    display.partialUpdate();
    c=!c;
    delay(250);
  }
  display.clearDisplay();
  display.display();
}
/*
todo:
2 rows 3 columns
center says "Indoor conditions"
time (already kind of done but needs to dynamically move)
aqi
humidity
pressure
temp
*/
//don't use this!!!
const char* openWeatherMapAPIKey = "3ab5c5c6f7bf91345d02ce33bb926195";
const char* city = "rochester";
const char* formatURL = "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=imperial";
float temp = 0;
int humidity = 0;
int cloud = 0;
int visibility = 0;
int sunrise = 0;
int sunset = 0;
int spacing = ((WIDTH/3)-250)/2;

void fetchWeatherData(){
  char weatherURL[128];
  snprintf(weatherURL,sizeof(weatherURL),formatURL,city,openWeatherMapAPIKey);
  // HTTPClient http;
  http.begin(weatherURL);
  int httpCode = http.GET();
  if (httpCode > 0) {
      String payload = http.getString();
      StaticJsonDocument<512> doc;
      deserializeJson(doc, payload);
      temp = doc["main"]["temp"];
      Serial.printf("temp=%i\n",temp);
      humidity = doc["main"]["humidity"];
      cloud = doc["clouds"]["all"];
      visibility = doc["visibility"];
      sunrise = doc["sys"]["sunrise"];
      sunset = doc["sys"]["sunset"];
      http.end();
  } else {
      Serial.println("Failed to connect");
      http.end();
  }
}
void renderHomeAssistant(){
  display.drawLine(0, HEIGHT/2, WIDTH, HEIGHT/2, BLACK);
  display.partialUpdate();
  delay(100);
  display.drawLine(WIDTH/3, 0, WIDTH/3, HEIGHT, BLACK);
  display.drawLine((WIDTH/3)*2, 0, (WIDTH/3)*2, HEIGHT, BLACK);
  display.partialUpdate();
  delay(100);
  char* outdoor = "outdoor";
  char* conditions = "conditions";
  display.setTextColor(BLACK);
  display.setTextSize(4);
  display.setCursor((WIDTH/3)+10, 8);
  display.println(outdoor);
  display.partialUpdate();
  display.setCursor((WIDTH/3)+10, 5*8);
  display.println(conditions);
  display.partialUpdate();
  display.setCursor((WIDTH/3)+10, 9*8);
  display.println(city);
  display.partialUpdate();
  display.drawImage("rochester.png", (WIDTH/3)+spacing, HEIGHT/2-122);
  display.partialUpdate();
  delay(100);
  //I only get 1,000 openWeatherMap calls a day.  
  //That's 41 an hour, one every five minutes, one every five fits
  if(millis()-lastOpenWeatherMapCall>=5*60*1000){
    fetchWeatherData();
    lastOpenWeatherMapCall = millis();
  }
  display.setCursor(0+10, 1*8);
  display.printf("Temp:%d f",int(temp));
  char* file;
  int width;
  if(temp<45){
    file="cold.png";
    width=250;
  }
  else if(temp<75){
    file="temp.png";
    width=157;
  } else{
    file="hot.png";
    width=255;
  }
  int spac = ((WIDTH/3)-width)/2;
  display.drawImage(file, spac, HEIGHT/2-250);
  display.partialUpdate();
  display.setCursor(0+8, HEIGHT/2+8);
  display.printf("Humidity:");
  //depending on the humidity, render more droplets
  int x_min = 0;
  int x_max = (WIDTH/3)-16;
  int y_min = (HEIGHT/2)+8;
  int y_max = 600-8;
  for(int i=0; i<humidity/5; i++){
    display.drawImage("droplet.png",random(x_min,x_max),random(y_min,y_max));
    display.partialUpdate();
  }
  display.setCursor(0+8, HEIGHT/2+5*8);
  display.printf("%i",humidity);
  display.partialUpdate();
  display.setCursor((WIDTH/3)+8, HEIGHT/2+8);
  display.printf("Cloud:",cloud);
  display.setCursor((WIDTH/3)+8, HEIGHT/2+5*8);
  display.printf("%i",cloud);
  display.partialUpdate();
  file;
  width;
  int height;
  if(cloud<33){
    file="sunny.png";
    width=250;
    height=246;
  }
  else if(cloud<66){
    file="partly.png";
    width=250;
    height=250;
  } else{
    file="cloud.png";
    width=250;
    height=159;
  }
  spac = ((WIDTH/3)-width)/2;
  // int vspac = ((HEIGHT/2)-height)/2;
  display.drawImage(file, (WIDTH/3)+spac, HEIGHT/2+9*8);
  display.partialUpdate();
  display.setCursor((WIDTH/3)*2+8, HEIGHT/2+8);
  display.printf("Visibility:");
  display.setCursor((WIDTH/3)*2+8, HEIGHT/2+5*8);
  display.printf("%i",visibility);
  display.partialUpdate();
  display.setCursor((WIDTH/3)*2+8, 0+8);
  timeClient.update();
  int hour;
  int minute;
  const char* ampm = "am";
  struct tm timeinfo;
  if(timeClient.getEpochTime()<sunrise||timeClient.getEpochTime()>sunset){
    gmtime_r((time_t*)&sunrise, &timeinfo);
    display.printf("Sunrise:");
  }
  else{
    gmtime_r((time_t*)&sunset, &timeinfo);
    display.printf("Sunset:");
  }
  hour = timeinfo.tm_hour+TIMEZONE;
  if(hour>12){
    hour=hour-12;
    ampm = "pm";
  }
  minute = timeinfo.tm_min;
  display.setCursor((WIDTH/3)*2+8, 0+8*5);
  display.printf("%i:%i %s",hour,minute,ampm);
  display.partialUpdate();
  delay(3000);
  display.clearDisplay();
  display.display();
}
void renderSuntimes(){
  display.setTextColor(BLACK);
  display.setTextSize(4);
  //draw a nice looking arc to render on
  
  Serial.printf("Sunrise=%i",sunrise);
  Serial.printf("Sunset=%i",sunset);
  /*
  if it is before sunrise, get total time between sunrise and sunset
  then, take the difference between now and sunset
  then, we want the moon to be at WIDTH*((now-sunset)/(sunrise-sunset))
  */
  struct tm timeinfo;
  int total;
  int howLong;
  bool nightTime = false;
  if(timeClient.getEpochTime()<sunrise){
    //if the sun has already set, openweathermap will move the sunset forward
    //so, estimate when the sunset was by taking the next sunset, and subtracting a day
    Serial.println("\nSun not up yet...");
    total = sunrise - (sunset-(24*60*60));
    howLong = timeClient.getEpochTime()-(sunset-(24*60*60));
    nightTime=true;
  }
  else if(timeClient.getEpochTime()>sunset){
    Serial.println("\nSun gone down...");
    total = (sunrise+(24*60*60)) - sunset;
    howLong = timeClient.getEpochTime()-sunset;
    nightTime=true;
  }
  /*
  if it is after sunrise, get total time between
  as well as diff between now and sunrise
  WIDTH*(how long its been / how long in total) to get the x component
  */
  else{
    Serial.println("\nSun going down...");
    total = sunset - sunrise;
    howLong = timeClient.getEpochTime()-sunrise;
  }
  int x = (int)(WIDTH*((float)howLong/(float)total));
  Serial.printf("howLong=%i total=%i\n",howLong,total);
  //just the bottom arc of the circle, the display starts at 0,0
  //in the left hand corner
  int y = 975-sqrt(640000-(x-WIDTH/2)^2);
  Serial.printf("ratio=%i y=%i");
  Serial.printf("x=%i y=%i\n",x,y);
  if(nightTime){
    display.fillCircle(x, y, 50, BLACK);
    display.drawCircle(WIDTH/2, WIDTH+225, WIDTH, BLACK);
  }
  else{
    display.drawCircle(x, y, 50, BLACK);
    display.fillCircle(WIDTH/2, WIDTH+225, WIDTH, BLACK);
  }
  display.display();
  timeClient.update();
  bool c = true;
  for(int i=0; i<10; i++){
    drawTime(hi, timeClient, c);
    display.partialUpdate();
    c=!c;
    delay(250);
  }
  // delay(5000);
  display.clearDisplay();
  display.display();
}
void loop()
{
  renderWeatherRadar();
  renderClockImage();
  renderHomeAssistant();
  renderSuntimes();

  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());

  //renderForecast();
  //renderBumper();
}
