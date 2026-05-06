#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

void motors(int8_t left, int8_t right);

void i2c_init(void);

void i2c_send(uint8_t* buf, uint8_t len);

#endif