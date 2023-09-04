#include <Arduino.h>
#include "config.h"
#include "gform.h"
#include "ssid.h"
#include <HardwareSerial.h>
#include <ModbusMaster.h>
#include <SPI.h>
#include <WiFi.h>
#include <GoogleFormPost.h>
#include "epever.h"
#include "esp_sleep.h"

//#define YOUR_FORM_URL XXX for the GoogleFormPost library in gform.h

//#define DEBUG 1 // Defining debug enables the debug code for the GoogleFormPost library

//Setup the RS485 and ModbusMaster
HardwareSerial Serial485(2);
ModbusMaster node;

//deep sleep config
#define uS_TO_S_FACTOR 1000000   /* Conversion factor for micro seconds to seconds */
const int TIME_TO_SLEEP=900;        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;
//RTC_DATA_ATTR int sleepmode = 1;  // if 1, then enable deepsleep mode.   This survives sleeps through RTC memory

//global arrays for the registers
const int registers[MOD_REG_COUNT] = {PV_VOLTAGE_,PV_CURRENT_,PV_POWER_L_,PV_POWER_H_,BT_VOLTAGE_,BT_CURRENT_,BT_POWER_L_,BT_POWER_H_,LD_VOLTAGE_,LD_CURRENT_,LD_POWER_L_,LD_POWER_H_,BT_CELCIUS_,EQ_CELCIUS_,PC_CELCIUS_,BT_PERCENT_,RMT_BT_TMP_,BT_RTD_PWR_,BT_STATUS_,CE_STATUS_,DE_STATUS_};
const String form_entities[MOD_REG_COUNT] = {ENTRY1,ENTRY2,ENTRY3,ENTRY4,ENTRY5,ENTRY6,ENTRY7,ENTRY8,ENTRY9,ENTRY10,ENTRY11,ENTRY12,ENTRY13,ENTRY14,ENTRY15,ENTRY16,ENTRY17,ENTRY18,ENTRY19,ENTRY20};
const String register_names[MOD_REG_COUNT] = {"PV_VOLTAGE", "PV_CURRENT", "PV_POWER_L", "PV_POWER_H", "BT_VOLTAGE", "BT_CURRENT", "BT_POWER_L", "BT_POWER_H", "LD_VOLTAGE", "LD_CURRENT", "LD_POWER_L", "LD_POWER_H", "BT_CELCIUS", "EQ_CELCIUS", "PC_CELCIUS", "BT_PERCENT", "RMT_BT_TMP", "BT_RTD_PWR", "BT_STATUS", "CE_STATUS", "DE_STATUS"};

//this stores the register results
uint16_t data[MOD_REG_COUNT]; // global data from solar controller

//used for a simple timer
int timer1=0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

int connect_wifi()
{
  //scan wifi
  //look for know ssids
  //connect
  //if connected, return true

  // Scan for available networks
  int numNetworks = WiFi.scanNetworks();
  Serial.print("Number of networks found: ");
  Serial.println(numNetworks);

  // Connect to a known network
  for (int i = 0; i < numNetworks; i++) {
 
    String ssid = WiFi.SSID(i);
    Serial.println(ssid);
    if (ssid == ssid1 || ssid == ssid2 || ssid == ssid3) {
      Serial.print("Connecting to ");
      Serial.println(ssid);

      const char* password;
      if (ssid == ssid1) {
        password = password1;
      } else if (ssid == ssid2) {
        password = password2;
      } else {
        password = password3;
      }

      WiFi.begin(ssid.c_str(), password);
      
      i=0;
      while (WiFi.status() != WL_CONNECTED) {
        i++;
        if(i>10) return 0;
        delay(1000);
        Serial.print(".");
        // could return FALSE eventually
      }

      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return 1;
    }
    //Serial.println ("not a match");
  }
  return 0;
}


