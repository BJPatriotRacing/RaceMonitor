/*


 curl https://api.race-monitor.com/v2/Live/GetSession --data "apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd&raceID=140330"
 curl https://api.race-monitor.com/v2/Common/CurrentRaces --data "apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd"
 get live session
 curl https://api.race-monitor.com/v2/Live/GetSession --data "apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd&raceID=140330"
 curl https://api.race-monitor.com/v2/Live/GetRacerCount --data "apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd&raceID=140330"
 curl https://api.race-monitor.com/v2/Live/GetRacer --data "apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd&raceID=140330"

get live race feed

 const char *serverName = "https://api.race-monitor.com/v2/Common/CurrentRaces?apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd&seriesID=50";
 curl https://api.race-monitor.com/v2/Account/PastRaces --data "apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd"

 get races
 const char *serverName = "https://api.race-monitor.com/v2/Common/CurrentRaces?apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd";

 working
 const char *serverName = "https://api.race-monitor.com/v2/Common/RaceTypes?apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd";


 const char *token = "apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd";
 const char *serverName = "https://api.race-monitor.com/v2/Common/PastRaces/";




*/


#include <WiFi.h>           // Use in additional boards manager https://dl.espressif.com/dl/package_esp32_index.json
#include <WebServer.h>      // Use in additional boards manager https://dl.espressif.com/dl/package_esp32_index.json
#include <HTTPClient.h>     // Use in additional boards manager https://dl.espressif.com/dl/package_esp32_index.json
#include "time.h"           // https://github.com/lattera/glibc/blob/master/time/time.h
#include "elapsedMillis.h"  // https://github.com/pfeerick/elapsedMillis/blob/master/elapsedMillis.h
#include "Wire.h"
#include "WiFiClientSecure.h"
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "time.h"
#include "Colors.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "fonts\FreeSans18pt7b.h"
#include "fonts\FreeSans12pt7b.h"
#include "fonts\FreeSansBold12pt7b.h"
#include "fonts\FreeSans9pt7b.h"
#include <FlickerFreePrint.h>
#include <OneWire.h>
#include <Adafruit_ILI9341_Controls.h>
#include <EEPROM.h>
#include <ArduinoJson.h>  // https://github.com/bblanchon/ArduinoJson
#include "elapsedMillis.h"
#include "Adafruit_ILI9341_Keypad.h"
#include <ArduinoJson.h>  // https://github.com/bblanchon/ArduinoJson
#include <WirePacker.h>
#include <PatriotRacing_Utilities.h>

#define FONT_HEADER FreeSans12pt7b
#define FONT_HEADING FreeSans18pt7b
#define FONT_ITEM FreeSans12pt7b
#define FONT_BUTTON FreeSans12pt7b
#define C_RADIUS 4
#define I2C_SDA 21
#define I2C_SCL 22

#define CODE_VERSION "1.0"

#define PIN_RXD2 16
#define PIN_TXD2 17
#define PIN_M0 4
#define PIN_M1 22
#define PIN_AX 21
#define T_CS 12
#define T_IRQ 27
#define TFT_DC 2
#define TFT_CS 5
#define TFT_RST 25
#define PIN_LED 26

#define COL_CAR 15
#define COL_LAPS 90
#define COL_LAPTIME 160
#define COL_POS 270

#define STATUS_NEW 0
#define STATUS_DRAW 2
#define STATUS_DONE 3

#define DISABLE_COLOR 0x8410

uint16_t CarNumbers[] = {
  23,
  7,
  22,
  62,
  3,
  1,
  5,
  15,
};
uint16_t tCarNumbers[] = {
  23,
  7,
  22,
  62,
  3,
  1,
  5,
  15,
};


uint16_t CarLaps[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
};

uint16_t CarLapTime[] = { 0,
                          0,
                          0,
                          0,
                          0,
                          0,
                          0,
                          0 };
uint8_t CarPos[] = { 0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0 };

bool CarError[] = {
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
};
bool found = false;

uint8_t FlagStatus = 0;
int Row;
char buf[2];
char timebuf[10];
char buffer[12];
int BtnX, BtnY, BtnZ;
int HTTPCodeRaceData = 0;
int HTTPCodeCurrentRace = 0;
int x;
bool KeepIn = true;
int i;
byte h = 0, m = 0, s = 0;

int NumberOfSites = 0, CurrentSite = 0, SiteToUse = 0;
int NumberOfRaces = 0, CurrentRace = 0, SelectedRace = 999;
int32_t RaceTypeID = 0, SelectedRaceID = 0;

String JSONCurrentRace;
String JSONRaceData;

byte Start = 0;

Adafruit_ILI9341 Display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

XPT2046_Touchscreen Touch(T_CS, T_IRQ);

TS_Point TP;

IPAddress Actual_IP;
IPAddress PR_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

WebServer server(443);

DynamicJsonDocument CurrentRaceDoc(24000);
DynamicJsonDocument RaceDoc(24000);

DeserializationError CurrentRaceError;
DeserializationError RaceDataError;

elapsedSeconds UpdateRaceData;

Button MonitorBtn(&Display);
Button DefineCarsBtn(&Display);
Button DefineRaceBtn(&Display);
Button InternetConnectBtn(&Display);

Button Car1Btn(&Display);
Button Car2Btn(&Display);
Button Car3Btn(&Display);
Button Car4Btn(&Display);
Button Car5Btn(&Display);

Button DoneBtn(&Display);
Button RefreshBtn(&Display);
Button UpBtn(&Display);
Button DownBtn(&Display);

