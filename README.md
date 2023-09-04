# epever_solar_rs485
 
## Introduction
My camper van has an EPEVER solar controller and I wanted to collect the data to screate a logger as well as notify me if the battery ever dropped to low.

## RS485 uController
I am a fan of the ESP32 line of uC mostly because of the built-in Wifi and Bluetooth chips.   I went searching for a dev board that also had a RS485 chip and came across the LilyGo T-CAN485.
https://www.lilygo.cc/products/t-can485

## Wiring
I made a custom cable using some cat6 I had lying around.    I expect you could also cut the end off of an ethernet cable.  The pinout on the Epever (in the manual) has 2 pins for each signal.    I used one of the grounds for power and one of the grounds connected to the RS485 input.  This is to ensure no grounding issues when powered off of USB for testing/programing.

## Software

- Scan for available SSIDs
- If my home or starlink wifi is available, connect to the wifi.
- Enable the RS485 controller
- Query 20 registers from the solar controller using the modbus protocol
- Post the data to a google spreadsheet via an HTTPS post through a Google form (super lightweight on the uC side compared to using the sheets API)
- Set a 15 minute deep-sleep timer on the Real-time clock 
- Go to deep-sleep mode (turns off everything except the RTC clock)
- 15 minutes later it powers back on and repeats.

## Dashboard
I have done a few projects previously using a Google sheet to store data and a Looker datastudio report to display the data.   I do like free tools.
This software uses the GoogleFormPost library to send the data to a Googler Sheet.

I created a report that shows the last reported data plus a historical view of the Solar and Battery voltages.    For anyone trying to duplicate, the trick for showing "last received" data in a time-series data set is to create a blend with the same data set on either side of a left hand join.   Add a calculated field that is the MAX() of your date field and join that with the date field on the other side of the join.

## Alerting
One reason I like using Google sheets for this type of data storage is you can write App Script functions that can be triggers based on time or when data is posted to the spreadsheet.

I created an appscript function that triggers on a form post to the spreadsheet and acts on the battery voltage.
Write the voltage to a persistent script property - this allows you to access the results from other functions in the same script.
Check if the voltage is < 13.0
Call the prowlapp.com API to send an alert to my mobile phone.    (This could have been an email, but I wanted the mobile alert)
```
function onFormSubmit(e) {
  

  const timestamp = e.namedValues['Timestamp'];
  const batteryV = e.namedValues['BT_VOLTAGE'];

  if(!batteryV)
  {
    console.log("No battery voltage returned.");
    return;
  }
  console.log("Battery from form:"+batteryV);
      
  // Store the entry ID in user properties
  const scriptProperties = PropertiesService.getScriptProperties();
  scriptProperties.setProperty('batteryV', batteryV.toString());
  scriptProperties.setProperty('timestamp', timestamp.toString());

  lowbattery(batteryV, timestamp);      
  
}
```

I also added some logic to suppress duplicate alerts as well as send a notification every day at noon with the last recorded voltage.

## Backlog
- 3d print a case
- Tweak the sleep cycle after I collect some data.    Pushing to 30 or 60 min deep sleep cycle will even further reduce the power draw from the monitoring circuit.
- ~Clean up the code and post on github~
- ~Add functionality to the software so you can optionally put the hardware in pass-thru mode so you can use the Epever software to control the controller directly.~
- Add a BLE or ESP-NOW data burst on boot to support a remote display. (Maybe BLE so you can see it with a mobile phone using one of the BLE utilities on the app store)


