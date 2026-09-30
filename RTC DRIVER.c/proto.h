//...............rtc.........//
void rtc_fun(void);
void rtc_int(void);
unsigned int getdigits(void);
int get_device_state(void); 
void dev_dis(void);
void onto_LCD(unsigned int val);
int get_device_state(void);

//........delays...........//
void delay_s(int i);
void delay_ms(int i);
void delay_us(int i);
//.........interrupts.......//
void ENIT_EN(void);
//..........LCD&&KEYPAD[intlizing].........//
void INTZ(void);
void INTZ1(void);
void WRITE_CMD(unsigned char cmd);
void WRITE_DATA(unsigned char letter);
void str(const char * s);
void digit(unsigned int n);
int colscan(void);
int rows(void);
int cols(void);
unsigned int keyvalue(void);
//.................LCD[pins]................//
#define DATA 8
#define REGSEL 17
#define REGWRT 19
#define REGEN  18
//............KEYPAD[pins]...............//
#define ROW0 16  // ROW pins //
#define ROW1 17
#define ROW2 18
#define ROW3 19

#define COL0 20   //col pins//
#define COL1 21
#define COL2 22
#define COL3 23
//.............rtc config...........//
#define PCLK 15000000
#define PREDIGIT ((PCLK/32768) -1)
#define PREFRC (PCLK-((PREDIGIT+1)*32768))
//.....................extern variable.................//
extern volatile unsigned int flag;
//_----------------MAIN_PASS------------------------//
#define MASTER_PIN 7777