Button ConnectBtn(&Display);
Button PasswordBtn(&Display);

Button RaceIDBtn(&Display);
Button CarsBtn(&Display);

Keyboard SSIDPasswordPad(&Display, &Touch);

NumberPad CarNumberPad(&Display, &Touch);

FlickerFreePrint<Adafruit_ILI9341> ConnectStatus(&Display, C_WHITE, C_BLACK);

WirePacker PackedData;

RACE_MONITOR Data;

// 'Key', 20x20px
const unsigned char Key[] PROGMEM = {
  0x00, 0x70, 0x00, 0x01, 0xfc, 0x00, 0x03, 0xfe, 0x00, 0x03, 0x8e, 0x00, 0x07, 0x07, 0x00, 0x06,
  0x03, 0x00, 0x06, 0x03, 0x00, 0x07, 0x07, 0x00, 0x0f, 0xff, 0x80, 0x0f, 0xff, 0x80, 0x1f, 0xff,
  0xc0, 0x1f, 0x8f, 0xc0, 0x1f, 0x8f, 0xc0, 0x1f, 0xdf, 0xc0, 0x1f, 0xdf, 0xc0, 0x1f, 0xdf, 0xc0,
  0x1f, 0xdf, 0xc0, 0x1f, 0xff, 0xc0, 0x0f, 0xff, 0x80, 0x07, 0xff, 0x00
};

void setup() {


  Serial.begin(115200);
  /*
  delay(4000);
  Wire.begin();
  Wire.begin(13, 14);
  Wire.setClock(100000);
*/

  ledcSetup(0, 5000, 8);
  ledcAttachPin(PIN_LED, 0);

  EEPROM.begin(100);

  Display.begin();


  ledcWrite(0, 255);

  Display.setRotation(3);
  Display.fillScreen(C_BLACK);

  Display.fillRect(10, 40, 300, 20, C_RED);
  Display.fillRect(10, 160, 300, 20, C_BLUE);

  Display.setTextColor(C_WHITE);
  Display.setFont(&FONT_HEADER);
  Display.setCursor(10, 110);
  Display.print(F("PATRIOT RACING"));
  Display.setFont(&FONT_ITEM);
  Display.setCursor(10, 140);
  Display.print(F(CODE_VERSION));

  Display.setFont(&FONT_ITEM);

  Display.fillRoundRect(20, 200, 280, 30, 4, C_DKGREY);
  Display.fillRoundRect(20, 200, 10, 30, 4, C_GREY);
  Display.setCursor(30, 220);
  Display.print(F("Creating interface"));

  Touch.begin();

  CreateInterface();
  CarNumbers[0] = EEPROM.read(10);
  CarNumbers[1] = EEPROM.read(15);
  CarNumbers[2] = EEPROM.read(20);
  CarNumbers[3] = EEPROM.read(25);
  CarNumbers[4] = EEPROM.read(30);

  Display.setTextColor(C_WHITE, C_BLACK);
  Display.setFont(&FONT_ITEM);
  FlagStatus = 0;

  Display.fillRoundRect(20, 200, 280, 30, 4, C_DKGREY);
  Display.fillRoundRect(20, 200, 50, 30, 4, C_GREY);
  Display.setCursor(30, 220);
  Display.setTextColor(C_BLACK, C_GREY);
  Display.print(F("Waiting for server..."));

  delay(4000);
  Wire.begin();
  Wire.begin(13, 14);
  Wire.setClock(100000);

  Display.fillRoundRect(20, 200, 280, 30, 4, C_DKGREY);
  Display.fillRoundRect(20, 200, 280, 30, 4, C_GREY);
  Display.setCursor(30, 220);
  Display.print(F("Setup Complete"));
  delay(1000);

  MainMenu();

  disableCore0WDT();
  disableCore1WDT();

  DrawRaceProgress(true);

  UpdateRaceData = 20;  // force initial read
}

void loop() {

  if (Touch.touched()) {
    ProcessTouch();
    if (PressIt(CarsBtn) == true) {
      GetCars();
      DrawRaceProgress(true);
      UpdateRaceProgress();
      UpdateRaceData = 20;  // force update
    }
  }

  if (UpdateRaceData > 10) {

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("no internet...");
    }

    UpdateRaceData = 0;

    GetRaceData();

    UpdateRaceProgress();

    SendRaceData();
  }
}

void SendRaceData() {

  /*
  for (i = 0; i < 8; i++) {

    Serial.print(CarNumbers[i]);
    Serial.print(" ");
    Serial.print(CarLapTime[i]);
    Serial.print(" ");
    Serial.print(CarPos[i]);
    Serial.print(" ");
    Serial.print(CarLaps[i]);
    Serial.print(" ");
    Serial.println(FlagStatus);
  }
*/

  // clear packed data
  PackedData.reset();

  // build packet
  Data.CAR1 = (CarNumbers[0] << 22) | ((CarLapTime[0] & 0b0000000111111111) << 13) | ((CarPos[0] & 0b00011111) << 8) | ((CarLaps[0] & 0b01111111) << 1);
  Data.CAR2 = (CarNumbers[1] << 22) | ((CarLapTime[1] & 0b0000000111111111) << 13) | ((CarPos[1] & 0b00011111) << 8) | ((CarLaps[1] & 0b01111111) << 1);
  Data.CAR3 = (CarNumbers[2] << 22) | ((CarLapTime[2] & 0b0000000111111111) << 13) | ((CarPos[2] & 0b00011111) << 8) | ((CarLaps[2] & 0b01111111) << 1);
  Data.CAR4 = (CarNumbers[3] << 22) | ((CarLapTime[3] & 0b0000000111111111) << 13) | ((CarPos[3] & 0b00011111) << 8) | ((CarLaps[3] & 0b01111111) << 1);
  Data.CAR5 = (CarNumbers[4] << 22) | ((CarLapTime[4] & 0b0000000111111111) << 13) | ((CarPos[4] & 0b00011111) << 8) | ((CarLaps[4] & 0b01111111) << 1);
  Data.CARX = (CarPos[5] << 27) | ((CarPos[6] & 0b00011111) << 22) | ((CarPos[7] & 0b00011111) << 17) | ((FlagStatus & 0b00000011) << 15);

  // pack it
  PackedData.write((uint8_t *)&Data, (uint8_t)sizeof(Data));

  // end it
  PackedData.end();

  // now transmit the packed data
  while (PackedData.available()) {  // write every packet byte
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(PackedData.read());
    Wire.endTransmission();  // stop transmitting
  }
}


