// adc124s051.h
#ifndef ADC124S051_H
#define ADC124S051_H

#include "hardware/spi.h"

typedef struct {
    spi_inst_t* spi_port;
    uint gpio_sck;
    uint gpio_mosi;
    uint gpio_miso;
    uint gpio_cs;
} ADC124S051;

void adc124s051_init(ADC124S051* adc, spi_inst_t* spi_port, uint gpio_sck, uint gpio_mosi, uint gpio_miso, uint gpio_cs);
float adc124s051_read(ADC124S051* adc, uint8_t channel);

#endif // ADC124S051_H
