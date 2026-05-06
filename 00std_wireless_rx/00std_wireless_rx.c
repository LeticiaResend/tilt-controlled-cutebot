#include <stdio.h>
#include <nrf.h>
#include "nrf52833.h"
#include "radio_rx.h"
#include "motor.h"

static uint8_t pdu[8+1] = { 0 };
char last_cmd; 

int main(void) {
    
    radio_tx_init();
    i2c_init();

    while(1) {

        __WFE();

        last_cmd = radio_get_command();

        switch(last_cmd){
          case 'F': motors(30,30); break;
          case 'B': motors(-30,-30); break;
          case 'R': motors(30,0); break;
          case 'L': motors(0,30); break;
          default: motors(0,0); break;
        }
      
    }

}