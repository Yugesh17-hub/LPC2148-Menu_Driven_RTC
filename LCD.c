#include <LPC21xx.H>
#include"proto.h"
void INTZ(void);
void WRITE_CMD(unsigned char cmd);
void WRITE_DATA(unsigned char letter);
void num(void);
int fabnoc(void);
void INTZ(void)
{
IODIR0 |= 0xff << DATA;
IODIR0 |= 1 << REGSEL;
IODIR0 |= 1 << REGWRT;
IODIR0 |= 1 << REGEN;
delay_ms(15);
WRITE_CMD(0x30);
delay_ms(5);
WRITE_CMD(0x30);
delay_ms(1);
WRITE_CMD(0x38);
WRITE_CMD(0x0E);
WRITE_CMD(0x01);
WRITE_CMD(0x06);
}
void WRITE_CMD(unsigned char cmd)
{
IOCLR0 = 1 << REGSEL;
IOCLR0 = 1 << REGWRT;
IOPIN0 = ((IOPIN0&~(255<<DATA))|(cmd << DATA));
IOSET0 = 1 << REGEN;
delay_ms(1);
IOCLR0 = 1 << REGEN;
delay_ms(2);
}
void WRITE_DATA(unsigned char letter)
{
IOCLR0 = 1 << REGWRT;
IOSET0 = 1 << REGSEL;
IOPIN0 = ((IOPIN0&~(255<<DATA))|(letter << DATA));
IOSET0 = 1 << REGEN;
delay_ms(1);
IOCLR0 = 1 << REGEN;
delay_ms(2);
}
void digit(unsigned int n)
{
int a[16];
signed int k = 0,l = 0 ,i;
while(n)
{
l = (n % 10) + 48;
a[k++] = l;
n /= 10;
}
for(i = k - 1; i>= 0;i--){
WRITE_DATA(a[i]);
}
}
void str(const char * s)
{
while(*s)
{
WRITE_DATA(*s);;
s++;
}
}