void MainMenu() {

  // UI to draw screen and capture input
  bool MainMenuKeepIn = true;

  DrawMainMenu();


  while (MainMenuKeepIn) {
    delay(50);


    // if touch screen pressed handle it
    if (Touch.touched()) {

      ProcessTouch();

      if (PressIt(MonitorBtn) == true) {
        MainMenuKeepIn = false;
      }
      if (PressIt(InternetConnectBtn) == true) {
        ConnectToInternet();
        DrawMainMenu();
      }
      if (PressIt(DefineCarsBtn) == true) {
        GetCars();
        DrawMainMenu();
      }

      if (PressIt(DefineRaceBtn) == true) {
        SelectRace();
        DrawMainMenu();
      }
    }
  }
}

void DrawMainMenu() {

  //nothing fancy, just a header and some buttons
  Display.fillScreen(C_BLACK);

  Display.setTextColor(C_BLACK, C_WHITE);
  Display.setFont(&FONT_HEADING);
  Display.setCursor(10, 31);
  Display.fillRect(0, 0, 320, 40, C_LTGREY);
  Display.print(F("Race Montior"));

  if (WiFi.status() != WL_CONNECTED) {
    MonitorBtn.disable();
    DefineRaceBtn.disable();
  } else {
    MonitorBtn.enable();
    DefineRaceBtn.enable();
  }

  MonitorBtn.draw();
  DefineCarsBtn.draw();
  DefineRaceBtn.draw();
  InternetConnectBtn.draw();
}

void DrawRaceProgress(bool Status) {

  //nothing fancy, just a header and some buttons
  Display.fillScreen(C_BLACK);


  Display.setFont(&FONT_HEADING);
  Display.setCursor(10, 31);
  Display.fillRect(0, 0, 320, 40, C_LTGREY);

  if (!Status) {
    Display.setTextColor(C_RED, C_WHITE);
    Display.print(F("No Internet"));
  } else {
    Display.setTextColor(C_BLACK, C_WHITE);
    Display.print(F("Race Data"));
  }



  Display.setTextColor(C_WHITE, C_BLACK);
  Display.setFont(&FONT_ITEM);

  Display.setCursor(COL_CAR, 65);
  Display.print(F("Car"));
  Display.setCursor(COL_LAPS - 15, 65);
  Display.print(F("Laps"));
  Display.setCursor(COL_LAPTIME - 20, 65);
  Display.print(F("Lap Time"));
  Display.setCursor(COL_POS, 65);
  Display.print(F("Pos"));

  Display.setCursor(COL_CAR, 100);
  Display.print(CarNumbers[0]);
  Display.setCursor(COL_CAR, 130);
  Display.print(CarNumbers[1]);
  Display.setCursor(COL_CAR, 160);
  Display.print(CarNumbers[2]);
  Display.setCursor(COL_CAR, 190);
  Display.print(CarNumbers[3]);
  Display.setCursor(COL_CAR, 220);
  Display.print(CarNumbers[4]);

  CarsBtn.draw();
}

void UpdateRaceProgress() {

  Display.setFont(&FONT_ITEM);
  /*
  if (FlagStatus == FLAG_GREEN) {
    Display.fillCircle(280, 20, 16, C_GREEN);
  } else if (FlagStatus == FLAG_RED) {
    Display.fillCircle(280, 20, 16, C_RED);
  } else if (FlagStatus == FLAG_YELLOW) {
    Display.fillCircle(280, 20, 16, C_YELLOW);
  } else {
    Display.fillCircle(280, 20, 16, C_GREY);
  }
*/
  Display.fillRect(COL_LAPS, 70, 310, 320 - COL_LAPS, C_BLACK);

  for (i = 0; i < 5; i++) {

    Display.setCursor(COL_LAPS, 100 + (i * 30));
    if (CarError[i]) {
      Display.setTextColor(C_RED, C_BLACK);
      Display.print("?");
    } else {
      Display.setTextColor(C_WHITE, C_BLACK);
      Display.print(CarLaps[i]);
    }

    Display.setCursor(COL_LAPTIME, 100 + (i * 30));
    if (CarError[i]) {
      Display.setTextColor(C_RED, C_BLACK);
      Display.print("?");
    } else {
      Display.setTextColor(C_WHITE, C_BLACK);
      m = (int)(CarLapTime[i] / 60);
      s = (int)(CarLapTime[i] % 60);
      sprintf(timebuf, "%02d:%02d", m, s);
      Display.print(timebuf);
    }

    Display.setCursor(COL_POS, 100 + (i * 30));
    if (CarError[i]) {
      Display.setTextColor(C_RED, C_BLACK);
      Display.print("?");
    } else {
      Display.setTextColor(C_WHITE, C_BLACK);
      Display.print(CarPos[i]);
    }
  }
}


