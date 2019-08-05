#define DIGIPOT_UP   HIGH
#define DIGIPOT_DOWN LOW
#define DIGIPOT_MAX_AMOUNT 99
#define DIGIPOT_UNKNOWN 255

class DigiPot
{
 public:
  DigiPot(int incPin, int udPin, int csPin);
  void increase(int amount);
  void decrease(int amount);
  void change(int direction, int amount);
  void set(int value);
  void setCurrentValue(int value);
  int get();
  void reset();

 private:
  int _incPin;
  int _udPin;
  int _csPin;
  int _currentValue;
};
