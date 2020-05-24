
void cycleStart()
{
  switch (mode){
    case 1:
      // homePos();
      if (start_pos == 0) {
        while((digitalRead(updownSensor) == LOW) && (digitalRead(rightSensor) == HIGH))
          {sqieez.ON(); break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(rightSensor) == HIGH))
          {right.OFF(); left.ON();break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(leftSensor) == HIGH))
          {sqieez.OFF(); left.OFF(); right.ON(); footFlag = 0; write_ds(0xEF, 0); FND++;
          break;}
           
      } else if (start_pos == 1){
        while((digitalRead(updownSensor) == LOW) && (digitalRead(rightSensor) == HIGH))
          {right.OFF(); left.ON();break;}
        while((digitalRead(updownSensor) == LOW) && (digitalRead(leftSensor) == HIGH))
          {sqieez.ON(); break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(leftSensor) == HIGH))
          {left.OFF(); right.ON();break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(rightSensor) == HIGH))
          {sqieez.OFF(); footFlag = 0; write_ds(0xEF, 0); FND++;
          break;}        
      }
    break;

    case 2:
      //homePos();
      if (start_pos == 0) {
        sqieez.ON();
        left.OFF();
        right.ON();
        sqieez.OFF();
        right.OFF();
        left.ON();
      } else {
        sqieez.ON();
        right.OFF();
        left.ON();
        sqieez.OFF();
        left.OFF();
        right.ON();
      }
      FND++;
    break;
    
    default:
    break;  
  }
}
