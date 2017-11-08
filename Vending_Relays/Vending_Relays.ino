/* This code is intended to run the Relay board in the HackPGH 'Vend ~N~ Hack' vending machine.
 * Written by Bob Burger, Geno Soroka, Greg Land & chad elish.
 * Modified for Coin Reader by Geno Soroka 10/29/17
 * Fixed LCD overflow issue, added lcd.clear( ) 11/7/17, Geno Soroka
 */
 // LCD Code
#include <stdlib.h>
#include <Wire.h>   // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

const int firstPin = 22;
const int buttonPin = 2;     // the number of the "encoder" pin
// int buttonState = 0;         // variable for reading the pushbutton status \\Just another way to code the same thing with more varibles
char ch;
int relaypin;
int charToPin(char ch);
int inMin = 22; // Lowest input pin to set as output
int inMax = 48; // Highest input pin to set as output

//Snack prices lookup table, change this if snack prices change. Note Q=0 because there is no Q
const unsigned int prices[] = {50,50,50,50,50,50,100,100,100,50,100,100,100,100,100,100,0,100,100,100,100,50,100,50,50,50};
unsigned long duration=0; //Coin Reader pulse duration
unsigned int AmountQuarters=0; // Total amount of quarters available to purchase snacks
char PriceDisplay[10]; // ASCII buffer for displaying price

void setup() 
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(52,INPUT); // coin reader pin
  digitalWrite(52, HIGH); // pull up
  for(int i=inMin; i<=inMax; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  lcd.begin(20,4);   // initialize the lcd for 20 chars 2 lines, turn on backlight

  // ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
   }
  lcd.backlight(); // finish with backlight on 
  //-------- Write characters on the display ------------------
  // NOTE: Cursor Position: (CHAR, LINE) start at 0  
  lcd.clear ( );
  lcd.setCursor(4,0); //Start at character 4 on line 0
  lcd.print("Vend n' Hack");
  delay(1000);
  lcd.setCursor(14,1);
  lcd.print("(v.3b)"); 
  lcd.setCursor(7,3); 
  lcd.print('$');
  lcd.setCursor(8,3);
  sprintf(PriceDisplay,"%d.%02d",AmountQuarters/100,AmountQuarters%100);
  lcd.print(PriceDisplay);
} /*--(end setup )---*/


void loop()
{
  if (Serial.available() > 0) 
  {
      ch = Serial.read();
      relaypin = charToPin(ch);
      
      if (relaypin > 0)      
      {
          if (digitalRead(buttonPin) == HIGH)
          {
            //Only activate coil if user dropped in enough quarters
            if (AmountQuarters>= prices[ch-65])
            {
              AmountQuarters=AmountQuarters-prices[ch-65]; //Subtract the number of quarters which the item costs     
              lcd.clear ( );
              lcd.setCursor(5,3); //Start at character 0 on line 4
              lcd.print("Vending: ");
              lcd.print(ch);
              digitalWrite(relaypin, LOW);
              //Serial.println("LED ON!");
              //Serial.println("Waiting");
              delay (1000);
              while (digitalRead(buttonPin) == LOW)
              {
                //buttonState = digitalRead(buttonPin); \\Just another way to code the same thing with more code and varibles
              }
              //Serial.println("Button Unpressed!");  
              digitalWrite(relaypin, HIGH);
              //Serial.println("LED OFF!");
              delay (600);
              lcd.clear ( );
              lcd.setCursor(4,0); //Start at character 4 on line 0
              lcd.print("Vend n' Hack");
              lcd.setCursor(14,1);
              lcd.print("(v.3b)");
              lcd.setCursor(4,3); //Start at character 0 on line 4
              lcd.print("                  ");
              lcd.setCursor(7,3); 
              lcd.print('$');
              lcd.setCursor(8,3);
              sprintf(PriceDisplay,"%d.%02d",AmountQuarters/100,AmountQuarters%100);
              lcd.print(PriceDisplay);
            }
       
           }
       }
   }

   /*Poll pin 52 for pulses coming from coin reader, only detects quarters for now*/
   duration = pulseIn(52, HIGH); 
   if (duration>=15000)
   {
    AmountQuarters+=25;
    lcd.clear ( );
    lcd.setCursor(4,0); //Start at character 4 on line 0
    lcd.print("Vend n' Hack");
    lcd.setCursor(14,1);
    lcd.print("(v.3b)");
    lcd.setCursor(4,3); //Start at character 0 on line 4
    lcd.print("                  ");
    lcd.setCursor(7,3); 
    lcd.print('$');
    lcd.setCursor(8,3);
    sprintf(PriceDisplay,"%d.%02d",AmountQuarters/100,AmountQuarters%100);
    lcd.print(PriceDisplay);
   }
   
  
}
  
int charToPin(char ch) {
  if (ch < 'A' || ch > 'Z') {
    return -1;
  }
  return ch - 'A' + firstPin;
  }
