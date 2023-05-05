#include "ldr.h"
#include "Arduino.h"
#include "main.h"

void init_ldr(void)
{
   pinMode(GARDEN_LIGHT, OUTPUT);
   
}
void brightness_control(void)
{
  unsigned short inputVal;
  /*read values from LDR sensor*/
  inputVal = analogRead(LDR_SENSOR);
  /*scale it down from (0 to 1023) to (0 to 255)*/
  inputVal = inputVal/4;
  /*set the pwm from 255 to 0*/
  inputVal = 255 - inputVal;

  analogWrite (GARDEN_LIGHT, inputVal);

  delay(100);
}
