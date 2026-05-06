#include <stdio.h>
#include <nrf.h>
#include "nrf52833.h"
#include "motor.h"

void i2c_init(void) {

    NRF_P0->PIN_CNF[26] = 0x00000602;
    NRF_P1->PIN_CNF[0]  = 0x00000602;

    NRF_TWI0->PSEL.SCL = 0x1A;
    NRF_TWI0->PSEL.SDA = 0x20;

    NRF_TWI0->FREQUENCY = 0x01980000;
    NRF_TWI0->ADDRESS   = 0x10;

    NRF_TWI0->ENABLE = 5;
}

void i2c_send(uint8_t* buf, uint8_t len) {

    NRF_TWI0->TXD = buf[0];
    NRF_TWI0->EVENTS_TXDSENT = 0;
    NRF_TWI0->TASKS_STARTTX = 1;

    for(int i = 1; i < len; i++) {
        while (!NRF_TWI0->EVENTS_TXDSENT);
        NRF_TWI0->EVENTS_TXDSENT = 0;
        NRF_TWI0->TXD = buf[i];
    }

    while (!NRF_TWI0->EVENTS_TXDSENT);
    NRF_TWI0->TASKS_STOP = 1;
}

void motors(int8_t left, int8_t right) {

    uint8_t dirL = (left >= 0) ? 1 : 0;
    uint8_t dirR = (right >= 0) ? 1 : 0;

    uint8_t speedL = (left >= 0) ? left : -left;
    uint8_t speedR = (right >= 0) ? right : -right;

    uint8_t L[] = {0x99,0x01,0x01,dirL,speedL,0x00,0x88};
    uint8_t R[] = {0x99,0x01,0x02,dirR,speedR,0x00,0x88};

    i2c_send(L, sizeof(L));
    i2c_send(R, sizeof(R));
}