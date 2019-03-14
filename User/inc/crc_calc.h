#ifndef CRC_HEAD
#define CRC_HEAD

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "gpio_driver.h"
#include <stdlib.h>

uint16_t crc16(uint8_t *buffer, uint8_t buffer_length);

#endif
