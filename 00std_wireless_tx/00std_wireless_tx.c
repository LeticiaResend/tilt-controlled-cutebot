#include <nrf.h>
#include "nrf52833.h"
#include "acc.h"
#include "radio.h"


uint32_t wait;


int main(void) {
    
    radio_init();
    acc_init();

    while(1) {

        Direction dir = acc_get_direction();

        switch(dir){
          case DIR_UP: radio_send('F'); break;
          case DIR_DOWN: radio_send('B'); break;
          case DIR_RIGHT: radio_send('R'); break;
          case DIR_LEFT: radio_send('L'); break;
          default: radio_send('S'); break;
        }
      
        wait = 2000000;
        while (wait--);
    }
}
