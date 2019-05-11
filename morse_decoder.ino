/*
 * 
 *  Arduino Morse Decoder
 *  (c) Craig McIntyre 2019
 *  Licensed under WTFPL
 *  
*/

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 13);
unsigned long signal_valid,timer1,timer2,timer3,timer4;   //timing variables
int inputPin = 10;                      //input pin for push button
int morsePin = 9;                       //output for morse LED
int buzzPin = 8;                        //powers a 555 buzzer circuit (optional)
String code = "";                       //string to hold letters
int space = true;                       //boolean to prevent multiple spaces
int first_space = true;                 //boolean to prevent leading space
int letterCount = 0;                    //keeps count to move on to second line of lcd

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);                     //initialise the LCD with 2 rows
  pinMode(buzzPin, OUTPUT);
  pinMode(inputPin, INPUT_PULLUP);      //to help prevent bouncing
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, buttonPressed, RISING);    
  pinMode(morsePin, OUTPUT);
}

void clearScreen() {
  lcd.clear();
  lcd.setCursor(0,0);  
}

void buttonPressed() {
  first_space = true;
  code = "";
  clearScreen();
}

void loop() {
NextDotDash:
  
  timer3 = millis();                    //space timer checks time from last press
  while (digitalRead(inputPin) == HIGH) {}
    space = false;                      //reset to allow spaces to occur
    timer1 = millis();                  //letter timer for letter spacing
    digitalWrite(morsePin, HIGH);       //turn on the led :)
    digitalWrite(buzzPin, HIGH);
  
  while (digitalRead(inputPin) == LOW) {}
    timer2 = millis();                            
    digitalWrite(morsePin, LOW);        //LED off on button release
    digitalWrite(buzzPin, LOW);
    signal_valid = timer2 - timer1;       //time for which button is pressed
  
  if (signal_valid > 50) {                //to account for switch debouncing
    code += morsify();                   //function to read dot or dash
  }
  
  while ((millis() - timer2) < 500) {   //move to next letter after 0.5sec 
    timer4 = millis();
    if ((timer4 - timer3) > 2000) {     // add a space at 2secs
      if (space == false && first_space == false) {
      lcd.print(" ");
      letterCount += 1;
      space = true;
      }
    }
    if (digitalRead(inputPin) == LOW) {
      goto NextDotDash;
    }
  }
  decoder();                          //function to decipher code into alphabet
}

char morsify() {
  if (signal_valid < 250 && signal_valid > 50)
  {
    return '0';                        //dot if signal < than 0.25sec
  }
  else if (signal_valid > 250)
  {
    return '1';                        //dash if signal > 0.25 sec
  }
}

void decoder() {
  
  static String letters[] = {"01", "1000", "1010", "100", "0", "0010", "110",
                             "0000", "00", "0111", "101", "0100", "11", "10",
                             "111", "0110", "1101","010", "000", "1", "001", 
                             "0001", "011", "1001", "1011", "1100", "E"};

  static String numbers[] = {"11111", "01111", "00111", "00011", "00001",
                             "00000", "10000", "11000", "11100", "11110", "E"};
                             
  int i = 0;                          //counter for letters loop
  int j = 0;                          //counter for numbers loop

  if (code == "010101") {             //manual checks for punctuation           
    lcd.print(".");
  }
  else if (code == "110011") {
    lcd.print(",");
  }
  else if (code == "001100") {
    lcd.print("?");
  }
  else if (code == "10010") {
    lcd.print("/");
  }
  else if (code == "011010") {
    lcd.print("@");
  }
  else {
    while (letters[i] != "E") {       //loops letter array for match
      if (letters[i] == code) {     
        lcd.print(char('A' + i));     //adds index of the array to 'A' to give letter
        break;
      }
      i++;
    }
    while (numbers[j] != "E") {       //loops number array for match
      if (numbers[j] == code) {
        lcd.print(0 + j);             //adds index of the array to 0 to give number
        break;
      }
      j++;
    }
    if (letters[i] == "E" && numbers[j] == "E") {
      lcd.print("#");                 //hash if character not found
    }
  }
  
  code = "";                          //empty code variable for next loop
  first_space = false;                //spaces can happen after first loop
  letterCount += 1;
  
  if (letterCount == 16) {           //move to second line after 16 chars
    lcd.setCursor(0,1);
  }
  else if (letterCount == 32) {       //clear screen after 32 chars
    delay(1000);
    clearScreen();
    letterCount = 0;
    first_space = true;
  }
}
