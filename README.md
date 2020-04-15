# Arduino Morse Code

This is a simple morse code translator, inspired by https://github.com/OwenK2/Ardunio-Morse-Code-Translator.
Using a button and passive buzzer, morse code symbols (. , _) can be entered. They are automatically translated into the appropriate character on the LCD screen (along with the entered symbols).

## Hardware
* Arduino Leonardo
* Button (Tactile switch)
* Passive Buzzer
* LCD display
* bundle of wires
* 10kohm resistor
* 330ohm resistor

## Software

I've used Platformio to build this, although it could easily be built using the Arduino IDE.
> Note: this project is built using [platformio](http://platformio.org) 

### Key variables

    LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

    const int LCD_ROWS = 2;
    const int LCD_COLS = 16;
    const int AUDIO_PIN = 6;
    const int BUTTON_PIN = 2;
    const int LED_PIN = 13;
    const int DOT_LEN = 200; 


## Usage
This device uses standard Morse Code practice, based on a single unit of time.

1. a dot is the length of 1 unit
2. a dash is the length of 3 units
3. space between elements (dot, dash) within a character separated by a unit of silence
4. space between characters is indicated by 3 units of silence
5. space between words is indicated by 7 units of silence

Pressing the button down for the appropriate length will register . or _. An appropriate length pause will trigger the elements to be translated into a character, and displayed on the LCD. spaces between words will be displayed on LCD as a space (but no more than one in a row). After 3 word-spaces length of time, the device presumes in activity and resets the screen and sleeps. It can be woken up by starting a new message.

## TODO
1. replace LCD control with I2C backpack to reduce pins required
2. replace LiquidCrystal library with one adequate for ATTiny85 (digispark)
3. reduce code size to fit on an ATTiny
4. add a potentiometer to control size of unit of time for dot / space