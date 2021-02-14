#include <MsTimer2.h>

#include <Key.h>
#include <Keypad.h>

#include "Wire.h"

#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define sw_MANU   PF0
#define sw_SEMI   PF1
#define sw_AUTO   PF2
#define sw_RUND   PF3
#define sw_JIG    PF4
#define sw_POS    PF5

#define sensor_UPDN   PA4
#define sensor_LEFT   PA5
#define sensor_RIGH   PA6
#define sw_FOOT   PA7

#define sw_UPDN   PD2
#define sw_RIGH   PD3
#define sw_LEFT   PD4
#define AC     PB6

Adafruit_7segment matrix = Adafruit_7segment();
Adafruit_7segment matrix1 = Adafruit_7segment();

const byte numRows = 7;
const byte numCols = 2;

char keymap[numRows][numCols]={
  {'1', 'A'},
  {'2', 'B'},
  {'3', 'C'},
  {'4', 'D'},
  {'5', 'E'},
  {'6', 'F'},
  {'7', 'G'}
};

byte rowPins[numRows] = {2,3,4,5,6,7,8};
byte colPins[numCols] = {12,13};

Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

int mode;                        // 0:수동, 1:반자동, 2: 자동
int jig;                         // 0 자동지그 OFF, 1:자동지그 ON
int start_pos;                   // 0:전인쇄, 1:후인쇄 모드 저장 변수
int shape;                       // 0:곡면, 1:평면
int startAddr;                   // 0:정지, 1:작업 개시
int option = 0;       // Jig 옵션 설정 모드 확인 변수
unsigned int FND, CT, optionTime;
unsigned long time_count = 30000;
unsigned long finTime = millis();

boolean curFoot = LOW;
boolean lastFoot = LOW;
boolean footFlag = false;

void btn_Update();

//--------------DS12C887------------------

const int dscs=43, dsas=52, dsrw=42, dsds=44, dsirq=51;
//define bidirectional address-data bus(양방향 주소 버스 정의)

const int ad[8]={28,29,30,31,32,33,34,35};  //등록 A B C D 주소

#define regA 0x0A
#define regB 0x0B
#define regC 0x0C
#define regD 0x0D

//DS12C887 함수 선언
void bus2out();
void bus2in();
void write_ds(byte add,byte date);
void set_count();
void set_time();
void set_optionTime();
byte read_ds(byte add);
int i; // DS12C887 Bit Address 지정용

