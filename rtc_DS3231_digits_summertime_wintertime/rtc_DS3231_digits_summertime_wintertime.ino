// preparing the clock
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

static const uint8_t dim[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };     // last days of month of a normal year
static const uint8_t dim_s[] = { 31,29,31,30,31,30,31,31,30,31,30,31 };   // last days of month of a leap-year
int last_day;     // empty variable for the last day of month

void setup()
{
  // Setup Serial connection
  Serial.begin(115200);

  // read summertime variable from the EEPROM
  summertime = EEPROM.read(addr);
  // summertime = 1;    // switch summertime manually: 1 = summertime; 0 = wintertime
  Serial.print("read from EEPROM. summertime state is: ");
  Serial.print(summertime);
  Serial.print(" ");

  // Initialize the rtc object
  rtc.begin();
  
    // The following lines can be uncommented to set the date and time manually
  //rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(3, 58, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(30, 3, 2018);   // Set the date (d, m, y)


  // checking for wintertime and summertime and if the variable "summertime" is wrongly set correct it + set the time + set the date correctly
  t = rtc.getTime();
  if ((t.mon > 10 || t.mon < 2) || ( t.mon == 10 && t.date <= 25) || ( t.mon == 10 && t.date >= 25 && t.dow == 7 && t.hour >= 3) || 
    ( t.mon == 10 && t.date >= 26 && t.dow<=1 ) || ( t.mon == 10 && t.date >= 27 && t.dow<=2 ) || 
    ( t.mon == 10 && t.date >= 28 && t.dow<=3 ) || ( t.mon == 10 && t.date >= 29 && t.dow<=4 ) || 
    ( t.mon == 10 && t.date >= 30 && t.dow<=5 ) || 
    ( t.mon == 3 && t.date <= 24) || ( t.mon == 3 && t.date <= 30 && t.date >= 25 && !(t.dow == 7))){
    Serial.print("It's wintertime! ");
    if (summertime == 1){
      
      Serial.print("But the clock is set to summertime :-( ");
      Serial.print("Setting time 1 hour back");
      
      // edge case: 00 hours
      if (t.hour == 0){
        rtc.setTime(23, t.min, t.sec);     // -> set hour to 23                  
        int date_compare = t.date; 
        rtc.setDate(t.date - 1, t.mon, t.year);          // ... and set the date - 1 day, hoping that it's not the first day of the month...
        if (t.date == date_compare){      // if date could not be set -1day, because it was the first day of month
          int mon_compare = t.mon;
          int mon_minus1 = t.mon-2;   // -2 because the arrays dim + dim_s begin to count at number 0

          if (t.mon == 1) {   // else => January the first in the midnight hour, so setting the clock 1 hour back means:
            rtc.setDate(31, 12, t.year-1);   // Happy Newyearseve again!
            }
          
          else if ((t.mon -1) == 2){ // if the month is February then check if we are in a leap-year
              // check for active leap-year starting from 2016 until 2400 ... should suffice ;-)
              for (int y = 2016; y < 2400; y = y + 4){    // loop every 4 years
                if (t.year == y){          
     
                  last_day = dim_s[mon_minus1];     // selecting leap-year-array entry for last day of month
                  }
                else {
                  last_day = dim[mon_minus1];     // last day of a normal year february
                  }
                }
            }
          else if (((t.mon -1) == 1) || ((t.mon - 1) > 2) && ((t.mon - 1) < 13)){
            last_day = dim[mon_minus1];     // selecting the normal-year array for the last day of the month
            }
          rtc.setDate(last_day, t.mon-1, t.year);   // 
          }
        }
      // end of edge case: 00 hours
        
      else{
        rtc.setTime(t.hour-1, t.min, t.sec);     // Simply set the clock 1 hour back
        }
        
      
      summertime = 0;   // switch to wintertime
      EEPROM.write(addr, summertime);   // write summertime variable to EEPROM (variable is permanently saved even without power)
      }
    }

  if (!((t.mon > 10 || t.mon < 2) || ( t.mon == 10 && t.date <= 25) || ( t.mon == 10 && t.date >= 25 && t.dow == 7 && t.hour >= 3) || 
    ( t.mon == 10 && t.date >= 26 && t.dow<=1 ) || ( t.mon == 10 && t.date >= 27 && t.dow<=2 ) || 
    ( t.mon == 10 && t.date >= 28 && t.dow<=3 ) || ( t.mon == 10 && t.date >= 29 && t.dow<=4 ) || 
    ( t.mon == 10 && t.date >= 30 && t.dow<=5 ) || 
    ( t.mon == 3 && t.date <= 24) || ( t.mon == 3 && t.date <= 30 && t.date >= 25 && !(t.dow == 7))))
    {
//  if (!((t.mon > 10 || t.mon < 2) || ( t.mon == 10 && t.date >= 26 && t.dow>=1 && t.dow<=6) || ( t.mon == 3 && t.date <= 24) || ( t.mon == 3 && t.date <= 30 && t.date >= 25 && !(t.dow == 7)))){

    Serial.print("It's summertime! ");
    if (summertime == 0){
      Serial.print("But the clock is set to wintertime :-( ");
      Serial.print("Setting time +1 hour");

      // edge case: 23 hours
      if (t.hour == 23){                  
        rtc.setTime(0, t.min, t.sec);     // -> set hour to 0
        int date_compare = t.date; 
        rtc.setDate(t.date + 1, t.mon, t.year);          // ... and set the date + 1 day, hoping that it's not the last day of the month...
        if (t.date == date_compare){      // if date could not be set +1day
          int mon_compare = t.mon;
          rtc.setDate(1, t.mon+1, t.year);    // setting the day to 0 and the month + 1
        }
      }
      // end of edge case: 23 hours

      else {
        rtc.setTime(t.hour+1, t.min, t.sec);     // Set the time
      }
      
      summertime = 1;   // switch to summertime
      EEPROM.write(addr, summertime);   // write summertime to EEPROM (variable is permanently saved even without power)
      Serial.print("switching to summertime and saving to EEPROM");
      }
    }
  }

void loop()
{
  // Get data from the DS3231
  t = rtc.getTime();

  // Set to wintertime
  if (t.mon == 10 && t.date >= 25 && t.dow == 7 && t.hour >= 3 && summertime == 1){
    Serial.print("It's October, more or equal than the 25. and Sunday: You must change the clock at 3 o'clock to 2 ");
    rtc.setTime(t.hour - 1, t.min, t.sec);     // Set the time to 02:00:00 (24hr format)
    summertime = 0;   // switch to wintertime
    EEPROM.write(addr, summertime);   // write summertime to EEPROM
  }

  // Set to summertime
  if (rtc.getMonthStr()=="March" && t.date >= 25 && t.dow == 7 && t.hour >= 2 && summertime == 0){
    Serial.print("It's October, more or equal than the 25. and Sunday and 2 o clock: You must change the clock to +1 hour to 3 ");
    rtc.setTime(t.hour + 1, t.min, t.sec);     // Set the time to 03:00:00 (24hr format)
    summertime = 1;   // switch to summertime
    EEPROM.write(addr, summertime);   // write summertime to EEPROM
  }
  
  // Send date over serial connection
  Serial.print("Today is the ");
  Serial.print(t.date, DEC);
  Serial.print(". day of ");
  Serial.print(rtc.getMonthStr());
  Serial.print(" in the year ");
  Serial.print(t.year, DEC);
  Serial.println(".");
  
  // Send Day-of-Week and time
  Serial.print("It is the ");
  Serial.print(t.dow, DEC);
  Serial.print(". day of the week (counting monday as the 1th), and it has passed ");
  

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