void SelectRace() {

  // UI to draw screen and capture input
  bool SelectRaceKeepIn = true;

  DrawSelectRaceScreen();
  DrawSelectRaceHeader();

  DrawStatusBar(STATUS_NEW, 0);
  GetCurrentRaces();
  DrawStatusBar(STATUS_DRAW, 100);
  delay(100);
  DrawStatusBar(STATUS_DONE, 0);
  DrawSelectRaceHeader();

  CurrentRace = 0;
  DrawSelectedRace();

  while (SelectRaceKeepIn) {
    delay(50);

    // if touch screen pressed handle it
    if (Touch.touched()) {

      ProcessTouch();

      if (PressIt(DoneBtn) == true) {
        SelectedRace = CurrentRace;
        SelectRaceKeepIn = false;
      }
      if (PressIt(UpBtn) == true) {
        CurrentRace++;
        if (CurrentRace >= NumberOfRaces) {
          CurrentRace = 0;
        }
        DrawSelectedRace();
      }

      if (PressIt(DownBtn) == true) {

        if (CurrentRace == 0) {
          CurrentRace = NumberOfRaces - 1;
        } else {
          CurrentRace--;
        }
        DrawSelectedRace();
      }
      if (PressIt(RefreshBtn) == true) {
        DrawStatusBar(STATUS_NEW, 0);
        GetCurrentRaces();
        DrawStatusBar(STATUS_DRAW, 100);
        delay(100);
        DrawStatusBar(STATUS_DONE, 0);
        CurrentRace = 0;
        DrawSelectedRace();
      }

      if (PressIt(RaceIDBtn) == true) {

        //SSIDPasswordPad.value = 0;

        SSIDPasswordPad.getInput();
        //SelectedRaceID = SSIDPasswordPad.value;
        DrawSelectRaceScreen();
        DrawSelectRaceHeader();
        DrawSelectedRace();
        Display.fillRect(10, 45, 320, 40, C_BLACK);
      }
    }
  }
}


void DrawSelectRaceScreen() {

  //nothing fancy, just a header and some buttons
  Display.fillScreen(C_BLACK);

  Display.setTextColor(C_WHITE, C_BLACK);
  Display.setFont(&FONT_ITEM);

  RefreshBtn.draw();
  UpBtn.draw();
  DownBtn.draw();
  RaceIDBtn.draw();
  CarsBtn.draw();
}

void DrawSelectedRace() {

  Display.setTextColor(C_WHITE, C_BLACK);
  Display.setFont(&FONT_ITEM);

  if (NumberOfRaces > 0) {

    Display.setTextColor(C_WHITE, C_BLACK);
    Display.setFont(&FONT_ITEM);

    Display.fillRect(57, 100, 200, 40, C_BLACK);


    RaceTypeID = CurrentRaceDoc["Races"][CurrentRace]["RaceTypeID"];
    SelectedRaceID = CurrentRaceDoc["Races"][CurrentRace]["ID"];
    const char *temp = CurrentRaceDoc["Races"][CurrentRace]["Name"];
    /*
    Serial.print("CurrentRace = ");
    Serial.print(CurrentRace);
    Serial.print(", CurrentRaceID = ");
    Serial.print(CurrentRaceID);
    Serial.print(", RaceTypeID = ");
    Serial.print(RaceTypeID);
    Serial.print(", Race name = ");
    Serial.print(temp);
    Serial.print(", ID = ");
    Serial.println(SelectedRaceID);
*/
    Display.setCursor(60, 128);
    Display.print("Race ID: ");
    Display.print(SelectedRaceID);


    Display.fillRect(10, 45, 320, 40, C_BLACK);

    Display.setCursor(15, 75);

    if (strlen(temp) > 20) {
      for (i = 0; i < 20; i++) {
        Display.print(temp[i]);
      }
      Display.print("...");
    } else {
      Display.println(temp);
    }

  } else {
    Display.print("No races found");
  }
}

void DrawSelectRaceHeader() {

  Display.setFont(&FONT_HEADING);
  Display.fillRect(0, 0, 320, 40, C_LTGREY);
  Display.setCursor(10, 31);

  if (NumberOfRaces == 0) {
    ConnectStatus.setTextColor(C_BLACK, C_WHITE);
    ConnectStatus.print("Select Race");
  } else if (HTTPCodeCurrentRace != 200) {
    ConnectStatus.setTextColor(C_RED, C_WHITE);
    ConnectStatus.print("No Data");
  } else {
    ConnectStatus.setTextColor(C_GREEN, C_WHITE);
    ConnectStatus.print("Races");
  }

  DoneBtn.draw();
}