void setup() {
  pinMode(37, INPUT);  //footSW         sw_FOOT   PA7
  pinMode(40, INPUT);  //updownSensor   sensor_UPDN   PA4
  pinMode(39, INPUT);  //leftSensor     sensor_LEFT   PA5
  pinMode(38, INPUT);  //rightSensor    sensor_RIGH   PA6

  pinMode(25, OUTPUT);  //Buz 25, PD7

  pinMode(20, OUTPUT);  //업다운 Sol
  pinMode(21, OUTPUT);  //RIGHT Sol
  pinMode(22, OUTPUT);  //LEFT Sol
  pinMode(15, OUTPUT);  //AC Output /PB7

  pinMode(45, OUTPUT);  //manual_led
  pinMode(46, OUTPUT);  //harf_led
  pinMode(47, OUTPUT);  //auto_led
  pinMode(49, OUTPUT);  //jig_led
  pinMode(50, OUTPUT);  //pos_led
  pinMode(48, OUTPUT);  //shape_led

  pinMode(dscs, OUTPUT);
  pinMode(dsas, OUTPUT);
  pinMode(dsrw, OUTPUT);
  pinMode(dsds, OUTPUT);
  pinMode(dsirq, OUTPUT);
  
  //DS12C887   칩 초기화 최초 1회 실행 할 것
  // write_ds(regA,0x20);      // 수정 발진기를 켜고 클럭을 계속 작동 시키려면 A 레지스터를 제어하십시오.
  // write_ds(regB,0x26);        //24 시간 모드, 데이터 바이너리 모드를 설정하고 알람 인터럽트를 활성화하십시오
  // write_ds(0xE2, 0); delay(10);
  // write_ds(0xE3, 0); delay(10);
  // write_ds(0xE4, 0); delay(10);
  // write_ds(0xE5, 0); delay(10);
  // write_ds(0xE6, 0); delay(10);
  // write_ds(0xE7, 0); delay(10);

  delay(100);
  
  byte hiByte = read_ds(0xE0); // 카운터 read(주소)
  byte loByte = read_ds(0xE1); // 카운터 read(주소)
  FND = word(hiByte, loByte);
  
  byte ct_hiByte = read_ds(0xE2); // 카운터 read(주소)
  byte ct_loByte = read_ds(0xE3); // 카운터 read(주소)
  CT = word(ct_hiByte, ct_loByte);

  byte option_hiByte = read_ds(0xEA); // 카운터 read(주소)
  byte option_loByte = read_ds(0xEB); // 카운터 read(주소)
  optionTime = word(option_hiByte, option_loByte);

  mode = read_ds(0xE4);
  switch (mode) { 
    case 0 : bitSet (PORTF, 0); bitClear (PORTF, 1); bitClear (PORTF, 2); break;
    case 1 : bitSet (PORTF, 1); bitClear (PORTF, 0); bitClear (PORTF, 2); break;
    case 2 : bitSet (PORTF, 2); bitClear (PORTF, 0); bitClear (PORTF, 1); break;
    default : break;
  }
  jig = read_ds(0xE5);
  switch (jig) { 
    case 0 : bitClear (PORTF, 4); break;
    case 1 :   bitSet (PORTF, 4); break;
    default : break;
  }
  start_pos = read_ds(0xE6);
  switch (start_pos) { 
    case 0 : bitClear (PORTF, 5); break;
    case 1 :   bitSet (PORTF, 5); break;
    default : break;
  }
  shape = read_ds(0xE7);
  switch (shape) { 
    case 0 : bitClear (PORTF, 3); break;
    case 1 :   bitSet (PORTF, 3); break;
    default : break;
  }

  Wire.begin();
  matrix.begin(0x70);
  matrix1.begin(0x71);

  matrix.print(88888, DEC);
  matrix1.print(88888, DEC);
  matrix.writeDisplay();
  matrix1.writeDisplay();

  matrix.blinkRate(1);
  matrix1.blinkRate(1);
  delay(800);
  matrix.blinkRate(0);
  matrix1.blinkRate(0);
  delay(100);
    
  matrix.print(FND, DEC); 
  matrix.writeDisplay();
  boolean drawDots = true;
  matrix1.print(0, DEC); 
  matrix1.writeDisplay();
  matrix1.writeDigitNum(1, (CT / 1000) % 10, drawDots);
  matrix1.writeDigitNum(2, (CT / 100) % 10);
  matrix1.writeDisplay(); 
  
  MsTimer2::set(100, timer_ISR); // 100ms period
  MsTimer2::start();
}

void sw_manu(){if (!(PINF&(1<<sw_MANU))) bitSet (PORTF, 0); bitClear (PORTF, 1); bitClear (PORTF, 2); write_ds(0xE4, 0);}
void sw_semi(){if (!(PINF&(1<<sw_SEMI))) bitSet (PORTF, 1); bitClear (PORTF, 0); bitClear (PORTF, 2); write_ds(0xE4, 1);}
void sw_auto(){if (!(PINF&(1<<sw_AUTO))) bitSet (PORTF, 2); bitClear (PORTF, 0); bitClear (PORTF, 1); write_ds(0xE4, 2);}
void sw_rund(){if (!(PINF&(1<<sw_RUND))) {bitSet (PORTF, 3); write_ds(0xE7, 1);} else {bitClear (PORTF, 3); write_ds(0xE7, 0);}}
void sw_jig() {if (!(PINF&(1<<sw_JIG)))  {bitSet (PORTF, 4); write_ds(0xE5, 1);} else {bitClear (PORTF, 4); write_ds(0xE5, 0);}}
void sw_pos() {if (!(PINF&(1<<sw_POS)))  {bitSet (PORTF, 5); write_ds(0xE6, 1);} else {bitClear (PORTF, 5); write_ds(0xE6, 0);}}