void setup()
{
  Serial.begin(115200);
  delay(1000); //allow serial to settle
  Serial.println("EPever RS485 tool");
  Serial.println("Boot number: " + String(bootCount)); //bootCount is a RTC attriute so survices deep sleeps
  print_wakeup_reason();

  //setup rs485 chip  
  pinMode(RS485_EN_PIN, OUTPUT);
  digitalWrite(RS485_EN_PIN, HIGH);

  pinMode(RS485_SE_PIN, OUTPUT);
  digitalWrite(RS485_SE_PIN, HIGH);

  pinMode(PIN_5V_EN, OUTPUT);
  digitalWrite(PIN_5V_EN, HIGH);

  Serial485.begin(115200, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);

  if (bootCount != 0) //not first boot. first boot lets the loop run
  {
    bootCount++; //increment boot count
    do_send_data();
  }
  else{
    bootCount++; //increment boot count
  }
}
void do_send_data()
{
    node.begin(1, Serial485); //enable Modbus mode
      
    if (connect_wifi())
    {
      processsolardata();
    }
    goto_sleep();
}
void goto_sleep()
{
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to wakeup from sleep every " + String(TIME_TO_SLEEP) + " Seconds");
    Serial.println("Going to deep sleep.");
    Serial.flush();
    esp_deep_sleep_start();
}

uint16_t  getregister(int reg)
{
    //#include <ModbusMaster.h> - Global
    //ModbusMaster node; - Global

    if (node.readInputRegisters(reg, 1) == node.ku8MBSuccess)
    {
      //*most* registers just need a "/ 100.0"
      //status registers need a bit mask.  Values are multiplied by 100 so later on we can simply /100
      //we don't /100 now because passing and printing ints are better then floats in Arduino libraries
     
     uint16_t regdata = node.getResponseBuffer(0);
     switch (reg)
     {
      case CE_STATUS_:
        return ((regdata & 0b0000000000001100 ) >> 2) * 100;  // mask bits 3 and for and move to the right
        //return (node.getResponseBuffer(0));  // still debugging correct bitmask, doesn't lineup to documentation
        break;
      case BT_STATUS_:
        return ((regdata) & 0b1111)*100; // still debugging correct bitmask, doesn't lineup to documentation
        break;
      case DE_STATUS_:
        return (regdata & 0x0002) * 100;
        break;
      default:
        return regdata;
     } //switch
    }
    else
    {
      Serial.print("error getting register: ");
      Serial.println(reg);
      return 0; //no valid data
    }
}


void processsolardata()
{
  
  uint8_t i;
  //uint16_t data[MOD_REG_COUNT]; // makin this global so it store the last version for access from website
  char tmp[8];

    GoogleFormPost gf;
    
#ifdef DEBUG
    gf.setDebugMode(true);
#endif

    gf.setFormUrl( String(YOUR_FORM_URL) );
  
  for (i = 0; i < MOD_REG_COUNT ; i++)
  {
      char prnt[30];
      data[i] = getregister(registers[i]);
      dtostrf(data[i]/100.0,3,2,tmp);
      gf.addData( tmp, form_entities[i] );
      Serial.print(register_names[i]);
      Serial.print(":");
      Serial.print(tmp);
      Serial.print(":");
      Serial.print(data[i],HEX);


  }

  #ifdef DEBUG
    Serial.print("DEBUG: Battery Voltage: ");
    Serial.println(data[BT_VOLTAGE]/100.0,2);
    gf.showDebug();
  #endif

  gf.send();
  Serial.println("---");
  //OTATerminal.println("---");
  
}

void loop()
{
  
  while (Serial.available())
  {
    Serial485.write(Serial.read());
    timer1=millis(); //reset idle timer if anything from the serial port
  }
  while (Serial485.available())
    Serial.write(Serial485.read());

  if (millis() > timer1+(60*1000))  //1 min idle timer
  {
    Serial.println("First boot idle timer");
    do_send_data();
  }
  
}
