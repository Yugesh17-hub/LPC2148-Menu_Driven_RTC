#include <LPC21xx.H>
#include "proto.h"

// ---------------------------------------------------------
// GLOBALS
// ---------------------------------------------------------
extern volatile unsigned int flag;

static unsigned int n = 0;
static unsigned int on_HOURS = 9, on_MINS = 0;
static unsigned int off_HOURS = 17, off_MINS = 0; 

const char *days[7] = {"SUN","MON","TUE","WED","THUR","FRI","SAT"};

//Iap Flash Memory Config
//---------------------------------------------------------------//
#define IAP_LOCATION 0x7FFFFFF1
#define FLASH_SECTOR_14 0x00038000
void(*iap_tool)(unsigned int[],unsigned int[]) = (void (*)(unsigned int[],unsigned int[]))IAP_LOCATION;
unsigned int pepw[5];
unsigned int result[4];
unsigned int buff[128];

// ---------------------------------------------------------
// HELPER FUNCTIONS
// ---------------------------------------------------------
void print_2_digits(unsigned int val) {
    WRITE_DATA((val / 10) + 48);
    WRITE_DATA((val % 10) + 48);
}

int get_device_state(void) {
    int curr_m = (HOUR * 60) + MIN;
    int on_m = (on_HOURS * 60) + on_MINS;
    int off_m = (off_HOURS * 60) + off_MINS;

    if (on_m == off_m) return 0; 
    if (on_m < off_m) return (curr_m >= on_m && curr_m < off_m);
    else return (curr_m >= on_m || curr_m < off_m);
}

// Validates the maximum number of days in a given month and year
unsigned int get_max_days(unsigned int m, unsigned int y) {
    if (m == 2) {
        // Leap year condition: divisible by 4, not 100, unless divisible by 400
        return ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) ? 29 : 28;
    }
    if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
    return 31;
}

