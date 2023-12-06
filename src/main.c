// main.c
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "ADC124S051.h"

#include "pico/multicore.h"
#include "pico/multicore.h"


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

int main() {
    stdio_init_all();

    // Initialize the control pin as output and set it to low
    gpio_init(CONTROL_PIN);
    gpio_set_dir(CONTROL_PIN, GPIO_OUT);
    gpio_put(CONTROL_PIN, 0);

    // Create an instance of ADC124S051 struct
    ADC124S051 adc;
    //void adc_init(ADC124S051* adc, spi_inst_t* spi_port, uint gpio_sck, uint gpio_mosi, uint gpio_miso, uint gpio_cs) {
    adc124s051_init(&adc, spi0, GPIO_SCK, GPIO_MOSI, GPIO_MISO, GPIO_CS); // Initialize with SPI and GPIO settings

    while (1) {

      // Process keyboard entry, if any

      int input = getchar_timeout_us(10);
      if (input == '0')
	{
	  printf("Turning off\n");
	  gpio_put(CONTROL_PIN, 0);
	}
      else if (input == '1') {
	printf("Turning on\n");
	gpio_put(CONTROL_PIN, 1);
      }

      for (int i = 0 ; i <4; i++){
        float adc_value = adc124s051_read(&adc, 0); // Read from channel 0
        adc_value = adc124s051_read(&adc, 0); // Read from channel 0	
        printf("ADC %d: %.2f\n", i,adc_value);
        sleep_ms(1000);
      }
    }
}