void GetCars() {

  bool GetCarsKeepIn = true;

  DrawGetCarScreen();

  while (GetCarsKeepIn) {
    delay(50);

    // if touch screen pressed handle it
    if (Touch.touched()) {

      ProcessTouch();

      if (PressIt(DoneBtn) == true) {
        GetCarsKeepIn = false;
      }

      if (PressIt(Car1Btn) == true) {
        CarNumberPad.value = CarNumbers[0];
        CarNumberPad.getInput();
        CarNumbers[0] = CarNumberPad.value;
      }
      if (PressIt(Car2Btn) == true) {
        CarNumberPad.value = CarNumbers[1];
        CarNumberPad.getInput();
        CarNumbers[1] = CarNumberPad.value;
      }
      if (PressIt(Car3Btn) == true) {
        CarNumberPad.value = CarNumbers[2];
        CarNumberPad.getInput();
        CarNumbers[2] = CarNumberPad.value;
      }
      if (PressIt(Car4Btn) == true) {
        CarNumberPad.value = CarNumbers[3];
        CarNumberPad.getInput();
        CarNumbers[3] = CarNumberPad.value;
      }
      if (PressIt(Car5Btn) == true) {
        CarNumberPad.value = CarNumbers[4];
        CarNumberPad.getInput();
        CarNumbers[4] = CarNumberPad.value;
      }
      DrawGetCarScreen();
    }
  }

  EEPROM.put(10, CarNumbers[0]);
  EEPROM.commit();
  EEPROM.put(15, CarNumbers[1]);
  EEPROM.commit();
  EEPROM.put(20, CarNumbers[2]);
  EEPROM.commit();
  EEPROM.put(25, CarNumbers[3]);
  EEPROM.commit();
  EEPROM.put(30, CarNumbers[4]);
  EEPROM.commit();
}

void DrawGetCarScreen() {

  //nothing fancy, just a header and some buttons
  Display.fillScreen(C_BLACK);
  Display.setFont(&FONT_HEADING);
  Display.fillRect(0, 0, 320, 40, C_LTGREY);
  Display.setCursor(10, 31);
  Display.setTextColor(C_BLACK, C_WHITE);
  Display.print("Select Cars");

  Display.fillRect(200, 40, 110, 200, C_BLACK);

  Display.setFont(&FONT_ITEM);

  Display.setCursor(200, 60);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print(CarNumbers[0]);

  Display.setCursor(200, 100);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print(CarNumbers[1]);

  Display.setCursor(200, 140);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print(CarNumbers[2]);

  Display.setCursor(200, 180);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print(CarNumbers[3]);

  Display.setCursor(200, 220);
  Display.setTextColor(C_WHITE, C_BLACK);
  Display.print(CarNumbers[4]);

  Car1Btn.draw();
  Car2Btn.draw();
  Car3Btn.draw();
  Car4Btn.draw();
  Car5Btn.draw();

  DoneBtn.draw();
}

void ConnectToInternet() {

  // UI to draw screen and capture input
  bool ConnectToInternetKeepIn = true;

  DrawConnectToInternet();
  DrawConnectToInternetHeader();

  DrawStatusBar(STATUS_NEW, 0);
  GetSites();
  DrawStatusBar(STATUS_DRAW, 100);
  delay(100);
  DrawStatusBar(STATUS_DONE, 0);

  SiteToUse = 0;
  DrawSSID();


  while (ConnectToInternetKeepIn) {
    delay(50);
    // if touch screen pressed handle it
    if (Touch.touched()) {

      ProcessTouch();

      if (PressIt(DoneBtn) == true) {
        ConnectToInternetKeepIn = false;
      }
      if (PressIt(UpBtn) == true) {
        SiteToUse++;
        if (SiteToUse >= NumberOfSites) {
          SiteToUse = 0;
        }
        DrawSSID();
      }

      if (PressIt(DownBtn) == true) {

        if (SiteToUse == 0) {
          SiteToUse = NumberOfSites - 1;
        } else {
          SiteToUse--;
        }
        DrawSSID();
      }
      if (PressIt(RefreshBtn) == true) {
        DrawStatusBar(STATUS_NEW, 0);
        GetSites();
        DrawStatusBar(STATUS_DRAW, 100);
        delay(100);
        DrawStatusBar(STATUS_DONE, 0);
        SiteToUse = 0;
        DrawSSID();
      }

      if (PressIt(PasswordBtn) == true) {
        SSIDPasswordPad.clearInput();
        SSIDPasswordPad.getInput();
        DrawConnectToInternet();
        DrawConnectToInternetHeader();
        DrawSSID();
        ConnectBtn.enable();
        ConnectBtn.draw();
      }

      if (PressIt(ConnectBtn) == true) {
        ConnectToSelectedInternet();
        DrawSSID();
        DrawConnectToInternetHeader();
      }
    }
  }
}


void DrawConnectToInternet() {

  //nothing fancy, just a header and some buttons
  Display.fillScreen(C_BLACK);
  DrawStatusBar(STATUS_NEW, 0);



  PasswordBtn.draw();
  ConnectBtn.draw();
  RefreshBtn.draw();
  UpBtn.draw();
  DownBtn.draw();
}



void ConnectToSelectedInternet() {
  i = 0;
  //Serial.println("starting server");
  DrawStatusBar(STATUS_NEW, 0);

  //Serial.println(WiFi.SSID(SiteToUse).c_str());
  //Serial.println(SSIDPasswordPad.data);

  WiFi.begin(WiFi.SSID(SiteToUse).c_str(), SSIDPasswordPad.data);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");

    DrawStatusBar(STATUS_DRAW, i += 2);
    if (i >= 100) {
      DrawStatusBar(STATUS_DONE, 0);
      return;
    }
  }
  //Serial.println();
  Actual_IP = WiFi.localIP();
  DrawStatusBar(STATUS_DRAW, 100);
  //Serial.println("CONNECTED");
  printWifiStatus();
  delay(100);
  DrawStatusBar(STATUS_DONE, 0);
}

