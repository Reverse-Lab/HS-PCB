
void setup() {
  
  pinMode(footSW, INPUT);
  pinMode(updownSensor, INPUT);
  pinMode(leftSensor, INPUT);
  pinMode(rightSensor, INPUT);

  pinMode(Buz, OUTPUT);

  pinMode(manual_led,   OUTPUT);
  pinMode(harf_led,     OUTPUT);
  pinMode(auto_led,     OUTPUT);
  pinMode(jig_led,      OUTPUT);
  pinMode(pos_led,      OUTPUT);
  pinMode(shape_led,    OUTPUT);

  pinMode(dscs, OUTPUT);
  pinMode(dsas, OUTPUT);
  pinMode(dsrw, OUTPUT);
  pinMode(dsds, OUTPUT);
  pinMode(dsirq, OUTPUT);
  
  //DS12C887   칩 초기화 최초 1회 실행 할 것
  // write_ds(regA,0x20);      // 수정 발진기를 켜고 클럭을 계속 작동 시키려면 A 레지스터를 제어하십시오.
  // write_ds(regB,0x26);        //24 시간 모드, 데이터 바이너리 모드를 설정하고 알람 인터럽트를 활성화하십시오
  // write_ds(0xE4, 0);
  // write_ds(0xE5, 0);
  // write_ds(0xE6, 0);
  // write_ds(0xE7, 0);
  
  byte hiByte = read_ds(0xE0); // 카운터 read(주소)
  byte loByte = read_ds(0xE1); // 카운터 read(주소)
  FND = word(hiByte, loByte);

  byte ct_hiByte = read_ds(0xE2); // 카운터 read(주소)
  byte ct_loByte = read_ds(0xE3); // 카운터 read(주소)
  CT = word(ct_hiByte, ct_loByte);

  mode = read_ds(0xE4);
  jig = read_ds(0xE5);
  start_pos = read_ds(0xE6);
  shape = read_ds(0xE7);

  write_ds(0xEF, 0); // 작업개시 주소 초기화

  mode_update();
  jig_update();
  pos_update();
  shape_update();

  delay(500);

  digitalWrite(Buz, HIGH);
  delay(40);
  digitalWrite(Buz, LOW);
  delay(10);
    
  //Serial.begin(115200);
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
}
