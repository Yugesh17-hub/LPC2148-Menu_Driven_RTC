#include <LPC21xx.H>
#include "proto.h"
volatile unsigned int flag;
int  main()
{
INTZ();
INTZ1();
rtc_int();
ENIT_EN();
rtc_fun();
while(1);
}
