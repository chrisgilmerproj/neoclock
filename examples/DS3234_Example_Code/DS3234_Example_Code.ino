#include <SPI.h>
const int  cs=10; //chip select

void setup() {
  Serial.begin(9600);
  RTC_init();
  //day(1-31), month(1-12), year(0-99), hour(0-23), minute(0-59), second(0-59)
  SetTimeDate(11,12,13,14,15,16);
}

void loop() {
  Serial.println(ReadTimeDate());
  delay(1000);
}
//=====================================
int RTC_init(){
      pinMode(cs,OUTPUT); // chip select
      // start the SPI library:
      SPI.begin();
      SPI.setBitOrder(MSBFIRST);
      SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work
      //set control register
      digitalWrite(cs, LOW);
      SPI.transfer(0x8E);
      SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
      digitalWrite(cs, HIGH);
      delay(10);
}
//=====================================
// Methods to set time
// 0x80 - Second
// 0x81 - Minute
// 0x82 - Hour
// 0x83 - Weekday
// 0x84 - Day
// 0x85 - Month
// 0x86 - Year
//=====================================

void setValue(int address, int val){
    // Convert the values to hex
    int b = val/10;   // 10's spot
    int a = val-b*10; //  1's spot

    // For hour
    if (address == 0x82) {
      if (b==2) {
          b=B00000010;
      }
      else if (b==1) {
          b=B00000001;
      }
    }

    // Write the hex value back
    int out_val = a+(b<<4);

    digitalWrite(cs, LOW);
    SPI.transfer(address);
    SPI.transfer(out_val);
    digitalWrite(cs, HIGH);
}

void setSecond(int val){
    int address = 0x80;
    setValue(address, val);
}

void setMinute(int val){
    int address = 0x81;
    setValue(address, val);
}

void setWeekday(int val){
    int address = 0x82;
    setValue(address, val);
}

void setHour(int val){
    int address = 0x82;
    setValue(address, val);
}

void setDay(int val){
    int address = 0x84;
    setValue(address, val);
}

void setMonth(int val){
    int address = 0x85;
    setValue(address, val);
}

void setYear(int val){
    int address = 0x86;
    setValue(address, val);
}

void SetTime(int h, int mi, int s){
    setHour(h);
    setMinute(mi);
    setSecond(s);
}

void SetDate(int d, int mo, int y){
    setDay(d);
    setMonth(mo);
    setYear(y);
}

void SetTimeDate(int d, int mo, int y, int h, int mi, int s){
    setDay(d);
    setMonth(mo);
    setYear(y);
    setHour(h);
    setMinute(mi);
    setSecond(s);
}

//=====================================
String ReadTimeDate(){
    String temp;
    int TimeDate [7]; //second,minute,hour,weekday,day,month,year
    for(int i=0; i<=6;i++){
        // Skip Weekday
        if(i==3){
            i++;
        }
        digitalWrite(cs, LOW);
        SPI.transfer(i+0x00);
        unsigned int n = SPI.transfer(0x00);
        digitalWrite(cs, HIGH);
        int a=n & B00001111;
        if(i==2){
            int b=(n & B00110000)>>4; //24 hour mode
            if(b==B00000010)
                b=20;
            else if(b==B00000001)
                b=10;
            TimeDate[i]=a+b;
        }
        else if(i==4){
            int b=(n & B00110000)>>4;
            TimeDate[i]=a+b*10;
        }
        else if(i==5){
            int b=(n & B00010000)>>4;
            TimeDate[i]=a+b*10;
        }
        else if(i==6){
            int b=(n & B11110000)>>4;
            TimeDate[i]=a+b*10;
        }
        else{
            int b=(n & B01110000)>>4;
            TimeDate[i]=a+b*10;
            }
    }
    temp.concat(TimeDate[4]);
    temp.concat("/") ;
    temp.concat(TimeDate[5]);
    temp.concat("/") ;
    temp.concat(TimeDate[6]);
    temp.concat("     ") ;
    temp.concat(TimeDate[2]);
    temp.concat(":") ;
    temp.concat(TimeDate[1]);
    temp.concat(":") ;
    temp.concat(TimeDate[0]);
  return(temp);
}
