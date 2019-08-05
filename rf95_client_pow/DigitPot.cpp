#include "Arduino.h"
#include "DigiPot.h"

DigiPot::DigiPot(int incPin, int udPin, int csPin) {
  _incPin = incPin;
  _udPin = udPin;
  _csPin = csPin;  
  _currentValue = DIGIPOT_UNKNOWN;

  pinMode(_incPin, OUTPUT);
  pinMode(_udPin, OUTPUT);
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);

}

void DigiPot::reset() {
  // change down maximum number of times to ensure the value is 0
  decrease(DIGIPOT_MAX_AMOUNT);
  _currentValue = 0;
}

void DigiPot::setCurrentValue(int val){
  _currentValue = val;
}

void DigiPot::set(int value) {
  value = constrain(value, 0, DIGIPOT_MAX_AMOUNT);
  if (_currentValue == DIGIPOT_UNKNOWN) reset();
  if (_currentValue > value) {
    change(DIGIPOT_DOWN, _currentValue-value);
  } else if (_currentValue < value) {
    change(DIGIPOT_UP, value-_currentValue);
  }
}

int DigiPot::get() {
  return _currentValue;
}

void DigiPot::increase(int amount) {
  amount = constrain(amount, 0, DIGIPOT_MAX_AMOUNT);
  change(DIGIPOT_UP, amount);
}

void DigiPot::decrease(int amount) {
  amount = constrain(amount, 0, DIGIPOT_MAX_AMOUNT);
  change(DIGIPOT_DOWN, amount);
}

void DigiPot::change(int direction, int amount) {
  amount = constrain(amount, 0, DIGIPOT_MAX_AMOUNT);
  digitalWrite(_udPin, direction);
  digitalWrite(_incPin, HIGH);
  digitalWrite(_csPin, LOW);

  for (int i=0; i<amount; i++) {
    digitalWrite(_incPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_incPin, HIGH);
    delayMicroseconds(2);
    if (_currentValue != DIGIPOT_UNKNOWN) {
      _currentValue += (direction == DIGIPOT_UP ? 1 : -1);
      _currentValue = constrain(_currentValue, 0, DIGIPOT_MAX_AMOUNT);
    }
    
  }
  digitalWrite(_csPin, HIGH);
}