void updn_L() {  bitSet (PORTD, 2);}
void updn_H() {bitClear (PORTD, 2);}
void righ_H() {  bitSet (PORTD, 3);}
void righ_L() {bitClear (PORTD, 3);}
void left_H() {  bitSet (PORTD, 4);}
void left_L() {bitClear (PORTD, 4);}
void ac_H()   {  bitSet (PORTB, 7);}
void ac_L()   {bitClear (PORTB, 7);}
//================================================= LOOP

unsigned int beepOnTime = 0;
boolean beepFlag;

void timer_ISR() {    
  
  if (time_count <=  optionTime) {bitSet(PORTB, 7); time_count += 100;} else {bitClear(PORTB, 7); time_count = 30000; }
  
  curFoot = bitRead(PINA, PINA7); // 풋스위치 감지
  if ( lastFoot == LOW && curFoot == HIGH ) { footFlag =! footFlag; }
  lastFoot = curFoot;
  
  bitClear(PORTD, 7); // beep 해제
  
}

void beep() {
  bitSet(PORTD, 7);
}

void loop() {

  int Mode;
  boolean drawDots = true; 
  matrix.print(FND, DEC); 
  matrix.writeDisplay();
  char btn = myKeypad.getKey();
  if (btn != NO_KEY) beep();      

  if (footFlag) { // 반자동 모드 Start
    if (PINF&(1<<sw_MANU)) {footFlag = false; Mode = 0; }
    if ( (PINF&(1<<sw_SEMI)) && (!(PINF&(1<<sw_POS))) && (!(PINF&(1<<sw_RUND))) ) Mode = 1;   // 평면 전인쇄 (반자동)
    if ( (PINF&(1<<sw_AUTO)) && (!(PINF&(1<<sw_POS))) && (!(PINF&(1<<sw_RUND))) ) Mode = 2;   // 평면 전인쇄 (자동)
    if ( (PINF&(1<<sw_SEMI)) && (PINF&(1<<sw_POS))    && (!(PINF&(1<<sw_RUND))) ) Mode = 3;   // 평면 후인쇄 (반자동)
    if ( (PINF&(1<<sw_AUTO)) && (PINF&(1<<sw_POS))    && (!(PINF&(1<<sw_RUND))) ) Mode = 4;   // 평면 후인쇄 (자동)
    
    if ( (PINF&(1<<sw_SEMI)) && (!(PINF&(1<<sw_POS))) && (PINF&(1<<sw_RUND)) ) Mode = 30;     // 곡면 전인쇄 (반자동)
    if ( (PINF&(1<<sw_AUTO)) && (!(PINF&(1<<sw_POS))) && (PINF&(1<<sw_RUND)) ) Mode = 40;     // 곡면 전인쇄 (자동)
    if ( (PINF&(1<<sw_SEMI)) && (PINF&(1<<sw_POS))    && (PINF&(1<<sw_RUND)) ) Mode = 10;     // 곡면 후인쇄 (반자동)
    if ( (PINF&(1<<sw_AUTO)) && (PINF&(1<<sw_POS))    && (PINF&(1<<sw_RUND)) ) Mode = 20;     // 곡면 후인쇄 (자동)

    switch(Mode) {
      case 1:
        if (PINA&(1<<sensor_RIGH)) {righ_L(); left_H(); footFlag = false; goto FAULT;} 
        if (PINF&(1<<sw_JIG)) {time_count = 20000; }// 진공 사용 여부   
        while (!(PINA&(1<<sensor_RIGH)))  { left_L(); righ_H();   }         
        while (!(PINA&(1<<sensor_UPDN)))  { updn_L();  if (!(footFlag)) { updn_H(); goto FAULT;} }
        footFlag =! footFlag;
        while (!(PINA&(1<<sensor_LEFT)))  { righ_L(); left_H();  } 
        while (PINA&(1<<sensor_UPDN))     { updn_H();  }        
        FND++;  set_count();
      break;

      case 2:
        if (PINA&(1<<sensor_RIGH)) {righ_L(); left_H(); footFlag = false; goto FAULT;} 
        if ((millis() - finTime) > CT - 20000) {
        if (PINF&(1<<sw_JIG)) {time_count = 20000; }// 진공 사용 여부    
        while (!(PINA&(1<<sensor_RIGH)))  { left_L(); righ_H();    }         
        while (!(PINA&(1<<sensor_UPDN)))  { updn_L();    if (!(footFlag)) { updn_H(); goto FAULT;} }
        while (!(PINA&(1<<sensor_LEFT)))  { righ_L(); left_H();    } 
        while (PINA&(1<<sensor_UPDN))     { updn_H();   }   
        finTime = millis(); FND++;  set_count();}
      break;
      
      case 3:
        if (PINA&(1<<sensor_LEFT)) {left_L(); righ_H(); footFlag = false; goto FAULT;} 
        if (PINF&(1<<sw_JIG)) {time_count = 20000; }// 진공 사용 여부      
        while (!(PINA&(1<<sensor_UPDN)))  { updn_L();    if (!(footFlag)) { updn_H(); goto FAULT;} }
        footFlag =! footFlag;
        while (!(PINA&(1<<sensor_LEFT)))  { righ_L(); left_H();      }
        while (PINA&(1<<sensor_UPDN))     { updn_H();             }
        while (!(PINA&(1<<sensor_RIGH)))  { left_L(); righ_H();      }
        FND++;  set_count();
      break;
      
      case 4:
        if (PINA&(1<<sensor_LEFT)) {left_L(); righ_H(); footFlag = false; goto FAULT;} 
        if ((millis() - finTime) > CT - 20000) {
        if (PINF&(1<<sw_JIG)) {time_count = 20000; }// 진공 사용 여부      
        while (!(PINA&(1<<sensor_UPDN)))  { updn_L();    if (!(footFlag)) { updn_H(); goto FAULT;} }        
        while (!(PINA&(1<<sensor_LEFT)))  { righ_L(); left_H();        }
        while (PINA&(1<<sensor_UPDN))     { updn_H();                }
        while (!(PINA&(1<<sensor_RIGH)))  { left_L(); righ_H();       }
        finTime = millis(); FND++;  set_count();}
      break;

      case 10:
        if (PINA&(1<<sensor_RIGH)) {righ_L(); left_H(); footFlag = false; goto FAULT;} 
        if (PINF&(1<<sw_JIG)) {time_count = 20000; }// 진공 사용 여부   
        while (!(PINA&(1<<sensor_UPDN)))  { updn_L();  if (!(footFlag)) { updn_H(); goto FAULT;} }
        footFlag =! footFlag;
        while (!(PINA&(1<<sensor_RIGH)))  { left_L(); righ_H();   }         
        while (PINA&(1<<sensor_UPDN))     { updn_H();  }        
        while (!(PINA&(1<<sensor_LEFT)))  { righ_L(); left_H();  } 
        FND++;  set_count();
      break;

       case 20:
        if (PINA&(1<<sensor_RIGH)) {righ_L(); left_H(); footFlag = false; goto FAULT;} 
        if ((millis() - finTime) > CT - 20000) {
        if (PINF&(1<<sw_JIG)) {time_count = 20000; }// 진공 사용 여부   
        while (!(PINA&(1<<sensor_UPDN)))  { updn_L();  if (!(footFlag)) { updn_H(); goto FAULT;} }
        while (!(PINA&(1<<sensor_RIGH)))  { left_L(); righ_H();   }         
        while (PINA&(1<<sensor_UPDN))     { updn_H();  }        
        while (!(PINA&(1<<sensor_LEFT)))  { righ_L(); left_H();  } 
        finTime = millis(); FND++;  set_count();}
      break;

      case 30:
        if (PINA&(1<<sensor_LEFT)) {left_L(); righ_H(); footFlag = false; goto FAULT;} 
        if (PINF&(1<<sw_JIG)) {time_count = 20000; }// 진공 사용 여부    
        while (!(PINA&(1<<sensor_LEFT)))  { righ_L(); left_H();      }  
        while (!(PINA&(1<<sensor_UPDN)))  { updn_L();    if (!(footFlag)) { updn_H(); goto FAULT;} }
        footFlag =! footFlag;
        while (!(PINA&(1<<sensor_RIGH)))  { left_L(); righ_H();      }
        while (PINA&(1<<sensor_UPDN))     { updn_H();             }
        FND++;  set_count();
      break;

      case 40:
        if (PINA&(1<<sensor_LEFT)) {left_L(); righ_H(); footFlag = false; goto FAULT;} 
        if ((millis() - finTime) > CT - 20000) {
        if (PINF&(1<<sw_JIG)) {time_count = 20000; }// 진공 사용 여부    
        while (!(PINA&(1<<sensor_LEFT)))  { righ_L(); left_H();      }  
        while (!(PINA&(1<<sensor_UPDN)))  { updn_L();    if (!(footFlag)) { updn_H(); goto FAULT;} }
        while (!(PINA&(1<<sensor_RIGH)))  { left_L(); righ_H();      }
        while (PINA&(1<<sensor_UPDN))     { updn_H();             }
        finTime = millis(); FND++;  set_count();}
      break;

      default :
      FAULT : break;
    }
    
  }

  switch(btn) {
    case '1': if (PINF&(1<<sw_MANU)) {righ_L(); left_H();} break;
    case '2': if (PINF&(1<<sw_MANU)) updn_H(); break;
    case '3': sw_manu(); break;
    case '4': sw_semi(); break;
    case '5': sw_auto(); break;    
    case 'A': if (PINF&(1<<sw_MANU)) {left_L(); righ_H();} break;
    case 'B': if (PINF&(1<<sw_MANU)) updn_L(); break;    
    case 'D': sw_jig();     break;
    case 'E': sw_pos();     break;
    case 'F': sw_rund();    break;
    
    case 'C':
      option = 1-option;
       if(option == 0)       {
         matrix1.blinkRate(0);
         matrix1.writeDigitNum(1, (CT / 1000) % 10, drawDots);
         matrix1.writeDigitNum(2, (CT / 100) % 10);
         matrix1.writeDisplay();
       }
       if(option == 1)       {
         matrix1.blinkRate(1);
         matrix1.writeDigitNum(1, (optionTime / 1000) % 10, drawDots);
         matrix1.writeDigitNum(2, (optionTime / 100) % 10);
         matrix1.writeDisplay();
       }  break;
       
    case '6':
    if(option == 1)
    {
      optionTime += 100;
      if (optionTime >= 29900) optionTime=29900;
      set_optionTime();
      matrix1.writeDigitNum(1, (optionTime / 1000) % 10, drawDots);
      matrix1.writeDigitNum(2, (optionTime / 100) % 10);
      matrix1.writeDisplay();
     
    } else {    
      CT += 100;
      if (CT >= 29900) CT=29900;
      set_time();    
      matrix1.writeDigitNum(1, (CT / 1000) % 10, drawDots);
      matrix1.writeDigitNum(2, (CT / 100) % 10);
      matrix1.writeDisplay();
      
    } break;
    
    case '7':
    if(option == 1)
    {
      optionTime -= 100;
      if (optionTime<20000) optionTime=20000;
      set_optionTime();
      matrix1.writeDigitNum(1, (optionTime / 1000) % 10, drawDots);
      matrix1.writeDigitNum(2, (optionTime / 100) % 10);
      matrix1.writeDisplay();
      
    } else {
      CT -= 100;
      if (CT<20000) CT=20000;
      set_time();
      matrix1.writeDigitNum(1, (CT / 1000) % 10, drawDots);
      matrix1.writeDigitNum(2, (CT / 100) % 10);
      matrix1.writeDisplay();
      
    } break;
    case 'G':  FND = 0; break;
    default : break;
  }
}