void DrawSSID() {

  Display.setTextColor(C_WHITE, C_BLACK);
  Display.setFont(&FONT_ITEM);
  Display.fillRect(10, 45, 320, 40, C_BLACK);

  if (NumberOfSites > 0) {
    Display.setTextColor(C_WHITE, C_BLACK);
    Display.setFont(&FONT_ITEM);

    Display.setCursor(60, 128);
    Display.print("Choose SSID");
    Display.setCursor(15, 75);
    if (WiFi.SSID(SiteToUse).length() > 13) {
      Display.print(WiFi.SSID(SiteToUse).substring(0, 13));
      Display.print("...");
    } else {
      Display.print(WiFi.SSID(SiteToUse));
    }
    Display.print(" (");
    Display.print(WiFi.RSSI(SiteToUse));
    ConnectBtn.enable();
    if (WiFi.encryptionType(SiteToUse) != WIFI_AUTH_OPEN) {

      drawBitmap(Display.getCursorX(), 57, Key, 20, 20, C_RED);
      ConnectBtn.disable();
    }
    ConnectBtn.draw();
    Display.setCursor(Display.getCursorX() + 22, 75);
    Display.print(")");

  } else {
    Display.setCursor(15, 75);
    Display.print("No sites found");
  }
}

void DrawConnectToInternetHeader() {

  Display.setFont(&FONT_HEADING);
  Display.fillRect(0, 0, 320, 40, C_LTGREY);
  Display.setCursor(10, 31);
  if (WiFi.status() != WL_CONNECTED) {
    ConnectStatus.setTextColor(C_RED, C_WHITE);
    ConnectStatus.print("No Internet");
  } else {
    ConnectStatus.setTextColor(C_GREEN, C_WHITE);
    ConnectStatus.print("Connected");
  }

  DoneBtn.draw();
}


void GetCurrentRaces() {

  const char *sendString = "https://api.race-monitor.com/v2/Common/CurrentRaces?apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd";

  HTTPClient http;
  WiFiClientSecure client;

  http.setTimeout(6000);

  found = false;

  if (WiFi.status() == WL_CONNECTED) {

    //Serial.println("WL_CONNECTED ");
    DrawStatusBar(STATUS_DRAW, 10);
    http.begin(sendString);

    HTTPCodeCurrentRace = http.POST(sendString);
    DrawStatusBar(STATUS_DRAW, 20);
    //Serial.print("HTTP to get race data: ");
    //Serial.println(sendString);

    //Serial.print("HTTP Response code: ");
    //Serial.println(HTTPCodeCurrentRace);

    JSONCurrentRace = http.getString();
    DrawStatusBar(STATUS_DRAW, 70);
    //Serial.println(JSONData);
    CurrentRaceError = deserializeJson(CurrentRaceDoc, JSONCurrentRace);
    //Serial.print(F("deserializeJson() return code: "));
    //Serial.println(CurrentRaceError.f_str());
    NumberOfRaces = CurrentRaceDoc["Races"].size();
    DrawStatusBar(STATUS_DRAW, 80);

    /*
    Serial.println("List of current races");   
    Serial.print("number of race");
    Serial.println(NumberOfRaces);
    // NumberOfRaces = 30;
    for (i = 0; i < NumberOfRaces; i++) {
      Serial.print("Data: ");
      Serial.print(i);
      Serial.print(", ");
      int32_t sID = CurrentRaceDoc["Races"][i]["ID"];
      Serial.print(sID);
      Serial.print(", ");
      int32_t sSeriesName = CurrentRaceDoc["Races"][i]["RaceTypeID"];
      Serial.print(sSeriesName);
      Serial.print(", ");
      String sName = CurrentRaceDoc["Races"][i]["Name"];
      Serial.print(sName);
      Serial.print(", ");
      String sTrack = CurrentRaceDoc["Races"][i]["Track"];
      Serial.print(sTrack);
      Serial.println();
    }
    Serial.println();
*/
    http.end();
  }

  else {
    Serial.println("no wifi connection");
  }
}

