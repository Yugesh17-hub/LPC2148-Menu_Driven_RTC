#include <LPC21xx.H>
#include "proto.h"

// ---------------------------------------------------------
// KEYPAD MATRIX MAPPING
// 0-9 act as standard number inputs.
// 10 acts as the "Backspace" command (* button).
// 11-15 act as placeholders for A, B, C, D, #.
// ---------------------------------------------------------
int key[4][4] = {
    { 1,  2,  3, 0},
    { 4,  5,  6, 0},
    { 7,  8,  9, 0},
    {10,  11, 12, 0}
};

void delay_ms(int j);

void INTZ1(void)
{
    // Configure ROW pins as outputs
    IODIR1 |= 15 << ROW0;
}

// ---------------------------------------------------------
//.............SCAN COLUMNS FOR ANY KEY PRESS...............
int colscan(void)
{
    if(((IOPIN1 >> COL0) & 15) < 15){
        return 0; // Key is pressed
    }
    else return 1; // No key pressed
}

// ---------------------------------------------------------
//...................IDENTIFY ACTIVE ROW....................
int rows(void)
{
    int rno;
    for(rno = 0; rno < 4; rno++){
        IOPIN1 = (IOPIN1 & ((~15u) << ROW0)) | (~1u << (ROW0 + rno));
        if(colscan() == 0) break;
    }
    IOCLR1 = 15 << ROW0;
    return rno;
}

// ---------------------------------------------------------
//.................IDENTIFY ACTIVE COLUMN...................
int cols(void)
{
    int cno;
    for(cno = 0; cno < 4; cno++)
    {
        if(((IOPIN1 >> (COL0 + cno)) & 1) == 0)
            break;
    }
    return cno;
}

// ---------------------------------------------------------
//..............RETURN MAPPED KEY VALUE.....................

unsigned int keyvalue(void)
{
    unsigned int value, rowno, colno;
    
    while(colscan()){
	if(flag == 1) 
	return 99;}     // Wait for a key press
    delay_ms(200);         // Debounce delay
    
    rowno = rows();        // Find row
    colno = cols();        // Find column
    
    value = key[rowno][colno]; // Look up mapped value
    
    while(!colscan());     // Wait for key release
    
    return value;
}
