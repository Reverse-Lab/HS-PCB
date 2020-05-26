
void homePos()
{
  if (start_pos == 1) {
    sqieez.OFF();
    left.OFF();
    right.ON();
    
  } else if (start_pos == 0) {
    sqieez.OFF();
    right.OFF();
    left.ON();
    
  }
}