//...................get-digits (With Backspace)..................//
unsigned int getdigits(void)
{
    unsigned int digits[2] = {0, 0};
    int i = 0;
    unsigned int val;
    
    while(i < 2) {
        val = keyvalue();
		//---------interrupt triggered--------------//
		if(val == 99){
					flag = 0;
					n = 0;
					WRITE_CMD(0x01);
					return 0;
				}

        
        // If 'Back' (10) is pressed and we aren't at the first digit
        if (val == 10 && i > 0) {
            i--;
            WRITE_CMD(0x10); // Move cursor left
            WRITE_DATA(' '); // Overwrite with space
            WRITE_CMD(0x10); // Move cursor left again to wait for new input
            delay_ms(200);   // Debounce
        } 
        // If a valid number 0-9 is pressed
        else if (val < 10) {
            digits[i] = val;
            WRITE_DATA(val + 48); // Print to LCD
            i++;
            delay_ms(200);   // Debounce
        }
    }
    return (digits[0] * 10) + digits[1];
}
//----------------get the password-------------
unsigned int get_password(void)
{
	unsigned int digits[4] = {0,0,0,0};
	int i = 0;
	unsigned int val,checkval;
	WRITE_CMD(0x01);
  WRITE_CMD(0x80); str("Enter PIN:");
  WRITE_CMD(0xC0);
	while(i < 4) {
        val = keyvalue();
		//.......Interrrupt triggered.........//
		    if(val == 99){
					flag = 0;
					n = 0;
					WRITE_CMD(0x01);
					return 0;
				}
        // If 'Back' (10) is pressed and we aren't at the first digit
        if (val == 10 && i > 0) {
            i--;
            WRITE_CMD(0x10); // Move cursor left
            WRITE_DATA(' '); // Overwrite asterisk with space
            WRITE_CMD(0x10); // Move cursor left again
            delay_ms(200);   // Debounce
        } 
        // If a valid number 0-9 is pressed
        else if (val < 10) {
            digits[i] = val;
            WRITE_DATA(val+48); // Print asterisk instead of the number
            delay_ms(70);
			WRITE_CMD(0x10);
			WRITE_DATA('*');
			

			i++;
            delay_ms(200);   // Debounce
        }
    }
	    // Wait for the 'Enter' key (11) or 'Abort' (99)
    do {
        checkval = keyvalue();
        if (checkval == 99) {
            flag = 0;
            n = 0;
            WRITE_CMD(0x01);
            return 0;
        }
    } while (checkval != 11);
		
    // Combine the 4 digits into a single integer (e.g., 1,2,3,4 becomes 1234)
    return (digits[0] * 1000) + (digits[1] * 100) + (digits[2] * 10) + digits[3];
}
//YEAR getting function//
unsigned int get_year(void)
{
	unsigned int digits[4] = {0,0,0,0};
	int i = 0;
	unsigned int val;
//	WRITE_CMD(0x01);
 // WRITE_CMD(0x80); str("Enter PIN:");
  //WRITE_CMD(0xC0);
	while(i < 4) {
        val = keyvalue();
		//.......Interrrupt triggered.........//
		    if(val == 99){
					flag = 0;
					n = 0;
					WRITE_CMD(0x01);
					return 0;
				}
        // If 'Back' (10) is pressed and we aren't at the first digit
        if (val == 10 && i > 0) {
            i--;
            WRITE_CMD(0x10); // Move cursor left
            WRITE_DATA(' '); // Overwrite asterisk with space
            WRITE_CMD(0x10); // Move cursor left again
            delay_ms(200);   // Debounce
        } 
        // If a valid number 0-9 is pressed
        else if (val < 10) {
            digits[i] = val;
            WRITE_DATA(val+48); // Print asterisk instead of the number
            //delay_ms(70);
			//WRITE_CMD(0x10);
			//WRITE_DATA('*');
			i++;
            delay_ms(200);   // Debounce
        }
    }
    // Combine the 4 digits into a single integer (e.g., 1,2,3,4 becomes 1234)
    return (digits[0] * 1000) + (digits[1] * 100) + (digits[2] * 10) + digits[3];
}
//----------------------------------------------------------
//Iap Memory Managing
//----------------------------------------------------------
void load_schedule(void)
{
	unsigned int *flash_ptr = (unsigned int*)FLASH_SECTOR_14;
	if(flash_ptr[0] != 0xFFFFFFFF){
		on_HOURS = flash_ptr[0];
		on_MINS  = flash_ptr[1];
		off_HOURS = flash_ptr[2];
		off_MINS = flash_ptr[3];
	}
}
//--------------------------------------------
//Iap saving to flash
//--------------------------------------------
void save_schedule(void)
{
	buff[0] = on_HOURS;
	buff[1] = on_MINS;
	buff[2] = off_HOURS;
	buff[3] = off_MINS;
	VICIntEnClr = 0xFFFFFFFF;
	
	//prepare secoter 14//
	pepw[0] = 50;
	pepw[1] = 14;
	pepw[2] = 14;
	iap_tool(pepw,result);
	if(result[0] != 0){ENIT_EN();str("ERROR1"); return;}
	
	//Erase Sector 14//
	pepw[0] = 52;
	pepw[1] = 14;
	pepw[2] = 14;
	pepw[3] = 60000;
	iap_tool(pepw,result);
	if(result[0] != 0){ENIT_EN(); return;}
	
	//prepare sector 14//
	pepw[0] = 50;
	pepw[1] = 14;
	pepw[2] = 14;
	iap_tool(pepw,result);
	if(result[0] != 0){ENIT_EN(); return;}
	
	//Write to sector 14//
	pepw[0] = 51;
	pepw[1] = FLASH_SECTOR_14;
	pepw[2] = (unsigned int)buff;
	pepw[3] = 512;
	pepw[4] = 60000;
	iap_tool(pepw,result);
	if(result[0] != 0){ENIT_EN(); return;}
	
	ENIT_EN();
}

// ---------------------------------------------------------
// RTC INITIALIZATION
// ---------------------------------------------------------
void rtc_int(void)
{
     CCR = 1 << 1;
	 CCR = 1 << 0 | 1<< 4;
	 //DOW = 3;
	//DOM = 10;
    //MONTH = 9;
    //YEAR = 2026; 
 /*if((CCR & 0x01) == 0){
    CCR = 1 << 1;
    SEC = 58;
    MIN = 59;
    HOUR = 12; 
    
	CCR = 1 << 1;
    CCR |= 1 << 4;
	CCR |= 1 << 0;
	DOW = 3;
	DOM = 10;
    MONTH = 9;
    YEAR = 2026; */
	
/*else {
	CCR = 1 << 1;
    PREINT = PREDIGIT;
    PREFRAC = PREFRC;
	CCR = (1 << 0);
    SEC = 58;
    MIN = 59;
    HOUR = 12; 
	//CCR = (1 << 0)|(1 << 4);
    DOW = 0;
    DOM = 10;
    MONTH = 9;
    YEAR = 2026;
	//} */
}

// ---------------------------------------------------------
//...................MAIN RTC LOOP.........................

