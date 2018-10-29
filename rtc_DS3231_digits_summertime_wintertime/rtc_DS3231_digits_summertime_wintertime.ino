// the clock
// DS3231
// - extracting the digits
// - automatically set wintertime and summertime
// - Correct for wintertime and summertime if clock is powered down for a longer period

#include <EEPROM.h>     // for permanently storing summertime
#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Init a Time-data structure
Time  t;

int hour1;
int hour2;
int min1;
int min2;
int sec1;
int sec2;
int addr = 0;
int summertime;   // 1=summertime, 2=wintertime

void setup()
{
  // Setup Serial connection
  Serial.begin(115200);
  // Uncomment the next line if you are using an Arduino Leonardo
  //while (!Serial) {}

  // Initialize the rtc object
  rtc.begin();
  
    // The following lines can be uncommented to set the date and time manually
//  rtc.setDOW(MONDAY);     // Set Day-of-Week to SUNDAY
//  rtc.setTime(22, 1, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(29, 10, 2018);   // Set the date to January 1st, 2014


  // checking for wintertime and summertime and if the variable "summertime" is wrongly set correct it + set the time
  t = rtc.getTime();
  if ((t.mon > 10 && t.mon < 2) || ( t.mon == 10 && t.date >= 26 && t.dow>=1 && t.dow<=6) || ( t.mon == 3 && t.date <= 24) || ( t.mon == 3 && t.date <= 30 && t.date >= 25 && !(t.dow == 7))){
    Serial.print("It's wintertime! ");
    if (summertime == 1){
      Serial.print("But the clock is set to summertime :-( ");

      rtc.setTime(t.hour-1, t.min, t.sec);     // Set the time to 12:00:00 (24hr format)
      Serial.print("Setting time 1 hour back");
      summertime = 2;   // switch to wintertime
      EEPROM.write(addr, summertime);   // write summertime to EEPROM (saves variable also when power is lost)
    
    }
    }

//  if ((t.mon > 2 && t.mon < 10) || ( t.mon == 3 && t.date >= 26 && t.dow>=1 && t.dow<=6) || ( t.mon == 3 && t.date == 25 && t.dow == 7 ) || ( t.mon == 3 && t.date == 31 && t.dow == 7)) || (t.mon == 10 && t.date <= 24) || (t.mon == 10 && t.date >=25 && !(t.dow == 7))){
  if (!((t.mon > 10 && t.mon < 2) || ( t.mon == 10 && t.date >= 26 && t.dow>=1 && t.dow<=6) || ( t.mon == 3 && t.date <= 24) || ( t.mon == 3 && t.date <= 30 && t.date >= 25 && !(t.dow == 7)))){

    Serial.print("It's summertime! ");
    if (summertime == 2){
      Serial.print("But the clock is set to wintertime :-( ");

      rtc.setTime(t.hour+1, t.min, t.sec);     // Set the time to 12:00:00 (24hr format)
      Serial.print("Setting time 1 hour forth");
      summertime = 1;   // switch to summertime
      EEPROM.write(addr, summertime);   // write summertime to EEPROM (saves variable also when power is lost)
    
    }
  }
}

void loop()
{
  // Get data from the DS3231
  t = rtc.getTime();

  if (rtc.getMonthStr()=="October" && t.date >= 25 && t.dow ==7 && t.hour == 3 && ((summertime==1) || (summertime==0))){
    Serial.print("It's October, more or equal than the 25. and Sunday: You must change the clock at 3 o'clock to 2 ");
    rtc.setTime(2, 0, 0);     // Set the time to 02:00:00 (24hr format)
    summertime = 2;   // switch to wintertime
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
  

// Separating the hours and minutes to digits:
//(x % 10) gives you the last digit. x = x / 10 removes the last digit, 
//so (x / 10) % 10 gives you the second-to-last digit, and so on...
  if (t.hour<10){
    hour1=0;
    hour2=t.hour;
  }
  else
    hour1=t.hour/10;
    hour2=t.hour % 10;

  if (t.min<10){
    min1=0;
    min2=t.min;
  }
  else
    min1=t.min/10;
    min2=t.min % 10;

    
  if (t.sec<10){
    sec1=0;
    sec2=t.sec;
  }
  else
    sec1=t.sec/10;
    sec2=t.sec % 10;
    

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
//  Serial.println(" second(s) since midnight.");

  // Send a divider for readability
  Serial.println("  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -");
  
  // Wait one second before repeating :)
  delay (1000);
}
