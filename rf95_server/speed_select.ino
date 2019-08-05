unsigned long t_start = 0;
uint8_t last_speed = 0;

uint8_t readSwitch() {
  byte out = 0;
  bitWrite(out, 0, !digitalRead(SPEED1));
  bitWrite(out, 1, !digitalRead(SPEED2));
  bitWrite(out, 2, !digitalRead(SPEED3));
  return out;
}

uint8_t getCurrentSpeed(int speed) {
  switch (speed) {
    case 0b001: return OUT_SPEED1;
    case 0b010: return OUT_SPEED2;
    case 0b100: return OUT_SPEED3;
    default: return currentSpeed;
  }
}

uint8_t readSpeed() {
  uint8_t speed = readSwitch();
  currentSpeed = getCurrentSpeed(speed);
  if (speed == 0 && last_speed == 0) {
    if (t_start == 0) {
      t_start = millis();
    }
    if (t_start > 0 && (millis() - t_start) >= 1500) {
      t_start = 0;
      currentSpeed = 0;
    }
  } else {
    currentSpeed = getCurrentSpeed(speed);
    if (currentSpeed && last_speed > 0 && t_start > 0) t_start = 0;
  }
  last_speed = speed;
}
