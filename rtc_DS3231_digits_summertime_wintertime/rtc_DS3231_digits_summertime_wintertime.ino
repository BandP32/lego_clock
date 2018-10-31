// the clock
// #############
// - used RealTimeClock DS3231
// - extracting/seperating the digits for display
// - automatically set wintertime and summertime
// - Corrects for wintertime and summertime if clock is powered down for a longer period

#include <EEPROM.h>     // EEPROM library for permanently storing summertime state
#include <DS3231.h>     // RealTimeClock DS3231 library from http://www.rinkydinkelectronics.com

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Init a Time-data structure
Time  t;

int hour1;  // hour first digit
int hour2;  // hour second digit
int min1;   // minute first digit
int min2;   // minute second digit
int sec1;   // second first digit
int sec2;   // second second digit
int addr = 0;     // byte memory adress of EEPROM (must be between 0 and 255)
bool summertime;   // 1=summertime, 0=wintertime

void setup()
{
  // Setup Serial connection
  Serial.begin(115200);

  // read summertime variable from the EEPROM
  summertime = EEPROM.read(addr);
  Serial.print("read from EEPROM. summertime state is: ");
  Serial.print(summertime);
  Serial.print(" ");

  // Initialize the rtc object
  rtc.begin();
  
    // The following lines can be uncommented to set the date and time manually
//  rtc.setDOW(MONDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(11, 31, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(31, 10, 2018);   // Set the date to January 1st, 2014


  // checking for wintertime and summertime and if the variable "summertime" is wrongly set correct it + set the time
  t = rtc.getTime();
  if ((t.mon > 10 && t.mon < 2) || ( t.mon == 10 && t.date >= 26 && t.dow>=1 && t.dow<=6) || ( t.mon == 3 && t.date <= 24) || ( t.mon == 3 && t.date <= 30 && t.date >= 25 && !(t.dow == 7))){
    Serial.print("It's wintertime! ");
    if (summertime == 1){
      Serial.print("But the clock is set to summertime :-( ");

      rtc.setTime(t.hour-1, t.min, t.sec);     // Set the time
      Serial.print("Setting time 1 hour back");
      summertime = 0;   // switch to wintertime
      EEPROM.write(addr, summertime);   // write summertime to EEPROM (variable is permanently saved even without power)
    
    }
    }

//  if ((t.mon > 2 && t.mon < 10) || ( t.mon == 3 && t.date >= 26 && t.dow>=1 && t.dow<=6) || ( t.mon == 3 && t.date == 25 && t.dow == 7 ) || ( t.mon == 3 && t.date == 31 && t.dow == 7)) || (t.mon == 10 && t.date <= 24) || (t.mon == 10 && t.date >=25 && !(t.dow == 7))){
  if (!((t.mon > 10 && t.mon < 2) || ( t.mon == 10 && t.date >= 26 && t.dow>=1 && t.dow<=6) || ( t.mon == 3 && t.date <= 24) || ( t.mon == 3 && t.date <= 30 && t.date >= 25 && !(t.dow == 7)))){

    Serial.print("It's summertime! ");
    if (summertime == 0){
      Serial.print("But the clock is set to wintertime :-( ");

      // edge case: 23 hours
      if (t.hour == 23){                  
        rtc.setTime(0, t.min, t.sec);     // -> set hour to 0
        int date_compare = t.date + 1; 
        rtc.setDate(t.date + 1, t.mon, t.year);          // ... and set the date + 1 day, hoping that it's not the last day of the month...
        if (t.date == date_compare){      // if date could not be set +1day
          int mon_compare = t.mon + 1;
          rtc.setDate(1, t.mon+1, t.year);    // setting the day to 0 and the month + 1
          if (t.mon == mon_compare){      // if month could not be set +1 month
            rtc.setDate(1, 1, t.year+1);   // Happy New Year!
          }
          
        }
      }
      rtc.setTime(t.hour+1, t.min, t.sec);     // Set the time
      Serial.print("Setting time 1 hour forth");
      summertime = 1;   // switch to summertime
      EEPROM.write(addr, summertime);   // write summertime to EEPROM (variable is permanently saved even without power)
    }
  }
}

void loop()
{
  // Get data from the DS3231
  t = rtc.getTime();

  // check if 
  if (rtc.getMonthStr()=="October" && t.date >= 25 && t.dow ==7 && t.hour == 3 && ((summertime==1))){
    Serial.print("It's October, more or equal than the 25. and Sunday: You must change the clock at 3 o'clock to 2 ");
    rtc.setTime(2, 0, 0);     // Set the time to 02:00:00 (24hr format)
    summertime = 0;   // switch to wintertime
    EEPROM.write(addr, summertime);   // write summertime to EEPROM (saves variable also when power is lost)
    
  }
  
  // Send date over serial connection
//  Serial.print("Today is the ");
//  Serial.print(t.date, DEC);
//  Serial.print(". day of ");
//  Serial.print(rtc.getMonthStr());
//  Serial.print(" in the year ");
//  Serial.print(t.year, DEC);
//  Serial.println(".");
  
  // Send Day-of-Week and time
//  Serial.print("It is the ");
//  Serial.print(t.dow, DEC);
//  Serial.print(". day of the week (counting monday as the 1th), and it has passed ");
  

// Selecting the hours and minutes to variables:
//(x % 10) gives you the last digit. x = x / 10 removes the last digit, 
//so (x / 10) % 10 gives you the second-to-last digit, and so on...
  if (t.hour<10){       // adds the 0 to the first digit if it is before 10
    hour1=0;
    hour2=t.hour;
  }
  else
    hour1=t.hour/10;    // "selects" the first number
    hour2=t.hour % 10;  // "selects" the second number

  if (t.min<10){        // adds 0 to the first digit if minutes < 10
    min1=0;             // first digit
    min2=t.min;         // second digit
  }
  else
    min1=t.min/10;      // "selects" the first number
    min2=t.min % 10;    // "selects" the second number

    
  if (t.sec<10){        // adds 0 to the first digit if seconds are < 10
    sec1=0;             // first digit
    sec2=t.sec;         // second digit
  }
  else
    sec1=t.sec/10;      // "selects" the first number
    sec2=t.sec % 10;    // "selects" the first number
    

//  Serial.print(t.hour, DEC);
  Serial.print(hour1);
  Serial.print(hour2);
  Serial.print(":");
//  Serial.print(t.min, DEC);
  Serial.print(min1);
  Serial.print(min2);
  Serial.print(":");
//  Serial.print(t.sec, DEC);
  Serial.print(sec1);
  Serial.print(sec2);

  // Send a divider for readability
  Serial.println("  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -");
  
  // Wait one second before repeating :)
  delay (1000);
}
