

/* This code is intended to run the Relay board in the HackPGH 'Vend ~N~ Hack' vending machine.
 * Written by Bob Burger, Geno Soroka, Greg Land & Chad Elish.
 * Modified for Coin Reader by Geno Soroka 10/29/17
 * Fixed LCD overflow issue, added lcd.clear( ) 11/7/17, Geno Soroka
 * Added bill reader, 11/18/17, Geno Soroka
 * Changed prices for Arduinos, Pi's, and movie Hackers, 12/22/17
 */
 // LCD Code
#include <stdlib.h>
#include <Wire.h>   // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <Apex5400BillAcceptor.h>
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

#define PIN_ENABLE 7 //Purple wire, to enable set low, to disable set high
#define PIN_INTERRUPT_LINE 8 //Orange wire on Apex, Request to send data to host
#define PIN_SEND_LINE 9 //White/Blue wire, Host Ready Signal
#define PIN_TTL_RX 10 //Green wire, Transmit Data Line from acceptor

Apex5400BillAcceptor *billAcceptor;

const int firstPin = 22;
const int buttonPin = 2;     // the number of the "encoder" pin
// int buttonState = 0;         // variable for reading the pushbutton status \\Just another way to code the same thing with more varibles
char ch;
int relaypin;
int charToPin(char ch);
int inMin = 22; // Lowest input pin to set as output
int inMax = 48; // Highest input pin to set as output

//Snack prices lookup table, change this if snack prices change. Note Q=0 because there is no Q
//                               A,  B,  C,  D,  E,  F,  G,  H,  I, J, K,  L,  M,     N, O, P,Q,   R,   S,  T,  U,   V,   W,   X,   Y,  Z
const unsigned int prices[] = {100,100,100,200,100,100,100,100,200,50,100,100,1337,5000,50,50,0,1000,1000,500,100,1000,2000,4000,2000,300};
unsigned long duration=0; //Coin Reader pulse duration
unsigned int AmountMoney=0; // Total amount of money available to purchase snacks (in cents, i.e. 50= $0.50, 100= $1.00)
char PriceDisplay[10]; // ASCII buffer for displaying price

int code;

void setup() 
{
  Serial.begin(9600);
  billAcceptor = new Apex5400BillAcceptor(PIN_ENABLE, PIN_INTERRUPT_LINE, PIN_SEND_LINE, PIN_TTL_RX);
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
  sprintf(PriceDisplay,"%d.%02d",AmountMoney/100,AmountMoney%100);
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
            //Only activate coil if user dropped in enough money
            if (AmountMoney>= prices[ch-65])
            {
              AmountMoney=AmountMoney-prices[ch-65]; //Subtract the amount which the item costs     
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
              sprintf(PriceDisplay,"%d.%02d",AmountMoney/100,AmountMoney%100);
              lcd.print(PriceDisplay);
            }
       
           }
       }
   }

   /*Poll pin 52 for pulses coming from coin reader, only detects quarters for now*/
   duration = pulseIn(52, HIGH); 
   if (duration>=15000)
   {
    AmountMoney+=25;
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
    sprintf(PriceDisplay,"%d.%02d",AmountMoney/100,AmountMoney%100);
    lcd.print(PriceDisplay);
   }
   
   if(code = billAcceptor->checkForBill())
    {
      switch (code)
      {
        case 0x81:
        AmountMoney+=100;
        break;
        case 0x83:
        AmountMoney+=500;
        break;
        case 0x84:
        AmountMoney+=1000;
        break;
        case 0x85:
        AmountMoney+=2000;
        break;
        case 0x86:
        AmountMoney+=5000;
        break;
        case 0x87:
        AmountMoney+=10000;
        break;
      }

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
      sprintf(PriceDisplay,"%d.%02d",AmountMoney/100,AmountMoney%100);
      lcd.print(PriceDisplay);
    }
}
  
int charToPin(char ch) {
  if (ch < 'A' || ch > 'Z') {
    return -1;
  }
  return ch - 'A' + firstPin;
  }
