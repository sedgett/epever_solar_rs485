#define MOD_REG_COUNT 21

#define PV_VOLTAGE 0 
#define PV_CURRENT 1
#define PV_POWER_L 2
#define PV_POWER_H 3 
#define BT_VOLTAGE 4
#define BT_CURRENT 5
#define BT_POWER_L 6
#define BT_POWER_H 7
#define LD_VOLTAGE 8
#define LD_CURRENT 9
#define LD_POWER_L 10
#define LD_POWER_H 11
#define BT_CELCIUS 12
#define EQ_CELCIUS 13
#define PC_CELCIUS 14
#define BT_PERCENT 15
#define RMT_BT_TMP 16
#define BT_RTD_PWR 17
#define BT_STATUS 18
#define CE_STATUS 19
#define DE_STATUS 20

//#### MODBUS REGISTERS 

#define PV_VOLTAGE_ 0x3100 //(V|100) Solar charge controller--PV array voltage
#define PV_CURRENT_ 0X3101 //(A|100) Solar charge controller--PV array current
#define PV_POWER_L_ 0x3102 //(W|100) Solar charge controller--PV array power
#define PV_POWER_H_ 0x3103 //(W|100) Solar charge controller--PV array power
#define BT_VOLTAGE_ 0x3104 //(V|100) Battery voltage
#define BT_CURRENT_ 0x3105 //(A|100) Battery charging current
#define BT_POWER_L_ 0x3106 //(W|100) Battery charging power
#define BT_POWER_H_ 0x3107 //(W|100) Battery charging power
#define LD_VOLTAGE_ 0x310C //(V|100) Load voltage
#define LD_CURRENT_ 0x310D //(A|100) Load current
#define LD_POWER_L_ 0x310E //(W|100) Load power
#define LD_POWER_H_ 0x310F //(W|100) Load power
#define BT_CELCIUS_ 0x3110 //(℃|100) Battery Temperature
#define EQ_CELCIUS_ 0x3111 //(℃|100) Temperature inside case
#define PC_CELCIUS_ 0x3112 //(℃|100) Heat sink surface temperature of equipments power components 
#define BT_PERCENT_ 0x311A //(%|1) The percentage of battery's remaining capacity
#define RMT_BT_TMP_ 0x311B //(℃|100) The battery temperature measured by remote temperature sensor
#define BT_RTD_PWR_ 0x311D //(V|100) Current system rated voltage. 1200, 2400, 3600, 4800 represent 12V，24V，36V，48

#define BT_STATUS_ 0x3200 
/*
# Battery Status
# D3-D0:    00H Normal, 01H Overvolt, 02H Under Volt, 03H Low Volt Disconnect, 04H Fault
# D7-D4:    00H Normal, 01H Over Temp.(Higher than the warning settings), 02H Low Temp.(Lower than the warning settings)
# D8:       0 normal, 1 Battery inner resistance abnormal
# D15:      1-Wrong identification for rated voltage          
*/

#define CE_STATUS_ 0x3201 
/*
# Charging Equipment Status
# D15-D14:  Input volt status. 00H normal, 01H no power connected, 02H Higher volt input, 03H Input volt error.
# D13:      Charging MOSFET is short.
# D12:      Charging or Anti-reverse  MOSFET is short.
# D11:      Anti-reverse MOSFET is short.
# D10:      Input is over current.
# D9:       The load is Over current.
# D8:       The load is short.
# D7:       Load MOSFET is short.
# D4:       PV Input is short.
# D3-2:     Charging status. 00 No charging, 01 Float Charge, 02 Boost Charge, 03 Equalization.
# D1:       0 Normal, 1 Fault.
# D0:       0 Standby, 1 Running.

*/

#define DE_STATUS_ 0x3202
/*
# Discharging Equipment Status
# D15-D14:  00H normal, 01H low, 02H High, 03H no access Input volt error.
# D13-D12:  output power:00-light load,01-moderate,02-rated,03-overload
# D11:      short circuit
# D10:      unable to discharge
# D9:       unable to stop discharging
# D8:       output voltage abnormal
# D7:       input overpressure
# D6:       high voltage side short circuit
# D5:       boost overpressure
# D4:       output overpressure
# D1:       0 Normal, 1 Fault.
# D0:       0 Standby, 1 Running.



*/
