#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "FS.h"
#include <ArduinoJson.h>
#include <string>
#include <vector>
#include "time.h"
#include <Stepper.h>
#include <LiquidCrystal_I2C.h>

using namespace std;
//json
#define _LOAD_ERR -1
#define _SAVE_ERR -2
#define _PARSE_ERR -3
#define _LARGE_ERR -4

//med time
struct med_time{
  int	md_h;
  int	md_m;
  String heure;
};

struct med_time matin;
struct med_time midi;
struct med_time apres_midi;
struct med_time soir;


struct tm timeinfo;
float Time(millis());
float timePrev(millis());
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
bool have_take_med = false;

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// Replace with your network credentials
char ssid[] = "D-LINK MAISON";
char password[] = "INeedYou";
Stepper myStepper(200, 27, 26, 25, 33);

AsyncWebServer server(80);

//float  Time, timePrev, tempo_30;

 vector<String> explode(const String& s, const char& c){
	String buff{""};
	vector<String> v;
	
	for(auto n:s)
	{
		if(n != c) buff+=n; else
		if(n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if(buff != "") v.push_back(buff);
	
	return v;
}

bool timeCounter(float duraction){
   float tempo_time = (Time - timePrev) / 1000;
   Serial.printf("holla time passed  %d" ,tempo_time);
if(tempo_time > duraction){
   Serial.printf("holla time passed ");
   return true;
}
   else {
   Time=millis();
   return false;
   }
}



bool medtook(){

   if (touchRead(4)<10){
      Serial.println("have_take_med1");
        return true;
      }
      else
     return false ;
}



bool takeMed(){
  myStepper.step(400);
  for (size_t i = 0; i < 5; i++){
    if(!medtook()){
    digitalWrite(12,HIGH);
    lcd.clear();
    delay(350);
    lcd.setCursor(3,0);
    Serial.println("take your");
    Serial.println("medication");
    lcd.print("take your");
    lcd.setCursor(3,1);
    lcd.print("medication");
    digitalWrite(12,LOW);
    delay(350);
    }
    else return true;
  }
  return false ;
}
void registerInHistorie(){
Serial.printf("writing stuff in historie ");
}

bool isMedTook(){
  int i=0;
  if (  timeinfo.tm_hour == midi.md_h && timeinfo.tm_min == midi.md_m && !have_take_med && timeinfo.tm_sec==00){
        if(!takeMed()){
          while (1){
            Serial.println("inside the loop");
            delay(1000);
            i++;
            Serial.printf("%d \n",i);
            if(i<10)
            {
              Serial.println("inside the counter ");
              if(medtook()){
                Serial.println(" took before 30 second");
                registerInHistorie();
               break;
              }
            }
            else if(i>10)
            {
              Serial.println(" 30 seconde  passed ");
             registerInHistorie();
            break;
            }
                        
          }
         
        }
        else
        {
          Serial.printf("\n out of the  loop ");
          registerInHistorie();
        }
        

  }
}

void getSchedule(){
 
  File configFile = SPIFFS.open("/time.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return ;
  }

  size_t size = configFile.size();
  if (size > 1024) {

    Serial.println("Config file size is too large");
    return ;

  }


  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return ;
  }
  String midie=doc["midi"];

vector<String> v=explode(midie, ':');    
Serial.printf(" heure:%s\n minute:%s",v[0],v[1]);
midi.md_h=atoi(v[0].c_str());
midi.md_m =atoi(v[1].c_str());
//Serial.printf(" heure int :%d\n minute int :%d",heure,minute);

}

void priseMedHours(JsonDocument& doc){
File timeFile = SPIFFS.open("/time.json", "w");
    if (!timeFile) {
      Serial.println("Failed to open config file for writing");
      return ;
    }
  serializeJson(doc, timeFile);
}

void printLocalTime(){
    lcd.setCursor(0, 0);
    if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println();
  
  lcd.setCursor(0, 0);
  lcd.print(&timeinfo, "%A,%H:%M:%S");

  lcd.setCursor(2, 1);
  lcd.print(&timeinfo, "%d/%b/%Y");
}

void setup(){

  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false);
  });
  
  // Route to load style.css file
  server.on("/css/freelancer.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/freelancer.css", "text/css");
  });
   server.on("/css/freelance.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/all.min.css", "text/css");
  });
  server.on("/css/all.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/all.min.css", "text/css");
  });
    // Route to load style.css file,,i-
  server.on("/js/form.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/form.js", "text/js");
  });
  server.on("/takeMed.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/takeMed.json", "text/json");
  });
    server.on("/time.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/time.json", "text/json");
  });
  
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
    int params = request->params();
    StaticJsonDocument <235> hourMed;

    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
     Serial.printf("POST<[%s]>: <%s>\n", p->name().c_str(), p->value().c_str());
     hourMed[p->name().c_str()]= p->value().c_str();
    }
    priseMedHours(hourMed);
    getSchedule();
    request->send(200, "text/plain", "Post route");
  });
getSchedule();
  // Start server
  server.begin();
  
}

void loop(){
    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
      delay(1000);
      return;
    }
   // printLocalTime();

   isMedTook();
   delay(1000);
}