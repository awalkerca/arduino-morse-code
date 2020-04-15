#include <Arduino.h>
#include <LiquidCrystal.h>


// initialize the library with the numbers of the interface pins
/*
  VALUES TO CHANGE
*/
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int LCD_ROWS = 2;
const int LCD_COLS = 16;
const int AUDIO_PIN = 6;
const int BUTTON_PIN = 2;
const int LED_PIN = 13;
const int DOT_LEN = 200;     // length of the morse code 'dot'


// morse code and values
const char* codes[36] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", "-----"};
const char values[36] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

// Morse code rules
int dashLen = DOT_LEN * 3;    // length of the morse code 'dash'
int elemPause = DOT_LEN;  // length of the pause between elements of a character
int charPause = DOT_LEN * 3;     // length of the spaces between characters
int wordPause = DOT_LEN * 7;  // length of the pause between words
int note = 1200;      // music note/pitch

// input tracking
int downStart = 0;			//Timestamp at first press of morse button
bool down = false;			//Tracks state of morse btn
int spaceStart = 0;     //timestamp at first start of a space
int debounce = 20;      //To make space for reducing errors
int buttonState = 0; 
bool isActive = false;

// states for space processing
const int WORD = 2;
const int CHAR = 1;
const int ELEM = 0;
const int OFF = 4;
int spaceMode = ELEM;

// contains message being built
String message;
int msgCursor = 0;
int symbolCursor = 0;
const int msgRow = 1;
const int symbolRow = 0;

//Maps morse code to its approprate char
char mapChar(const char* code) {
	for(int i = 0;i < 38;++i) {
		if(strncmp(code,codes[i],6) == 0) {
			return values[i];
		}
	}
	return '?';
}

String mapSymbol(const char c) {
  for(int i = 0;i < 38;++i) {
		if(values[i]==c) {
			return codes[i];
		}
	}
	return "?";
}

// Function that returns a translated string from morse
String translate(String msg) {
	String translated = "";
	char current[6];
	memset(current, 0, 6);
  // only process up to first six characters, since no symbol uses more than that (in alpha-numeric, english)
  for (int i = 0; i < min(msg.length(),6); i++) {
    current[i] = msg.charAt(i);
  }
  translated += mapChar(current);
	return translated;
}

void beep (bool shouldBeep) {
  if (shouldBeep) {
    digitalWrite(LED_PIN, HIGH);
    tone(AUDIO_PIN, note);
  } else {
    digitalWrite(LED_PIN, LOW);
    noTone(AUDIO_PIN);
  }
}

String outCode (String msg) {
  String output = "";
  for (int i = 0; i < msg.length(); i++) {
    output+= mapSymbol(msg.charAt(i));
  }
  return output;
}

void registerCode(int currentTime) {
  char code;
  if (currentTime - downStart > dashLen) {
    code = '-';
  } else if (currentTime - downStart > debounce) {
    code = '.';
  }
  lcd.setCursor(symbolCursor, symbolRow);
  lcd.print(code);
  symbolCursor++;

  message+=code;
  spaceMode = ELEM;
}

void resetMessage () {
  message = "";
  lcd.clear();
  symbolCursor = 0;
  msgCursor = 0;
}

void clearRow(int row) {
  lcd.setCursor(0, row);
  lcd.print("                ");
}

void beepForSymbol(char symbol) {
  if (symbol == '.') {
    beep(true);
    delay(DOT_LEN);
    beep(false);
  } else if (symbol == '-') {
    beep(true);
    delay(dashLen);
    beep(false);
  }
  delay(elemPause);
}

void demoSpeed() {
  const String symbols = outCode("SOS");
  for (int i = 0; i < symbols.length(); i++) {
    beepForSymbol(symbols.charAt(i));
    delay(charPause);
  }
  

}

void processSpace(int currentTime) {
  //if gap is roughly that of 7x dash...
  //else if gap is roughly that of 3x dash...
  //else is a gap

  const int spaceLength = currentTime - spaceStart;
  if (spaceLength >= 3* wordPause) {
    isActive = false;
    spaceMode = OFF;
    resetMessage();
  } else if (spaceLength >= wordPause && spaceMode == CHAR) {
    //pause has now been long enough to indicate a word is being sent; send a space
    spaceMode = WORD;
    lcd.setCursor(msgCursor, msgRow);
    lcd.print(" ");
    msgCursor++;
  } else if (spaceLength >= charPause && spaceMode == ELEM) {
    //pause has now been long enough to indicate a character is being sent; process it
    spaceMode = CHAR;
    const String translated = translate(message);
    message = "";
    lcd.setCursor(msgCursor, msgRow);
    lcd.print(translated);
    msgCursor+= translated.length();
    lcd.write(" ");
    if (msgCursor >= LCD_COLS) {
      msgCursor = 0;
    }
    clearRow(symbolRow);
    symbolCursor = 0;

  } else {
    //do nothing, since it's still just within element time
  }
}

void setActive () {
  if (!isActive) {
    resetMessage();
    isActive = true;
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(AUDIO_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_COLS, LCD_ROWS);
  // Print a message to the LCD.
  String output = outCode("READY");
  lcd.setCursor(0, symbolRow);
  lcd.print(output);
  lcd.setCursor(0, msgRow);
  lcd.print("READY");
  demoSpeed();
}

void loop() {

  buttonState = digitalRead(BUTTON_PIN);
  bool isPressed = digitalRead(BUTTON_PIN) == HIGH;
  beep(isPressed);
  int currentTime = millis();

  if (isPressed && !down) { //then button is initially being pressed down (dot)
    down = true;
    downStart = currentTime;
    setActive();
  } else if (!isPressed && down) { //then button is released
    down = false;
    spaceStart = currentTime;
    registerCode(currentTime);
  } else if (isPressed && down) { //then button is currently being held down
    
  } else { //button is not currently being held down
    processSpace(currentTime);
  }
}