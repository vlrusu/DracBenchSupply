// main.c
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

#define CONTROL_PIN 21

// ADC Reference Voltage
#define PICO_ADC_VREF 3.3f

// ADC Resolution
#define PICO_ADC_RESOLUTION (1 << 12) // 12-bit ADC

char* names[]={"I5.0","I2.5","V5.0","V2.5"};

#define GETDEVICEID 'D'
#define POWERUP 'U'
#define POWEROFF 'O'
#define GETDATA 'A'

#define DEVICEID 0xB2

int main() {
  stdio_init_all();

  // Initialize the control pin as output and set it to low
  gpio_init(CONTROL_PIN);
  gpio_set_dir(CONTROL_PIN, GPIO_OUT);
  gpio_put(CONTROL_PIN, 1);


  adc_init();
  // Enable ADC for GPIO 29 (ADC3) which is connected to VSYS/3
  adc_gpio_init(26);
  adc_gpio_init(28);

  // Create an instance of ADC124S051 struct
  ADC124S051 adc;
  //void adc_init(ADC124S051* adc, spi_inst_t* spi_port, uint gpio_sck, uint gpio_mosi, uint gpio_miso, uint gpio_cs) {
  adc124s051_init(&adc, spi0, GPIO_SCK, GPIO_MOSI, GPIO_MISO, GPIO_CS); // Initialize with SPI and GPIO settings

  while (1) {

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
	for (int i = 0 ; i <4; i++){
	  float adc_value = adc124s051_read(&adc, i); // Read from channel 0
	  adc_value = adc124s051_read(&adc, i); // Read from channel 0
	  if (i==0 || i == 1){
	    adc_value = adc_value/(0.01*50);
	  
	  }

	  if (i==2) adc_value = adc_value*2.;


	
	  printf("%s: %.2f\n", names[i],adc_value);
	  sleep_ms(1000);
	}

	adc_select_input(0);
	adc_read();
	uint16_t raw = adc_read();
	float voltage = (raw * PICO_ADC_VREF) / PICO_ADC_RESOLUTION;
	float temp = (1.8455 - voltage)/0.01123;
	printf("Temp 2.5V %.2f\n", temp);

      
	adc_select_input(2);
	adc_read();
	raw = adc_read();
	voltage = (raw * PICO_ADC_VREF) / PICO_ADC_RESOLUTION;
	temp = (1.8455 - voltage)/0.01123;
	printf("Temp board %.2f\n", temp);

      }
      
      
  }
}
