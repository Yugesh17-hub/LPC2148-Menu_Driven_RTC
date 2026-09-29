# LPC2148-Menu_Driven_RTC
A Menu Driven RTC Configuration and Device Automation System Built On The LPC2148. Adding Features of 4x4 Keypad On Board Navigation,16x2 LCD UI and IAP Flash Memory Storage.
## System Interface & Menu Workflow

The system features a secure, interactive menu navigated via a 4x4 matrix keypad and displayed on a 16x2 LCD. 
1. Normal Display Mode
The LCD continuously alternates every 5 seconds between showing the live RTC clock (Time, Date, Day, Year) and the scheduled device ON/OFF timings.
![Live RTC Clock]
![Device ON/OFF Timings]

2. Secure Access
Pressing the external interrupt switch pauses the system and prompts the user to "Enter PIN" before allowing access to the configuration settings.
![Enter PIN Prompt]

3. Main Menu Navigation
Upon successful authentication, the main menu displays four options: `1.RTC 2.SCH 3.More 4.Exit`.
![Main Menu]

* 1. Edit RTC:** Prompts the user to set the current clock hours (`HH`) and minutes (`MM`).
![Set Hours]
![Set Minutes]

* 2. Edit Schedule:** Prompts the user to set the automated device `ON` and `OFF` times.

* 3. More Options:** Opens an advanced submenu displaying `1.Date 2.Pass 3.Back`.
![More Options Menu]

    * Date (Option 1):** Allows configuration of the Day (`DD`), Month (`MM`), Year (`YYYY`), and Day of the Week (`DoW`).
    ![Date Configuration]

    * Password (Option 2):** Prompts the user to enter a "New PIN" to securely update the system password.
    ![New PIN Configuration]

Project Demonstration
[Watch the full hardware demonstration video here]
