#include <LPC21xx.H>
void delay_s(int i)
{
T0PC = 0x0;
T0TC = 0x0;
T0TCR = 0x0;
T0PR = 15000000 - 1;
T0TCR = 1 << 0;
while(T0TC <= i);
T0TCR = 0X0;
}
void delay_ms(int i)
{
T0PC = 0x0;
T0TC = 0x0;
T0TCR = 0x0;
T0PR = 15000 - 1;
T0TCR = 1 << 0;
while(T0TC <= i);
T0TCR = 0X0;
}
void delay_us(int i)
{
T0PC = 0x0;
T0TC = 0x0;
T0TCR = 0x0;
T0PR = 15 - 1;
T0TCR = 1 << 0;
while(T0TC <= i);
T0TCR = 0X0;
}
