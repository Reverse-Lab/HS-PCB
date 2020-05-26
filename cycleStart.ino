
void cycleStart()
{
  unsigned long currentMillis = millis();
  switch (mode){
    case 1:
     // homePos();
      if (start_pos == 0) { // 후 인쇄 모드 LED ON 필요
        while((digitalRead(updownSensor) == LOW) && (digitalRead(leftSensor) == HIGH))
          {sqieez.ON(); break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(leftSensor) == HIGH))
          {left.OFF(); right.ON();break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(rightSensor) == HIGH))
          {sqieez.OFF(); break;}
        while((digitalRead(updownSensor) == LOW) && (digitalRead(rightSensor) == HIGH))
          {right.OFF(); left.ON(); footFlag = 0; write_ds(0xEF, 0); FND++;
          break;}
           
      } else if (start_pos == 1){  // 전 인쇄 모드 LED OFF 필요
        while((digitalRead(updownSensor) == LOW) && (digitalRead(leftSensor) == HIGH))
          {left.OFF(); right.ON();break;}
        while((digitalRead(updownSensor) == LOW) && (digitalRead(rightSensor) == HIGH))
          {sqieez.ON(); break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(rightSensor) == HIGH))
          {right.OFF(); left.ON();break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(leftSensor) == HIGH))
          {sqieez.OFF(); footFlag = 0; write_ds(0xEF, 0); FND++;
          break;}        
      }
    break;

    case 2:
      if (currentMillis - previousMillis >= CT){
   
        if (start_pos == 0) {
        while((digitalRead(updownSensor) == LOW) && (digitalRead(leftSensor) == HIGH))
          {sqieez.ON(); break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(leftSensor) == HIGH))
          {left.OFF(); right.ON();break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(rightSensor) == HIGH))
          {sqieez.OFF(); break;}
        while((digitalRead(updownSensor) == LOW) && (digitalRead(rightSensor) == HIGH))
          {right.OFF(); left.ON(); FND++;  
          previousMillis = currentMillis; break;} 
        } 
        
        else if (start_pos == 1){    
        while((digitalRead(updownSensor) == LOW) && (digitalRead(leftSensor) == HIGH))
          {left.OFF(); right.ON();break;}
        while((digitalRead(updownSensor) == LOW) && (digitalRead(rightSensor) == HIGH))
          {sqieez.ON(); break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(rightSensor) == HIGH))
          {right.OFF(); left.ON();break;}
        while((digitalRead(updownSensor) == HIGH) && (digitalRead(leftSensor) == HIGH))
          {sqieez.OFF(); FND++;    
          previousMillis = currentMillis; break;}   
        }
      }
    break;
    
    default:
    break;  
  }
}
