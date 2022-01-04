// This program for use with TFT display, temperature and power sensors
// Added extra calibration point for 10MHz to setup Reference amplifier
// dmc - 2021-04-11
// 2 pass conversion to string...
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define TERMINAL_BAUD 115200
#define __BAUD_RATE__ "BAUD = " TOSTRING(TERMINAL_BAUD) 
#define __WELCOME__ "Welcome  M0DMC"
#define SRC_FILE   "Source = " __FILE__

#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Use ILI9225 Display module 
#include "SPI.h"
#include "TFT_22_ILI9225.h"

// Use nano with TFT 220x176
#define TFT_RST 8
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_LED 3   // set to 0 and link to Vcc enable LED continuously
#define TFT_CLK 13  // SCK
#define TFT_BRT 120
class TFT_22_ILI9225  tft(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRT);

#include "ADCread.h";
class ADCread ADCread;

#include "ThermometerTFT.h"
class ThermometerTFT T;

#include "MeterTFT.h"
class MeterTFT MeterFwd,MeterRef;

#include "ShowReadingTFT.h"
class ShowReadingTFT Temperature,PowF,PowR,Volts,SWR;

#include "TermData.h"
class TermData TermData;

void setup() {
  TermData.Setup(TERMINAL_BAUD);
  sensors.begin();
  tft.begin();
  tft.setOrientation(3);
  tft.setFont(Terminal12x16);
  tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_WHITE);
  tft.drawText(20, 10, __FILE__,COLOR_WHITE);
  tft.drawText(20, 50, __WELCOME__,COLOR_GREEN);
  tft.drawText(20, 70, "Compiled - ",COLOR_GREEN);
  tft.drawText(20, 90,  __DATE__ ,COLOR_GREEN);
  tft.drawText(20, 110, __BAUD_RATE__ ,COLOR_GREEN);
  tft.drawText(20, 130,cal[ADCread.GetCalIndex()],COLOR_GREEN); 
  // Coords from top left...
  delay(5000);
  tft.clear();
  TermData.Welcome(SRC_FILE);
  MeterFwd.Setup(0  , 0 , 105,70,"Forward",1);// Calculate scale and update 
  MeterRef.Setup(110, 0 , 105,70,"Return",2);// to match range
  T.Setup(10,80,24,90);
  PowF.SetLocation(60,80);
  PowF.SetUnits('W');
  PowF.ShowLabel("Pf=");
  PowR.SetLocation(60,100);
  PowR.SetUnits('W');
  PowR.ShowLabel("Pr=");
  SWR.SetLocation(60,120);
  SWR.ShowLabel("SWR:");
  Volts.SetLocation(60,140);
  Volts.SetUnits('V');
  Volts.ShowLabel("V =");
  Temperature.SetLocation(60,160);
  Temperature.SetUnits('C');
  Temperature.ShowLabel("T =");
}

void loop() {
  int i;
  float Tc=0, f,r,d;
  unsigned long TimeNow,TempTimeCounter=0,CalTimeCounter=0;
  do{
    do {
      TimeNow=millis();
      if(TempTimeCounter<TimeNow){
        TempTimeCounter=TimeNow+1000;// Update temperature every 1s
        sensors.setWaitForConversion(true);
        if((Tc=sensors.getTempCByIndex(0))!=85){// 85 is error
	  Temperature.ShowValue(Tc);
	  T.NewReading((Tc-10)*3.333);// 10 to 30 equiv 0-100 range
	  TermData.ShowTempVusb(Tc,ADCread.getVsupplyUSB());
	  /// Show cal settings
          Serial.print((char*)cal[ADCread.GetCalIndex()]);
          Serial.print(" ");
          Serial.println(Offset[ADCread.GetCalIndex()]);
          ///
        }
        sensors.setWaitForConversion(false);
        sensors.requestTemperatures();
      }
      if(CalTimeCounter<TimeNow){
        CalTimeCounter=TimeNow+10000;// Update USB voltage measurement every 10s
        ADCread.GetADCratio();
        Volts.ShowValue((double)ADCread.getVsupplyUSB());
        ADCread.TogglePMax();
      }
      ADCread.ReadVoltages();
      SWR.ShowValue(ADCread.GetSWR());
      f=ADCread.GetFwddBm();
      r=ADCread.GetRefdBm();
      d=ADCread.GetPMax();
      MeterFwd.NewReading(TermData.ShowFwdPwr(ADCread.GetFwdVolts(),f,d));
      MeterRef.NewReading(TermData.ShowRefPwr(ADCread.GetRefVolts(),r,d));

      delay(200);  
    } while(Serial.available()==0);
    i=ADCread.GetCalIndex();
    switch(Serial.read()){
      default:
        break;
      case 'a':
        ADCread.SaveCal(i+1);
        break;
      case 'b':
        ADCread.SaveCal(i-1);
        break;
      case 'c':
	EEPROM.put(4+(i*sizeof(float)),Offset[i]+=0.1);
        break;
      case 'd':
	EEPROM.put(4+(i*sizeof(float)),Offset[i]-=0.1);
        break;
    }
    
  } while(true);
}
