#include "utility.h"

void init_SPI();

void cs_low();
void cs_high();

void reset_high();
void reset_low();

void reset();

void a0_high();
void a0_low();

void clk_low();

void spi_transmit(uint8_t data);
void spi_transmit_8(uint8_t data);
void spi_transmit_16(uint16_t data);