#include "acc.h"
#include "nrf52833.h"

#define ACC_ADDR       0x19
#define REG_CTRL1      0x20
#define REG_CTRL4      0x23
#define REG_OUT_X_L    0x28
#define TILT_THRESHOLD 300

static void i2c_init(void) {
    NRF_P0->PIN_CNF[8]  = 0x00000602; // SCL
    NRF_P0->PIN_CNF[16] = 0x00000602; // SDA

    NRF_TWI0->ENABLE    = 0x00000005;
    NRF_TWI0->PSEL.SCL  = 0x00000008;
    NRF_TWI0->PSEL.SDA  = 0x00000010;
    NRF_TWI0->FREQUENCY = 0x01980000; // 100 kbps
    NRF_TWI0->ADDRESS   = ACC_ADDR;
}

void i2c_send(uint8_t* buf, uint8_t buf_len) {

    uint8_t i = 0;
    volatile uint32_t timeout;

    NRF_TWI0->TXD            = buf[i];
    NRF_TWI0->EVENTS_TXDSENT = 0;
    NRF_TWI0->TASKS_STARTTX  = 1;
    i++;

    while (i < buf_len) {
        while (NRF_TWI0->EVENTS_TXDSENT == 0);
        NRF_TWI0->EVENTS_TXDSENT = 0;
        NRF_TWI0->TXD            = buf[i];
        i++;
    }

    while (NRF_TWI0->EVENTS_TXDSENT == 0);
    NRF_TWI0->TASKS_STOP = 1;
}

void i2c_recv(uint8_t reg, uint8_t* buf, uint8_t len) {
    uint8_t i;

    NRF_TWI0->TXD            = reg | 0x80;
    NRF_TWI0->EVENTS_TXDSENT = 0;
    NRF_TWI0->EVENTS_ERROR   = 0;
    NRF_TWI0->TASKS_STARTTX  = 1;

    while (NRF_TWI0->EVENTS_TXDSENT == 0);

    if (NRF_TWI0->EVENTS_ERROR) {
        NRF_TWI0->EVENTS_ERROR = 0;
        NRF_TWI0->TASKS_STOP   = 1;
        return;
    }

    NRF_TWI0->EVENTS_TXDSENT = 0;

    NRF_TWI0->EVENTS_RXDREADY = 0;
    NRF_TWI0->TASKS_STARTRX   = 1;

    for (i = 0; i < len; i++) {

        if (i == len - 1) NRF_TWI0->TASKS_STOP = 1;
        while (NRF_TWI0->EVENTS_RXDREADY == 0);
        NRF_TWI0->EVENTS_RXDREADY = 0;
        buf[i] = NRF_TWI0->RXD;

    }
}

void acc_init(void) {
    uint8_t buf[2];

    i2c_init();

    buf[0] = REG_CTRL1; 
    buf[1] = 0x57;

    i2c_send(buf, 2);

    buf[0] = REG_CTRL4; 
    buf[1] = 0x88;

    i2c_send(buf, 2);
}

Direction acc_get_direction(void) {

    uint8_t buf[4];
    int16_t x, y, ax, ay;

    i2c_recv(REG_OUT_X_L, buf, 4);
    x = (int16_t)((buf[1] << 8) | buf[0]) >> 4;
    y = (int16_t)((buf[3] << 8) | buf[2]) >> 4;

    ax = x < 0 ? -x : x;
    ay = y < 0 ? -y : y;

    if (ax < TILT_THRESHOLD && ay < TILT_THRESHOLD) return DIR_FLAT;
    if (ax >= ay) return x > 0 ? DIR_RIGHT : DIR_LEFT;
    return y > 0 ? DIR_UP : DIR_DOWN;
}
