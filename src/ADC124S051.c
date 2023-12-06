// adc124s051.c
#include "pico/stdlib.h"
#include "ADC124S051.h"

// ADC Reference Voltage
#define ADC_VREF 3.3f

// ADC Resolution
#define ADC_RESOLUTION (1 << 12) // 12-bit ADC


void adc124s051_init(ADC124S051* adc, spi_inst_t* spi_port, uint gpio_sck, uint gpio_mosi, uint gpio_miso, uint gpio_cs) {
    adc->spi_port = spi_port;
    adc->gpio_sck = gpio_sck;
    adc->gpio_mosi = gpio_mosi;
    adc->gpio_miso = gpio_miso;
    adc->gpio_cs = gpio_cs;

    // Initialize SPI
    spi_init(adc->spi_port, 1000000); // 1 MHz SPI clock
    gpio_set_function(adc->gpio_sck, GPIO_FUNC_SPI);
    gpio_set_function(adc->gpio_mosi, GPIO_FUNC_SPI);
    gpio_set_function(adc->gpio_miso, GPIO_FUNC_SPI);

    // Initialize CS pin
    gpio_init(adc->gpio_cs);
    gpio_set_dir(adc->gpio_cs, GPIO_OUT);
    gpio_put(adc->gpio_cs, 1); // Set CS high
}

float adc124s051_read(ADC124S051* adc, uint8_t channel) {
    uint8_t tx_buffer[2] = {0};
    uint8_t rx_buffer[2] = {0};

    // Prepare command to select channel
    tx_buffer[0] = (channel & 0x03) << 3;

    // Select ADC chip
    gpio_put(adc->gpio_cs, 0);

    // Transmit and receive data
    spi_write_read_blocking(adc->spi_port, tx_buffer, rx_buffer, 2);

    // Deselect ADC chip
    gpio_put(adc->gpio_cs, 1);

    // Convert received data to 12-bit ADC value
    uint16_t result = ((rx_buffer[0] & 0x0F) << 8) | rx_buffer[1];
    float voltage = (result * ADC_VREF) / ADC_RESOLUTION;
    return voltage;
}
