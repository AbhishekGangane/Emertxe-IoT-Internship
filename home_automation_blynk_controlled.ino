/*************************************************************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings

#define BLYNK_TEMPLATE_ID "TMPL3nZ1gDxDn"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "RrmcK80cPrFNAuXwYOUchHqHdC6IAEG5"


// Comment this out to disable prints 
//#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw,inlet_sw,outlet_sw;
unsigned int tank_volume;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value*/
BLYNK_WRITE(COOLER_V_PIN)
{
  int value = param.asInt();
  if(value)
  {
    cooler_control(ON);
    lcd.setCursor(7,0);
    lcd.print("CO_LR ON ");
  }
  else
  {
    cooler_control(OFF);
    lcd.setCursor(7,0);
    lcd.print("CO_LR OFF");
  }
    
}

  



/*To turn ON and OFF heater based virtual PIN value*/
BLYNK_WRITE(HEATER_V_PIN )
{
  heater_sw = param.asInt();
   if(heater_sw)
  {
    heater_control(ON);
    lcd.setCursor(7,0);
    lcd.print("HT_R  ON ");
  }
  else
  {
    heater_control(OFF);
    lcd.setCursor(7,0);
    lcd.print("HT_R  OFF");
  }
  
}


/*To turn ON and OFF inlet vale based virtual PIN value*/
BLYNK_WRITE(INLET_V_PIN)
{
  /*Reading the value present on the virtual pin INLET_V_PI and storing it in inlet_sw*/
  inlet_sw = param.asInt();
  if(inlet_sw)
  {
    enable_inlet();
    lcd.setCursor(7,1);
    lcd.print("IN_FL_ON ");
  }
  else
  {
    disable_inlet();
    lcd.setCursor(7,1);
    lcd.print("IN_FL_OFF");
  }  
}
/*To turn ON and OFF outlet value based virtual switch value*/
BLYNK_WRITE(OUTLET_V_PIN)
{
  outlet_sw = param.asInt();
  if(outlet_sw)
  {
    enable_outlet();
    lcd.setCursor(7,1);
    lcd.print("OT_FL_ON ");
  }
  else
  {
    disable_outlet();
    lcd.setCursor(7,1);
    lcd.print("OT_FL_OFF");
  }  
  
}
/* To display temperature and water volume as gauge on the Blynk App*/  
void update_temperature_reading()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  /*Sending temperature reading to temperature gauge for every 1 sec*/
   Blynk.virtualWrite(TEMPERATURE_GAUGE,read_temperature());
  /*Send volume of water in the tank for every 1 sec*/
   Blynk.virtualWrite(WATER_VOL_GAUGE,volume());
  
}

/*To turn off the heater if the temperature raises above 35 deg C*/
void handle_temp(void)
{
    if((read_temperature() > float(35)) && heater_sw)
    {
      heater_sw = 0;
      heater_control(OFF);
      /*send notification to Dashboard*/
      lcd.setCursor(7,0);
      lcd.print("HT_R  OFF");
      
      /*send notification to Blynk IOT app*/
      Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Temperature is above 35 degree celcius\n");
      Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning off Heater\n");
      /*to turnoff the heater widget button*/
      Blynk.virtualWrite(HEATER_V_PIN, 0);
    }

  
}

/*To control water volume above 2000ltrs*/
void handle_tank(void)
{
  /*Check if vol < 2000 and inlet valve is off*/
  if ((tank_volume < 2000) && (inlet_sw == OFF))
  {
    enable_inlet();
    inlet_sw = ON;
    /* To print notification on dashboard*/
    lcd.setCursor(7,1);
    lcd.print("IN_FL_ON");
    /*To print notification on Blynk IOT app*/
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water level is less than 2000\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water inflow enabled\n");

    /*Reflect the status ON on the widget button of inlet valve*/
    Blynk.virtualWrite(INLET_V_PIN, ON);
    }

  /*If volume of the water is 3000 and if inlet valver is ON disable inflow*/

  if ((tank_volume == 3000 && inlet_sw == ON))
  {
    disable_inlet();
    inlet_sw = OFF;
    /* To print notification on dashboard*/
    lcd.setCursor(7,1);
    lcd.print("IN_FL_OFF");
    /*To print notification on Blynk IOT app*/
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water level is full\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water inflow disabled\n");

    /*Reflect the status OFF on the widget button of inlet valve*/
    Blynk.virtualWrite(INLET_V_PIN, OFF);
    
  }
  
  

}


void setup(void)
{
  Blynk.begin(auth);
  /*initialize the lcd*/
  lcd.init();
  /*turn the backlight */
  lcd.backlight();
  /*clear the clcd*/
  lcd.clear();
  /*cursor to the home */
  lcd.home();

  /*to display temperature*/
  lcd.setCursor(0,0);
  lcd.print ("T=");
  /*display volume*/
  lcd.setCursor(0,1);
  lcd.print ("V=");

  /*Initialization of temperature syatem*/
  init_temperature_system();
  init_ldr();
  /*Initializing the serial tank*/
  init_serial_tank();  
  /*update temperature to Blynk app every 1 sec */
  timer.setInterval(1000L, update_temperature_reading);
}

void loop(void) 
{
    /* To run Blynk related functions*/
    Blynk.run();
    /* To call setInterval at particular periods*/
    timer.run();

    /*read temperature and display it on the dashboard*/
    String temperature;
    temperature = String (read_temperature(), 2);
    lcd.setCursor(2,0);
    lcd.print(temperature);
    /*read the volume of water in the tank and display it on the dashboard*/
    tank_volume = volume();
    lcd.setCursor(2,1);
    lcd.print(tank_volume);
    /*To control the garden lights based on Light Intensity*/
    brightness_control();

    /* To control threshhold temperature of 35 degree celcius*/
    handle_temp(); 

    /*To control volume of the water in the tank*/
    handle_tank();         
}
