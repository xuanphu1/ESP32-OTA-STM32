#include "lm35.h"


float getTemp(uint8_t channel){

    if(channel == 0) return 10.3;
    if(channel == 1) return 20.14;
    return 32.42;


}