//------------------------------DS12C887 
void bus2out(){
  DDRC = 0xFF;  //for(i=0;i<8;i++) {pinMode(ad[i], OUTPUT);}
}
void bus2in(){
  DDRC = 0x00; //for(i=0;i<8;i++) {pinMode(ad[i], INPUT); }
}

void write_ds(byte add,byte date)    //칩 쓰기 명령
{
  bus2out();  //set address pins to output
  bitClear (PORTA, 1);  //digitalWrite(dscs,0); // PA1  
  bitSet (PORTF, 7);    //digitalWrite(dsas,1); // PF7  
  bitSet (PORTA, 0);    //digitalWrite(dsds,1); // PA0  
  bitSet (PORTA, 2);    //digitalWrite(dsrw,1); // PA2  
  
  //set address on bus(주소 쓰기)
  for(i=0;i<8;i++) {digitalWrite(ad[i],bitRead(add,i));}
  bitClear (PORTF, 7);   //digitalWrite(dsas,0);  // PF7  
  bitClear (PORTA, 2);   //digitalWrite(dsrw,0); // PA2  
  
  //set byte to write(데이터 쓰기)
  for(i=0;i<8;i++) {digitalWrite(ad[i],bitRead(date,i));}
  bitSet (PORTA, 2);    //digitalWrite(dsrw,1); // PA2  
  bitSet (PORTF, 7);    //digitalWrite(dsas,1); // PF7  
  bitSet (PORTA, 1);    //digitalWrite(dscs,1); // PA1  
}