void rtc_fun(void)
{
    unsigned int sel, f = 0,temp,p1,p2,v,input_val;
    static int lst_sec = -1;
	 static int attempt = 3;
	  static int lst_t0tc = -1;
	  static unsigned int current_pin = 7777;
	//loading schedule from ram to flash//
	  load_schedule();
    				
    IODIR0 |= 1 << 7; // Configure device pin as output
    WRITE_CMD(0x01);
	str("RTC DRIVERS");
	WRITE_CMD(0xc0);
	str("PROJECT");
	delay_ms(1000);
	WRITE_CMD(0x01);


    while(1)
    {
        //.........Interrupt Flag..........
        if(flag == 1){
            delay_ms(100);
			if(n==0){ 
            n = 4;
			}
			flag = 0;
        }
		

        switch(n) {
            case 0: // NORMAL OPERATION
                if(SEC != lst_sec){
                    lst_sec = SEC;
                    
                    WRITE_CMD(0x80);
                    print_2_digits(HOUR); WRITE_DATA(':');
                    print_2_digits(MIN);  WRITE_DATA(':');
                    print_2_digits(SEC);

                    WRITE_CMD(0x80+12);
                    str((const char *)days[DOW]);

                    WRITE_CMD(0xc0);
                    print_2_digits(DOM); WRITE_DATA('/');
                    print_2_digits(MONTH); WRITE_DATA('/');
                    digit(YEAR);

                    // Actuate Hardware
                    if (get_device_state() == 1) IOSET0 = 1 << 7;
                    else IOCLR0 = 1 << 7;

                // Screen Toggle Logic
                if(on_HOURS != off_HOURS) {
                    if(f == 5){
                        dev_dis();
                        f = 0;
                    } else f++;
                }
				}
				 break;

            case 1: // EDIT MENU
                WRITE_CMD(0x01);
                WRITE_CMD(0x80); str("1.RTC 2.SCH");
                WRITE_CMD(0xc0); str("3.More 4.Exit");

                sel = keyvalue();
                delay_ms(200);
                
                if(sel == 1) n = 2;
                else if(sel == 2) n = 3;
                else if(sel == 3) {n = 6;}
				else if(sel == 4) n = 0;
				else if(sel == 0) n = 0;
				else if(sel == 99) {flag = 0; n = 0;}
				WRITE_CMD(0x01);
                break;
				case 2: // EDIT CLOCK
                WRITE_CMD(0x01);
                WRITE_CMD(0x80); str("1.SET:HH(00-23)");
                WRITE_CMD(0xc0); str("2.SET:MM(00-59)");
								v = keyvalue();
								delay_ms(200);
								if(v > 2|v <= 0){n = 1; break;}
								if (v == 99) { flag = 0; n = 0; break; }
																
                switch(v){
                case 1:
                WRITE_CMD(0x01);
                str("SET:HH(00-23)");
                HOUR = getdigits();
                if (n != 2) break;
                break;
																
                case 2:
                WRITE_CMD(0x01); str("SET:MM(00-59)");
                MIN = getdigits();
                if (n != 2) break;
                break;
                }
                str("Saved!"); delay_ms(1000);
                WRITE_CMD(0x01);
                break;

            
            case 3: // EDIT SCHEDULE
                WRITE_CMD(0x01);
                WRITE_CMD(0x80); str("on:HH(00-23)");
                on_HOURS = getdigits();
				        if (n != 3) break;

                WRITE_CMD(0xc0); str("on:MM(00-59)");
                on_MINS = getdigits();
				        if (n != 3) break;

                WRITE_CMD(0x01);
                WRITE_CMD(0x80); str("off:HH(00-23)");
                off_HOURS = getdigits();
				        if (n != 3) break;

                WRITE_CMD(0xc0); str("off:MM(00-59");
                off_MINS = getdigits();
				        if (n != 3) break;
				
			          //save schedule to flash//
			         	save_schedule();
                
                WRITE_CMD(0x01); str("Saved!"); delay_ms(1000);
                n = 0;
                WRITE_CMD(0x01);
                break;
				case 4: // PASSWORD CHECK (NEW)
				temp = get_password();
				if(n != 4) break;
                if (temp == current_pin) {
                    WRITE_CMD(0x01); 
                    WRITE_CMD(0x80); str("Access Granted"); 
                    delay_ms(500);
					attempt = 3;
                    n = 1; // Password correct: Move to the Edit Menu//
					}
					else {
					attempt--;
                    WRITE_CMD(0x01);
                    if(attempt > 0){											
                    WRITE_CMD(0x80); str("Wrong PIN!");
					delay_ms(100);
                    WRITE_CMD(0xC0);str("Attempt's lft:");
                    WRITE_DATA(attempt+48);											
                    delay_ms(1000);
                  }				
                  else {
					WRITE_CMD(0x01);
					WRITE_CMD(0x80);str("System Hang!..");
					//starting the timer//
					T1PC = 0X0;
					T1TC = 0X0;
					T1TCR = 0X0;
					T1PR = 15000000-1;
					T1TCR = 1 << 0;				
					lst_t0tc = -1;
                      n = 5;
					}
			}
			break;	
			case 5:
			   if(T1TC != lst_t0tc){
			     lst_t0tc = T1TC;

		          if(T1TC <= 10){
				WRITE_CMD(0xC0); str("Wait:");
				if(T1TC == 10){
				WRITE_DATA('1');WRITE_DATA('0');
				}
				else {
				WRITE_DATA(T1TC + 48);
				}
				str("'s");
				}
				else {
				T1TCR = 0x0;
				attempt = 3;
				n = 0;
				WRITE_CMD(0x01);
				}
				}		
               break;
			   	case 6: // MORE OPTIONS SUBMENU
                WRITE_CMD(0x01);
                WRITE_CMD(0x80); str("1.Date 2.Pass");
                WRITE_CMD(0xc0); str("3.Back");

                sel = keyvalue();
                delay_ms(200);
                
                if(sel == 1) n = 7;      // Go to Date Edit
                else if(sel == 2) n = 8; // Go to Change Password
                else if(sel == 3) n = 1; // Back to Main Menu
                else if(sel == 99) {flag = 0; n = 0;}
                WRITE_CMD(0x01);
                break;
								
								case 7: // EDIT DATE SUBMENU
                WRITE_CMD(0x01);
                WRITE_CMD(0x80); str("1.DD 2.MM 3.YY");
                WRITE_CMD(0xc0); str("4.DoW 5.Back");
                
                v = keyvalue();
                delay_ms(200);

                if (v == 5) { n = 6; break; } // Exit to More Options Menu
                if (v == 99) { flag = 0; n = 0; break; } // Abort entirely

                switch(v) {
                    case 1:
                        WRITE_CMD(0x01); str("SET:DD(01-31)");
                        input_val = getdigits();
                        // Validate maximum days for the current month/year
                        if (input_val > 0 && input_val <= get_max_days(MONTH, YEAR)) {
                            DOM = input_val;
                        } else {
                            WRITE_CMD(0x01); str("Invalid Day!"); delay_ms(1000);
                        }
                        break;
                    case 2:
                        WRITE_CMD(0x01); str("SET:MM(01-12)");
                        input_val = getdigits();
                        if (input_val > 0 && input_val <= 12) {
                            MONTH = input_val;
                            // Enforce date bounds if switching to shorter month
                            if (DOM > get_max_days(MONTH, YEAR)) DOM = get_max_days(MONTH, YEAR);
                        } else {
                            WRITE_CMD(0x01); str("Invalid Month!"); delay_ms(1000);
                        }
                        break;
                    case 3:
                        WRITE_CMD(0x01); str("SET:YYYY");
                        YEAR = get_year();
                        // Enforce leap year limitations
                        if (DOM > get_max_days(MONTH, YEAR)) DOM = get_max_days(MONTH, YEAR);
                        break;
                    case 4:
                        WRITE_CMD(0x01); str("SET:DoW(0-6)");
                        input_val = keyvalue();
                        if (input_val <= 6) {
                            DOW = input_val;
                        } else {
                            WRITE_CMD(0x01); str("Invalid DoW!"); delay_ms(1000);
                        }
                        break;
                    default:
                        break;
                }
                
                // If the user didn't abort during entry, show success and reload the Date menu
                if (n == 7) {
                    WRITE_CMD(0x01); str("Saved!"); delay_ms(800);
                }
                break;
								
				case 8: // CHANGE PASSWORD LOGIC
				str("New Pin");
				delay_ms(700);
                p1 = get_password();
                if (n != 8) break; // Exit if aborted
				WRITE_CMD(0x01);
               	str("cnf New Pin");
				delay_ms(700);
                p2 = get_password();
                if (n != 8) break; // Exit if aborted

                WRITE_CMD(0x01);
                if (p1 == p2) {
                    current_pin = p1; // Update active PIN
                    WRITE_CMD(0x80); str("PIN Saved!");
                } else {
                    WRITE_CMD(0x80); str("Mismatch!");
                }
                
                delay_ms(1500);
                n = 6; // Return to Submenu
                WRITE_CMD(0x01);
                break;
			   
        }
    }
}
// ---------------------------------------------------------
//Deivc Display.............................................

void dev_dis(void)
{
    WRITE_CMD(0x01);
    WRITE_CMD(0x80); str("ON:");
    print_2_digits(on_HOURS); WRITE_DATA(':');
    print_2_digits(on_MINS);

    WRITE_CMD(0xC0); str("Off:");
    print_2_digits(off_HOURS); WRITE_DATA(':');
    print_2_digits(off_MINS);
	delay_ms(1000);
}
