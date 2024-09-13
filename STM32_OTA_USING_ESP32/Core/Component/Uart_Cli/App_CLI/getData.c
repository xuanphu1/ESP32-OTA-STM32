#include "getData.h"

void getData(char **argv,uint8_t argv_num){

    if (argv_num < 2){

        responsePrint("don't enough arg, arg = %d\n",argv_num);
        return;
    } else if (argv_num > 2){
        responsePrint("too much arg, arg = %d\n",argv_num);
        return;

    } else {
        uint8_t channel = atoi(argv[1]);
        responsePrint("OK, arg = %d, channel = %d, temp= %f\n",argv_num,channel,getTemp(channel));

    }

}