byte read_ds(byte add)         //칩 읽기 명령
{
  byte readb=0; bus2out();  //set address pins to output
  bitClear (PORTA, 1);  //digitalWrite(dscs,0); // PA1  
  bitSet (PORTF, 7);    //digitalWrite(dsas,1); // PF7  
  bitSet (PORTA, 0);    //digitalWrite(dsds,1); // PA0  
  bitSet (PORTA, 2);    //digitalWrite(dsrw,1); // PA2  
  
  //set address on bus(주소 쓰기)
  for(i=0;i<8;i++) {digitalWrite(ad[i],bitRead(add,i));}
  bitClear (PORTF, 7);    //digitalWrite(dsas,0); // PF7  
  bitClear (PORTA, 0);    //digitalWrite(dsds,0); // PA0  
  
  bus2in(); // Input Mode 전환
  for(i=0;i<8;i++){digitalWrite(ad[i],1);}
  readb = bitRead(PINC, PINC0)|(bitRead(PINC, PINC1)<<1)|(bitRead(PINC, PINC2)<<2)|(bitRead(PINC, PINC3)<<3)|(bitRead(PINC, PINC4)<<4)|(bitRead(PINC, PINC5)<<5)|(bitRead(PINC, PINC6)<<6)|(bitRead(PINC, PINC7)<<7);
  bitSet (PORTA, 0);    //digitalWrite(dsds,1); // PA0  
  bitSet (PORTF, 7);    //digitalWrite(dsas,1); // PF7  
  bitSet (PORTA, 1);    //digitalWrite(dscs,1); // PA1  
  return readb;
}

void set_count()  // DS12C887에 카운터 쓰기
{
  write_ds(0xE0, highByte(FND));
  write_ds(0xE1, lowByte(FND)); 
}

void set_time()
{
  write_ds(0xE2, highByte(CT));
  write_ds(0xE3, lowByte(CT));
}

void set_optionTime()
{
  write_ds(0xEA, highByte(optionTime));
  write_ds(0xEB, lowByte(optionTime));
}
