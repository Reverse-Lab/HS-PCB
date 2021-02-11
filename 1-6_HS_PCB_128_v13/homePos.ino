void start_update()
{
    foot_Update();
    //emg_Update();
       
    startAddr = read_ds(0xEF);
    if (startAddr == 1)
    {
      mode_update();
      jig_update();
      pos_update();
      shape_update();

      if (mode == 1 || mode == 2) // 반자동 또는 자동모드면 작업개시
      {
        cycleStart();
      } else {
        homePos();
      }
    }
}
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
