#ifndef ACC_H
#define ACC_H

#include <stdint.h>

typedef enum {
    DIR_FLAT = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

void      acc_init(void);
Direction acc_get_direction(void);

#endif