#include <SPI.h>
const int  SLAVE_SELECT=10; //slave select

void setup() {
    Serial.begin(9600);
    RTC_init();
    //day(1-31), month(1-12), year(0-99), hour(0-23), minute(0-59), second(0-59)
    setDateTime(11,12,13,14,15,16);
}

void loop() {
    Serial.println(getDateTime());
    delay(1000);
}

//=====================================
int RTC_init(){
    pinMode(SLAVE_SELECT,OUTPUT); // chip select
    // start the SPI library:
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work
    //set control register
    digitalWrite(SLAVE_SELECT, LOW);
    SPI.transfer(0x8E);
    SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
    digitalWrite(SLAVE_SELECT, HIGH);
    delay(10);
}

//=====================================
// Methods to set time
// Write Address List:
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

    digitalWrite(SLAVE_SELECT, LOW);
    SPI.transfer(address);
    SPI.transfer(out_val);
    digitalWrite(SLAVE_SELECT, HIGH);
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

void setTime(int h, int mi, int s){
    setHour(h);
    setMinute(mi);
    setSecond(s);
}

void setDate(int d, int mo, int y){
    setDay(d);
    setMonth(mo);
    setYear(y);
}

void setDateTime(int d, int mo, int y, int h, int mi, int s){
    setDay(d);
    setMonth(mo);
    setYear(y);
    setHour(h);
    setMinute(mi);
    setSecond(s);
}

//=====================================
// Methods to get time
// Read Address List:
// 0x00 - Second
// 0x01 - Minute
// 0x02 - Hour
// 0x03 - Weekday
// 0x04 - Day
// 0x05 - Month
// 0x06 - Year
//=====================================

unsigned int getValue(int address){
  digitalWrite(SLAVE_SELECT, LOW);
  SPI.transfer(address);
  unsigned int n = SPI.transfer(0x00);
  digitalWrite(SLAVE_SELECT, HIGH);
  return n;
}

int getSecond(){
  int address = 0x00;
  unsigned int n = getValue(address);
  int a = n & B00001111;
  int b = (n & B01110000)>>4;
  return a+b*10;
}

int getMinute(){
  int address = 0x01;
  unsigned int n = getValue(address);
  int a = n & B00001111;
  int b = (n & B01110000)>>4;
  return a+b*10;
}

int getHour(){
  int address = 0x02;
  unsigned int n = getValue(address);
  int a = n & B00001111;
  int b=(n & B00110000)>>4; //24 hour mode
  if(b==B00000010)
      b=20;
  else if(b==B00000001)
      b=10;
  return a + b;
}

int getWeekday(){
  int address = 0x03;
  unsigned int n = getValue(address);
  int a = n & B00001111;
  int b = (n & B01110000)>>4;
  return a+b*10;
}

int getDay(){
  int address = 0x04;
  unsigned int n = getValue(address);
  int a = n & B00001111;
  int b=(n & B00010000)>>4;
  return a+b*10;
}

int getMonth(){
  int address = 0x05;
  unsigned int n = getValue(address);
  int a = n & B00001111;
  int b=(n & B00010000)>>4;
  return a+b*10;
}

int getYear(){
  int address = 0x06;
  unsigned int n = getValue(address);
  int a = n & B00001111;
  int b=(n & B11110000)>>4;
  return a+b*10;
}

String getDateTime(){
    String temp;

    temp.concat(getDay());
    temp.concat("/") ;
    temp.concat(getMonth());
    temp.concat("/") ;
    temp.concat(getYear());
    temp.concat("     ") ;
    temp.concat(getHour());
    temp.concat(":") ;
    temp.concat(getMinute());
    temp.concat(":") ;
    temp.concat(getSecond());
  return(temp);
}
