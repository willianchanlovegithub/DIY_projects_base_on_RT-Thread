#include "drv_gpio.h"

#define NRF24L01_CE_PIN         GET_PIN(A, 3)
#define NRF24L01_SPI_DEVICE     "spi10"

// if you don't use the interrupt of nrf24l01, ignore the following macro
#define NRF24_IRQ_PIN           GET_PIN(C,13)