void GetRaceData() {


  // curl https://api.race-monitor.com/v2/Live/GetSession --data "apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd&raceID=140330"

  // const char *sendString = "https://api.race-monitor.com/v2/Live/GetSession?apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd&raceID=37872";

  char sendString[200];
  char numbuf[3];
  sprintf(sendString, "https://api.race-monitor.com/v2/Live/GetSession?apiToken=518779b4-025b-45f4-9b82-f39d07c9aabd&raceID=%d", SelectedRaceID);
  const char *buffer = "                                    ";
  uint16_t test = 0;

  HTTPClient http;
  WiFiClientSecure client;

  http.setTimeout(13000);

  if (WiFi.status() == WL_CONNECTED) {

    // Serial.println("WL_CONNECTED ");

    http.begin(sendString);

    HTTPCodeRaceData = http.POST(sendString);

    //Serial.print("HTTP to get race data: ");
    //Serial.println(sendString);

    //Serial.print("HTTP Response code: ");
    //Serial.println(HTTPCodeRaceData);

    JSONRaceData = http.getString();

    //Serial.println(JSONData);
    RaceDataError = deserializeJson(RaceDoc, JSONRaceData);
    //Serial.print(F("deserializeJson() return code: "));
    // Serial.println(error.f_str());

    //int32_t Count = doc["Session"];
    //Serial.print("Total cars in this race: ");
    //Serial.print(Count);

    //Serial.println("Driver positions____________________");
    //Serial.println("Session time: ");

    buffer = RaceDoc["Session"]["SessionTime"];
    //Serial.println(buffer);

    buffer = RaceDoc["Session"]["FlagStatus"];
    if (buffer != nullptr) {
      //Serial.print("buffer ");
      //Serial.println(buffer);
      FlagStatus = FLAG_GREEN;
      if (buffer[0] == 'G') {
        FlagStatus = FLAG_GREEN;
      } else if (buffer[0] == 'Y') {
        FlagStatus = FLAG_YELLOW;
      } else if (buffer[0] == 'R') {
        FlagStatus = FLAG_RED;
      }
    }

    /*
    buffer = RaceDoc["Session"]["Competitors"] CarNumbers[0]["Number"];
    Serial.print(buffer);
    Serial.print(", ");
    buffer = RaceDoc["Session"]["Competitors"] CarNumbers[1]["LastName"];
    Serial.print(buffer);
    Serial.print(", ");
    buffer = RaceDoc["Session"]["Competitors"] CarNumbers[2]["Position"];
    Serial.print(buffer);
    Serial.print(", ");
    */

    for (i = 0; i < 8; i++) {

      sprintf(numbuf, "%d", CarNumbers[i]);
      //Serial.print("numbuf -");
      //Serial.print(numbuf);
      //Serial.println("-");

      buffer = RaceDoc["Session"]["Competitors"][numbuf]["Number"];

      if (buffer != nullptr) {
        test = atoi(buffer);

        if (test != CarNumbers[i]) {
          //Serial.println("car number mis match");
          CarError[i] = true;
        }
      } else {
        //Serial.print("car number read error: ");
        //Serial.println(CarNumbers[i]);
        CarError[i] = true;
      }


      buffer = RaceDoc["Session"]["Competitors"][numbuf]["Laps"];
      CarError[i] = false;
      if (buffer != nullptr) {
        //Serial.print("buffer ");
        //Serial.println(buffer);
        CarLaps[i] = atoi(buffer);
      } else {
        CarError[i] = true;
      }

      buffer = RaceDoc["Session"]["Competitors"][numbuf]["LastLapTime"];
      if (buffer != nullptr) {
        //Serial.print("buffer ");
        //Serial.println(buffer);
        h = (((buffer[0] - 48) * 10) + (buffer[1] - 48));
        m = (((buffer[3] - 48) * 10) + (buffer[4] - 48));
        s = (((buffer[6] - 48) * 10) + (buffer[7] - 48));
        CarLapTime[i] = ((h * 3600) + (m * 60) + s);
      } else {
        CarError[i] = true;
      }

      buffer = RaceDoc["Session"]["Competitors"][numbuf]["Position"];
      if (buffer != nullptr) {
        //Serial.print("buffer ");
        //Serial.println(buffer);
        CarPos[i] = atoi(buffer);
      } else {
        CarError[i] = true;
      }
    }

    http.end();
  }

  else {
    DrawRaceProgress(false);
    //Serial.println("no wifi connection");
  }
}

void printWifiStatus() {

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("Open http://");
  Serial.println(ip);
}


