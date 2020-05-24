
void homePos()
{
  if (start_pos == 1) {
    
    left.OFF();
    right.ON();
    sqieez.OFF();
  } else {
    
    right.OFF();
    left.ON();
    sqieez.OFF();
  }
}
