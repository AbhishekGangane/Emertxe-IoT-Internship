#include "temperature_system.h"
#include "Arduino.h"
#include "main.h"


void init_temperature_system(void)
{
  pinMode(COOLER, OUTPUT);
  pinMode(HEATER, OUTPUT);
    
  digitalWrite(COOLER, LOW);
  digitalWrite(HEATER, LOW);    
}

float read_temperature(void)
{
  float temperature;
  // converting digital values from ADC to temperature degree cel
  temperature = (((analogRead(A0) * (float)5/1023)) / (float) 0.01);
  return temperature; 
}

void cooler_control(bool control)
{

  digitalWrite(COOLER, control);
   
}
void heater_control(bool control)
{
  digitalWrite(HEATER, control);  
}
