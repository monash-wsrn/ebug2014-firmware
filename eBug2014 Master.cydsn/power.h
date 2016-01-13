#ifndef POWER_H
#define POWER_H

#include <cytypes.h>

int32 battery_voltage; //in mV
int32 battery_current; //in mA
int32 charge_current;  //in mA
int16 die_temperature; //in degrees C

void ADC_Power_Calibrate();
void Power_Start();
void hibernate();

#endif
