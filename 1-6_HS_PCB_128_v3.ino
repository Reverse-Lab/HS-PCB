// 한솔 PCB v1.3 PRG
//  2020-05-25

// 모드 변경시 footSW 기억해제 필요ㄴ
// 자동 운전중 footSW 정지기능 추가 필요
// 운전 시작위치 항상 일정한가? 아니면 현재 위치를 기준으로 하는가?? 
// -> 시작위치는 Shape 곡면 모드 left -> right
//             Shape 평면 모드 Right -> left 로 결정 됨 

#include <Key.h>
#include <Keypad.h>

#include "Wire.h"

#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();
Adafruit_7segment matrix1 = Adafruit_7segment();


//----------------------------------------------------------------------//

#define footSW        37  //DI2  풋 스위치
#define updownSensor  40  //DI5 하강 실린더 센서 입력
#define rightSensor   38  //DI4 제판 좌측 센서 ON/OFF
#define leftSensor    39  //DI3 제판 우측 센서 ON/OFF

#define sqieezSol     20  //DO13  // 스퀴즈 ON/OFF
#define rightSol      21  //DO14  // 제판 우측 이동
#define leftSol       22  //DO15  // 제판 좌측 이동

#define acOut         15  // 220V 제어 PIN

#define manual_led    45  //LED15
#define harf_led      46  //LED16
#define auto_led      47  //LED17
#define jig_led       49  //LED19
#define pos_led       50  //LED20
#define shape_led     48  //LED18

#define Buz 25

int footState = 0;                   // 센서 상태 저장 변수
int footFlag = 0;
int mode;                        // 0:수동, 1:반자동, 2: 자동
int jig;                         // 0 자동지그 OFF, 1:자동지그 ON
int start_pos;                   // 0:전인쇄, 1:후인쇄 모드 저장 변수
int shape;                       // 0:곡면, 1:평면
int startAddr;                   // 0:정지, 1:작업 개시
int FND;
int CT;

unsigned long previousMillis = 0;

//--------------DS12C887------------------

const int dscs=43;
const int dsas=52;
const int dsrw=42;
const int dsds=44;
const int dsirq=51;
//define bidirectional address-data bus(양방향 주소 버스 정의)

const int ad[8]={28,29,30,31,32,33,34,35};
//등록 A B C D 주소

#define regA 0x0A
#define regB 0x0B
#define regC 0x0C
#define regD 0x0D

//DS12C887 함수 선언
void bus2out();
void bus2in();
void write_ds(byte add,byte date);
byte read_ds(byte add);
int i; // DS12C887 Bit Address 지정용

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


class Flasher
{
    int solPin;         // 출력 핀 이름
    int sensorPin;      // 센서 핀
    int finishPin;      // 동작 완료 확인

    int finishState;

    int solState;       // 출력 상태 변수
    int sensorState;    // 센서 상태 변수

  public:
  Flasher(int sol, int sens, int check)
  {
    solPin = sol;
    pinMode(solPin, OUTPUT);    

    sensorPin = sens;
    pinMode(sensorPin, INPUT);

    finishPin = check;  // 솔 동작 상태 확인
    pinMode(finishPin, INPUT);

  }
 
  void ON()
  {
    sensorState = digitalRead(sensorPin);
      if(sensorState == LOW)
      {
        solState = HIGH;                // 솔밸브 ON
        digitalWrite(solPin, solState); 
      }
  }
  
  void OFF()
  {
    sensorState = digitalRead(sensorPin);
      if(sensorState == HIGH)
      {
        solState = LOW;                 // 솔밸브 OFF
        digitalWrite(solPin, solState); 
      }
  }
};


Flasher sqieez(sqieezSol, updownSensor, updownSensor);    // (출력핀, 센서핀, 타입아웃)
Flasher right(rightSol, rightSensor, rightSensor);       // 제판 우측 이동 ON/OFF
Flasher left(leftSol, leftSensor, leftSensor);          // 제판 좌측 이동 ON/OFF
