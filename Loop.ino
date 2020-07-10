

void loop() {
    //timer.run();
    boolean drawDots = true;    
  
    matrix.print(FND, DEC); 
    matrix.writeDisplay();

    start_update();

    char btn = myKeypad.getKey();
    if(btn != NO_KEY) {
    
    digitalWrite(Buz, HIGH);
    delay(40);
    digitalWrite(Buz, LOW);
    delay(10);
    /*
    byte hiByte = read_ds(0xE0); // read(주소)
    byte loByte = read_ds(0xE1); // read(주소)
    FND = word(hiByte, loByte);
    FND++;
    delay(10);
    */
    }
  set_count();
  
  switch(btn) {
    case '1':
      left.OFF();
      right.ON(); // 
    break;

    case 'A':
      right.OFF();
      left.ON(); // 스퀴즈 ON/OFF
    break;

    case '2':
      sqieez.ON(); // 스퀴즈 ON/OFF
    break;

    case 'B':
      sqieez.OFF(); // 스퀴즈 ON/OFF
    break;

    case '3':
      mode = 0; // 수동모드
      mode_update();
    break;

    case '4':
      mode = 1; // 반자동모드
      mode_update();
    break;

    case '5':
      mode = 2; // 자동모드
      mode_update();
    break;

    case 'C':
      option = 1-option;
       if(option == 0)
       {
         matrix1.blinkRate(0);
         matrix1.writeDigitNum(1, (CT / 1000) % 10, drawDots);
         matrix1.writeDigitNum(2, (CT / 100) % 10);
         matrix1.writeDisplay();
       }
        if(option == 1)
       {
         matrix1.blinkRate(1);
         matrix1.writeDigitNum(1, (optionTime / 1000) % 10, drawDots);
         matrix1.writeDigitNum(2, (optionTime / 100) % 10);
         matrix1.writeDisplay();
       }
    break;
    
    case 'D':
      jig = 1-jig; // 0:자동지그 OFF, 1:자동지그 ON
      jig_update();
    break;
    
    case 'E':
      start_pos = 1-start_pos; // 0:전인쇄, 1:후인쇄모드
      pos_update();
    break;

    case 'F':
      shape = 1-shape; // 0:곡면, 1:평면
      shape_update();
    break;
    
    case '6':
    if(option == 1)
    {
      optionTime += 100;
      if (optionTime>20000) optionTime=20000;
      set_optionTime();
      matrix1.writeDigitNum(1, (optionTime / 1000) % 10, drawDots);
      matrix1.writeDigitNum(2, (optionTime / 100) % 10);
      matrix1.writeDisplay();
      break;
    } else {
    
    CT += 100;
    if (CT>20000) CT=20000;
    set_time();
    
    matrix1.writeDigitNum(1, (CT / 1000) % 10, drawDots);
    matrix1.writeDigitNum(2, (CT / 100) % 10);
    matrix1.writeDisplay();
    break;
    }
    
    case '7':
    if(option == 1)
    {
      optionTime -= 100;
      if (optionTime<100) optionTime=99;
      set_optionTime();
      matrix1.writeDigitNum(1, (optionTime / 1000) % 10, drawDots);
      matrix1.writeDigitNum(2, (optionTime / 100) % 10);
      matrix1.writeDisplay();
      break;
    } else {
    CT -= 100;
    if (CT<100) CT=99;
    set_time();
    matrix1.writeDigitNum(1, (CT / 1000) % 10, drawDots);
    matrix1.writeDigitNum(2, (CT / 100) % 10);
    matrix1.writeDisplay();
    break;
    }
    
    case 'G':
    FND = 0;
    break;
    
    default:
    break;  
  }
}
