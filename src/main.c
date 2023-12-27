/* main.c --- 
 * 
 * Filename: main.c
 * Author: Vadim Rusu
 * Description: 
 * Created: Wed Dec 27 14:04:26 2023 (-0600)
 * Last-Updated: Wed Dec 27 14:47:31 2023 (-0600)
 *           By: Vadim Rusu
 *     Update #: 4
 */

/* Change Log:
 * 
 * 
 */

/* Code: */


#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "ADC124S051.h"

#include "pico/multicore.h"
#include "pico/multicore.h"

#include "hardware/adc.h"


#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "Debug.h"


#include "Infrared.h"
#include "LCD_1in14.h"

#define GPIO_SCK 6
#define GPIO_MOSI 7
#define GPIO_MISO 4
#define GPIO_CS 5

#define KEY_A 15
#define KEY_B 17



#define CONTROL_PIN 21

// ADC Reference Voltage
#define PICO_ADC_VREF 3.3f

// ADC Resolution
#define PICO_ADC_RESOLUTION (1 << 12) // 12-bit ADC


#define GETDEVICEID 'D'
#define POWERUP 'U'
#define POWEROFF 'O'
#define GETDATA 'A'

#define DEVICEID 0xB2

#define MAXTEMP 50

UWORD *BlackImage;
#define TOPCOLORWINDOW 50
#define STARTTEXTPOS 60

// Create an instance of ADC124S051 struct
ADC124S051 adc;

int voltagesetup;

float get_v5(){

  float adc_value = adc124s051_read(&adc, 2); // Read from channel 2
  adc_value = 2.*adc124s051_read(&adc, 2); // Read from channel 2
  return adc_value;
}

float get_v25(){
  float adc_value = adc124s051_read(&adc, 3); // Read from channel 3
  adc_value = adc124s051_read(&adc, 3); // Read from channel 3
  return adc_value;
}
	  

float get_i5(){
  float adc_value = adc124s051_read(&adc, 0); // Read from channel 3
  adc_value = adc124s051_read(&adc, 0); // Read from channel 3
  adc_value = adc_value/(0.01*50);
  return adc_value;
}

float get_i25(){
  float adc_value = adc124s051_read(&adc, 1); // Read from channel 3
  adc_value = adc124s051_read(&adc, 1); // Read from channel 3
  adc_value = adc_value/(0.01*50);
  return adc_value;
}

float get_temp25(){
  	adc_select_input(0);
	adc_read();
	uint16_t raw = adc_read();
	float voltage = (raw * PICO_ADC_VREF) / PICO_ADC_RESOLUTION;
	float temp = (1.8455 - voltage)/0.01123;
	return temp;
}
float get_temp(){
  	adc_select_input(2);
	adc_read();
	uint16_t raw = adc_read();
	float voltage = (raw * PICO_ADC_VREF) / PICO_ADC_RESOLUTION;
	float temp = (1.8455 - voltage)/0.01123;
	return temp;
}

// This function will be called when GPIO 5 goes high
void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == KEY_A)
    {
        // Your custom logic here
      voltagesetup=1;

    }

    else if (gpio == KEY_B)
    {
      voltagesetup=0;

    }
}


int main() {
  stdio_init_all();

  // Initialize the control pin as output and set it to low
  gpio_init(CONTROL_PIN);
  gpio_set_dir(CONTROL_PIN, GPIO_OUT);
  gpio_put(CONTROL_PIN, 1);

  voltagesetup=1;

  adc_init();
  // Enable ADC for GPIO 29 (ADC3) which is connected to VSYS/3
  adc_gpio_init(26);
  adc_gpio_init(28);

 
  //void adc_init(ADC124S051* adc, spi_inst_t* spi_port, uint gpio_sck, uint gpio_mosi, uint gpio_miso, uint gpio_cs) {
  adc124s051_init(&adc, spi0, GPIO_SCK, GPIO_MOSI, GPIO_MISO, GPIO_CS); // Initialize with SPI and GPIO settings


DEV_Delay_ms(100);
    printf("LCD_1in14_test Demo\r\n");
    if (DEV_Module_Init() != 0)
    {
        return -1;
    }

    DEV_SET_PWM(50);
    /* LCD Init */
    printf("1.14inch LCD demo...\r\n");
    LCD_1IN14_Init(HORIZONTAL);
    LCD_1IN14_Clear(WHITE);

    // LCD_SetBacklight(1023);
    UDOUBLE Imagesize = (LCD_1IN14_HEIGHT)*LCD_1IN14_WIDTH * 2;

    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    // /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, 0, RED);
    Paint_SetScale(65);
    Paint_Clear(WHITE);

    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);

    SET_Infrared_PIN(KEY_A);
    SET_Infrared_PIN(KEY_B);


    float v5,v25,i5,i25,t25,temp;
    int temp_alarm;
  
  while (1) {


    v5=get_v5();
    v25=get_v25();
    i5=get_i5();
    i25=get_i25();
    t25=get_temp25();
    temp=get_temp();

    int textpos = STARTTEXTPOS;

    temp_alarm = (temp>MAXTEMP || t25>MAXTEMP)?1:0;
				       
    
    if (temp_alarm == 0){
      if (voltagesetup){
	char v5string[20];
	char v25string[20];

	sprintf(v5string, "%.2xV %.2xA", v5,i5);
	sprintf(v25string, "%.2xV %.2xA", v25,i25);
	Paint_ClearWindows(1, TOPCOLORWINDOW, LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, GREEN);
	Paint_DrawString_EN(1, textpos, v5string, &Font20, 0x000f, 0xfff0);
	textpos+=20;
	Paint_DrawString_EN(1, textpos, v25string, &Font20, 0x000f, 0xfff0);

	LCD_1IN14_Display(BlackImage);
      }

      else{
	char tstring[20];
	sprintf(tstring, "%.2xC %.2xC", t25,temp);
	Paint_ClearWindows(1, TOPCOLORWINDOW, LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, GREEN);
	Paint_DrawString_EN(1, textpos, tstring, &Font20, 0x000f, 0xfff0);
	LCD_1IN14_Display(BlackImage);
      }	
    }

    else{
      Paint_ClearWindows(1, TOPCOLORWINDOW, LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, RED);
      Paint_DrawString_EN(1, textpos, "Temp Alarm, Fan fail", &Font20, 0x000f, 0xfff0);
    }
      
    // Process keyboard entry, if any
    int input = getchar_timeout_us(10);
    if (input == POWEROFF)
      {
	printf("Turning off\n");
	gpio_put(CONTROL_PIN, 0);
      }
    else if (input == POWERUP) {
      printf("Turning on\n");
      gpio_put(CONTROL_PIN, 1);
    }

    else if (input == GETDEVICEID)
      {
	printf("%.2x\n", DEVICEID);
      }

    else if (input == GETDATA)
      {
	char* names[]={"I5.0","I2.5","V5.0","V2.5"};
	printf("I5.0=%.2x\n",i5);
	printf("I2.5=%.2x\n",i25);
	printf("V5.0=%.2x\n",v5);
	printf("V2.5=%.2x\n",v25);
	printf("Temp2.5=%.2x\n",t25);
	printf("Temp=%.2x\n",temp);			


      }



    
      
      
      
  }
}

/* main.c ends here */
