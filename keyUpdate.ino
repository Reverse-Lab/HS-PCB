
void mode_update()
{
  switch (mode){
    case 0:
    digitalWrite (manual_led, HIGH);
    digitalWrite (harf_led, LOW);
    digitalWrite (auto_led, LOW);
    write_ds(0xE4, byte(mode));
    break;

    case 1:
    digitalWrite (manual_led, LOW);
    digitalWrite (harf_led, HIGH);
    digitalWrite (auto_led, LOW);
    write_ds(0xE4, byte(mode));
    break;

    case 2:
    digitalWrite (manual_led, LOW);
    digitalWrite (harf_led, LOW);
    digitalWrite (auto_led, HIGH);
    write_ds(0xE4, byte(mode));
    break;

    default:
    break;
  }
}

void jig_update()
{
  switch (jig){
    case 0:
    digitalWrite (jig_led, LOW);
    write_ds(0xE5, byte(jig));
    break;

    case 1:
    digitalWrite (jig_led, HIGH);
    write_ds(0xE5, byte(jig));
    break;

    default:
    break;
  }
}

void pos_update()
{
  switch (start_pos){
    case 0:
    digitalWrite (pos_led, LOW);
    write_ds(0xE6, byte(start_pos));
    break;

    case 1:
    digitalWrite (pos_led, HIGH);
    write_ds(0xE6, byte(start_pos));
    break;

    default:
    break;
  }
}

void shape_update()
{
  switch (shape){
    case 0:
    digitalWrite (shape_led, LOW);
    write_ds(0xE7, byte(shape));
    break;

    case 1:
    digitalWrite (shape_led, HIGH);
    write_ds(0xE7, byte(shape));
    break;

    default:
    break;
  }
}
