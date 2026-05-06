#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>

void radio_init(void);

void radio_send(char cmd);

#endif