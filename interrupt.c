#include <LPC21xx.H>
#include"proto.h"
extern volatile unsigned int flag;
void EINT_IRQ(void)__irq
{
flag = 1;
EXTINT = 1 << 0;
VICVectAddr = 0x0;
}
void ENIT_EN(void)
{
PINSEL1 = 0x15400001;
VICIntSelect = 0x0;
VICVectCntl0 = 14|1 << 5;
VICVectAddr0 = (unsigned int)EINT_IRQ;
VICIntEnable = 1 << 14;
EXTMODE = 1 << 0;
}
