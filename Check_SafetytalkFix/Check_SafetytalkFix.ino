
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>

//nodeMCU 1.0

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SS_PIN D4  //--> SDA / SS is connected to pinout D2
#define RST_PIN D3  //--> RST is connected to pinout D1

MFRC522 mfrc522(SS_PIN, RST_PIN);  //--> Create MFRC522 instance.

#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router

//----------------------------------------SSID and Password of your WiFi router-------------------------------------------------------------------------------------------------------------//
const char* ssid = "B2 12A Daring";
const char* password = "kucingmiaw";
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

int readsuccess;
byte readcard[4];
char str[32] = "";
String strUID, wd;
String payload, payload2;

int stoploop = false;

String hasilnama, hasiluuid, hasilemployeenumber;
int hasilstatus, hasildepartment;

String serverName = "http://192.168.0.143/access-app/data/dcu/employee";
String serverNamepost = "http://192.168.0.143/access-app/modul/safetytalk";

//-----------------------------------------------------------------------------------------------SETUP--------------------------------------------------------------------------------------//
void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC
  SPI.begin();      //--> Init SPI bus
  mfrc522.PCD_Init(); //--> Init MFRC522 card
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //-->OLED Address 0x3C
  delay(500);

  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");

  pinMode(ON_Board_LED, OUTPUT);
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.print("Menghubungkan...");

  Serial.print("Connecting");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.print("Terhubung ke : ");
  display.setCursor(10, 10);
  display.print(ssid);
  display.setCursor(10, 20);
  display.print("IP Address : ");
  display.setCursor(10, 30);
  display.print(WiFi.localIP());
  display.display();
  Serial.print("Konek!");

  delay(2000);


  display.setTextColor(WHITE);
  display.setCursor(10, 40);
  display.print("Press button");
  display.display();
  /**
    while (!stoploop) {
     int oo = 1;
      if (digitalRead(pinKiri)==LOW){
        stoploop = true;
        delay(500);
      }
    }
  **/
  /**
    for (int f = 0; f < 5; f++) {
      f = 0;
      if (digitalRead(pinKiri) == LOW) {
        f = 6;
      }
    }

    Serial.println("");
    Serial.print("Koneksi sukses di : ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Tap Kartu !");
    Serial.println("");

  **/

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------------LOOP---------------------------------------------------------------------------------------//
void loop() {
  // put your main code here, to run repeatedly
  WiFiClient client;
  readsuccess = getid();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.print("Silahkan Tap Kartu!");
  display.display();
  hasilstatus = 0;



  if (readsuccess) {
    digitalWrite(ON_Board_LED, LOW);
    HTTPClient http;    //Declare object of class HTTPClient


    String serverPath = serverName + "?uuid_card=" + wd;
    Serial.println(serverPath);

    http.begin(client, serverPath.c_str());
    //GET
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      //Serial.print("HTTP Response code: ");
      //Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
    }
    else {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 0);
      display.print("Komunikasi Error");
      display.display();

      Serial.print("Komunikasi Error, Error code: ");
      Serial.println(httpResponseCode);
    }
    decodestring();
    // stop resources
    http.end();

    if (hasilstatus == 200) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 0);
      display.print("Nama : ");
      display.setCursor(10, 10);
      display.print(hasilnama.substring(0, 13));
      display.setCursor(10, 20);
      display.print("No Pegawai : ");
      display.setCursor(10, 30);
      display.print(hasilemployeenumber);
      display.setCursor(10, 40);
      display.print("Department :");
      display.setCursor(10, 50);
      display.print(hasildepartment);
      display.display();
      delay(2000);
      Serial.print("Nama: ");
      Serial.println(hasilnama);
      Serial.print("No Pegawai: ");
      Serial.println(hasilemployeenumber);
      Serial.print("Department: ");
      Serial.println(hasildepartment);
      Serial.print("UUID: ");
      Serial.println(hasiluuid);

      //-----post data------//
      http.begin(client, serverNamepost);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"uuid\":\"" + hasiluuid + "\"}");
      payload2 = http.getString();

      Serial.print("HTTP POST Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("HTTP POST Response: ");
      Serial.println(payload2);
      http.end();

    }
    else {
      Serial.print("Pegawai tidak ditemukan "); Serial.print(hasilstatus); Serial.println(" Not Found");
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 0);
      display.print("Pegawai Tidak Ditemukan");
      display.display();
      //hasilstatus = 0;
    }
    delay(1000);
    digitalWrite(ON_Board_LED, HIGH);
  }
}

//void postdata(){

//}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------------Baca kartu----------------------------------------------------------------------------------------------------------//
int getid() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }


  Serial.println("UID kartu adalah : ");

  for (int i = 0; i < 4; i++) {
    readcard[i] = mfrc522.uid.uidByte[i]; //storing the UID of the tag in readcard
    array_to_string(readcard, 4, str);
    strUID = str;
  }
  Serial.print("String = "); Serial.println(strUID);

  if (strUID[4] == '0') {
    wd = strUID[5];
    wd = wd + strUID[2];
    wd = wd + strUID[3];
    wd = wd + strUID[0];
    wd = wd + strUID[1];
  }
  else {
    wd = strUID[4];
    wd = wd + strUID[5];
    wd = wd + strUID[2];
    wd = wd + strUID[3];
    wd = wd + strUID[0];
    wd = wd + strUID[1];
  }

  Serial.print("Wiegand = "); Serial.println(wd);
  mfrc522.PICC_HaltA();
  return 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------------------------ubah ke string------------------------------------------------------------------------------------------------------------//
void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------parse json dri get----------------------------------------------------------------------------------------------------//

void decodestring() {
  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(11) + 60;
  DynamicJsonBuffer jsonBuffer(capacity);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println(F("Parsing failed!"));
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 0);
    display.print("Database Error");
    display.display();
    return;
  }

  // Decode JSON/Extract values
  hasilstatus = root["status"].as<int>();
  hasilnama = root["data"]["name"].as<String>();
  hasilemployeenumber = root["data"]["employee_number"].as<String>();
  hasiluuid = root["data"]["uuid_card"].as<String>();
  hasildepartment = root["data"]["department_id"].as<int>();


}
