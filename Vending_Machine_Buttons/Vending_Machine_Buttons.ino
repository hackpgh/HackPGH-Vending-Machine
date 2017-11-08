/*  Keypadtest.pde
 *  Modification of simple keypad.
 *  Modified for HackPGH 'Vend ~N~ Hack' vending machine by chad elish.
 *
 */
#include <Keypad.h>

const byte ROWS = 5; // Four rows
const byte COLS = 5; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'A','B','C','D','E'},
  {'F','G','H','I','J'},
  {'K','L','M','N','O'},
  {'P','R','S','T','U'},
  {'V','W','X','Y','Z'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 7, 6, 5, 4, 3 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 12, 11, 10, 9, 8 }; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define ledpin 13

void setup()
{
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin, HIGH);
  Serial.begin(9600);
}

void loop()
{
  char key = kpd.getKey();
  if(key)  // Check for a valid key.
  {
    switch (key)
    {
      case '*':
        digitalWrite(ledpin, LOW);
        break;
      case '#':
        digitalWrite(ledpin, HIGH);
        break;
      default:
        Serial.println(key);
        delay(5000);                  // waits for a 5000ms. cheep and dirty debounce. If this is not here other arduino will cache other button presses during vend and then preform them.
    }
  }
}