void CreateInterface() {

  // main screen
  MonitorBtn.init(60, 140, 100, 170, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Begin", 0, 0, &FONT_BUTTON);

  InternetConnectBtn.init(220, 80, 140, 50, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Internet", 0, 0, &FONT_BUTTON);
  DefineCarsBtn.init(220, 140, 140, 50, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Set Cars", 0, 0, &FONT_BUTTON);
  DefineRaceBtn.init(220, 200, 140, 50, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Set Race", 0, 0, &FONT_BUTTON);

  // define cars screen
  Car1Btn.init(80, 60, 100, 35, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Car 1", 0, 0, &FONT_BUTTON);
  Car2Btn.init(80, 100, 100, 35, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Car 2", 0, 0, &FONT_BUTTON);
  Car3Btn.init(80, 140, 100, 35, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Car 3", 0, 0, &FONT_BUTTON);
  Car4Btn.init(80, 180, 100, 35, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Car 4", 0, 0, &FONT_BUTTON);
  Car5Btn.init(80, 220, 100, 35, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Car 5", 0, 0, &FONT_BUTTON);

  //common buttons
  DoneBtn.init(250, 20, 100, 30, C_DKGREEN, C_GREEN, C_WHITE, C_BLACK, "Done", 0, 0, &FONT_BUTTON);
  RefreshBtn.init(70, 220, 120, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Refresh", 0, 0, &FONT_BUTTON);
  UpBtn.init(30, 120, 40, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "<", 0, 0, &FONT_BUTTON);
  DownBtn.init(280, 120, 40, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, ">", 0, 0, &FONT_BUTTON);

  CarsBtn.init(265, 20, 100, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Cars", 0, 0, &FONT_BUTTON);
  RaceIDBtn.init(70, 170, 120, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Race ID", 0, 0, &FONT_BUTTON);

  // internet connect buttons
  PasswordBtn.init(70, 170, 120, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Password", 0, 0, &FONT_BUTTON);
  ConnectBtn.init(210, 170, 120, 40, DISABLE_COLOR, C_GREY, C_BLACK, C_BLACK, "Connect", 0, 0, &FONT_BUTTON);

  SSIDPasswordPad.init(C_DKGREY, C_BLACK, C_LTGREY, C_LTGREY, DISABLE_COLOR, &FONT_BUTTON);

  //SSIDPasswordPad.enableDecimal(false);
  //SSIDPasswordPad.enableNegative(false);
  //SSIDPasswordPad.setDisplayColor(C_WHITE, C_BLACK);
  // SSIDPasswordPad.hideInput();

  CarNumberPad.init(C_DKGREY, C_BLACK, C_LTGREY, DISABLE_COLOR, C_LTGREY, DISABLE_COLOR, C_DKGREY, &FONT_BUTTON);
  CarNumberPad.enableDecimal(false);
  CarNumberPad.enableNegative(false);
  CarNumberPad.setDisplayColor(C_WHITE, C_BLACK);
  CarNumberPad.setMinMax(0, 999);

  //MonitorBtn.setColors(C_MDGREY, C_GREY, C_DKGREY, C_BLACK, C_LTGREY, DISABLE_COLOR);
  //DefineRaceBtn.setColors(C_MDGREY, C_GREY, C_DKGREY, C_BLACK, C_LTGREY, DISABLE_COLOR);

  // main screen
  MonitorBtn.setCornerRadius(C_RADIUS);
  DefineCarsBtn.setCornerRadius(C_RADIUS);
  DefineRaceBtn.setCornerRadius(C_RADIUS);
  InternetConnectBtn.setCornerRadius(C_RADIUS);
  Car1Btn.setCornerRadius(C_RADIUS);
  Car2Btn.setCornerRadius(C_RADIUS);
  Car3Btn.setCornerRadius(C_RADIUS);
  Car4Btn.setCornerRadius(C_RADIUS);
  Car5Btn.setCornerRadius(C_RADIUS);
  DoneBtn.setCornerRadius(C_RADIUS);
  RefreshBtn.setCornerRadius(C_RADIUS);
  UpBtn.setCornerRadius(C_RADIUS);
  DownBtn.setCornerRadius(C_RADIUS);
  ConnectBtn.setCornerRadius(C_RADIUS);
  PasswordBtn.setCornerRadius(C_RADIUS);

  MonitorBtn.setBorderThickness(0);
  DefineCarsBtn.setBorderThickness(0);
  DefineRaceBtn.setBorderThickness(0);
  InternetConnectBtn.setBorderThickness(0);
  Car1Btn.setBorderThickness(0);
  Car2Btn.setBorderThickness(0);
  Car3Btn.setBorderThickness(0);
  Car4Btn.setBorderThickness(0);
  Car5Btn.setBorderThickness(0);
  DoneBtn.setBorderThickness(0);
  RefreshBtn.setBorderThickness(0);
  UpBtn.setBorderThickness(0);
  DownBtn.setBorderThickness(0);
  ConnectBtn.setBorderThickness(0);
  PasswordBtn.setBorderThickness(0);
}

void GetSites() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("scan start");
  DrawStatusBar(STATUS_DRAW, 2);
  // WiFi.scanNetworks will return the number of networks found
  NumberOfSites = WiFi.scanNetworks();
  Serial.println("scan done");
  if (NumberOfSites == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(NumberOfSites);
    Serial.println(" networks found");
    for (int CurrentSite = 0; CurrentSite < NumberOfSites; ++CurrentSite) {

      DrawStatusBar(STATUS_DRAW, ((CurrentSite * 100) / NumberOfSites));
      // Print SSID and RSSI for each network found
      Serial.print(CurrentSite + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(CurrentSite));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(CurrentSite));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(CurrentSite) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(100);
}


bool PressIt(Button TheButton) {
  if (TheButton.press(BtnX, BtnY)) {
    TheButton.draw(B_PRESSED);
    while (Touch.touched()) {
      if (TheButton.press(BtnX, BtnY)) {
        TheButton.draw(B_PRESSED);
      } else {
        TheButton.draw(B_RELEASED);
        return false;
      }
      ProcessTouch();
    }

    TheButton.draw(B_RELEASED);
    return true;
  }
  return false;
}

void ProcessTouch() {

  BtnX = -1;
  BtnY = -1;

  if (Touch.touched()) {

    TP = Touch.getPoint();
    BtnX = TP.x;
    BtnY = TP.y;
    BtnZ = TP.z;
    //yellow headers
    BtnX = map(BtnX, 3970, 307, 320, 0);
    BtnY = map(BtnY, 3905, 237, 240, 0);

    //black headers
    //BtnX  = map(BtnX, 0, 3905, 320, 0);
    //BtnY  = map(BtnY, 0, 3970, 240, 0);
    // Display.fillCircle(BtnX, BtnY, 3, C_GREEN);
#ifdef SHOW_DEBUG
    Serial.print("Mapped coordinates : ");
    Serial.print(BtnX);
    Serial.print(", ");
    Serial.print(BtnY);
    Serial.print(", ");
    Serial.println(BtnZ);

#endif
  }
}


void DrawStatusBar(uint8_t FirstDraw, int StatusPercent) {
  uint8_t w = 0;


  if (FirstDraw == STATUS_NEW) {
    Display.fillRoundRect(140, 200, 160, 40, 4, C_DKGREY);
  } else if (FirstDraw == STATUS_DRAW) {
    // compute width
    // draw status bar
    w = (StatusPercent * 160) / 100;
    if (w <= 8) {
      w = 8;
    }

    Display.fillRoundRect(140, 200, w, 40, 4, C_GREEN);

  } else {
    Display.fillRoundRect(140, 200, 160, 40, 4, C_DKGREY);
  }
}


void drawBitmap(int16_t x, int16_t y, const unsigned char *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t ByteWidth = (w + 7) / 8, i, j;
  uint8_t sByte = 0;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i & 7) sByte <<= 1;
      else sByte = pgm_read_byte(bitmap + j * ByteWidth + i / 8);
      if (sByte & 0x80) Display.drawPixel(x + i, y + j, color);
    }
  }
}


// end of code