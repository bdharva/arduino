/*

PIEZO BUZZER TOOL

This code will allow a user to:
* Determine the approximate resonant frequency of a piezo buzzer
* Select from a range of pure pitches about the resonant frequency
* Set duration, spacing, and loops to compose a two pitch sound design

Author: Benjamin D. Harvatine
Started: Thursday, January 26, 2017
Updated: Thursday, February 2, 2017

*/

// Pins

#define LED_A 4
#define LED_B 5
#define LED_C 6
//#define LED_D 6
#define BTN 7
#define BUZ 9
#define POT 15

// Pot Readings

int reading, reading_map, last_reading;

// Timers

unsigned long timer_btn;
unsigned long timer_led;
unsigned long timer_buz;

// Time limits

long time_limit_btn = 250;
long time_limit_buz, time_limit_led;

// Output states

boolean state_buz, state_led;

// Charlieplexed LED array mapping

int highs[] = { LED_A, LED_B, LED_A, LED_C, LED_B, LED_C };
int lows[] = { LED_B, LED_A, LED_C, LED_A, LED_C, LED_B };

// Button interactions

boolean btn_active = false;
boolean btn_active_long = false;
boolean btn_pressed = false;
boolean btn_pressed_long = false;

// Pitches

int pitches[] = { 31,                                                       // B0
  33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,                           // C1 to B1
  65, 69, 73, 78, 82, 87, 93, 98, 104, 110, 117, 123,                       // C2 to B2
  131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,               // C3 to B3
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,               // C4 to B4
  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,               // C5 to B5
  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,   // C6 to B6
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,   // C7 to B7
  4186, 4435, 4699, 4978                                                    // C8 to D#8
};

// Mapped pitches

int pitch_range[6];

// Selected pitches

int pitch_1, pitch_1_duration;
int pitch_2, pitch_2_duration;

// Pages and sections

int page = 0, section = 0;
typedef void (* GenericFP)();
GenericFP pages[] = {&page_0, &page_1, &page_2, &page_3, &page_4};

// Current properties to render

int set_frequency, res_freq, led_count, pitch, set_duration;

/*
SETUP() FUNCTION
Set all LED pins to INPUT (high impedance), button and potentiometer to input,
and buzzer to output.
*/

void setup() {
  pinMode(LED_A, INPUT);
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);
  //pinMode(LED_D, INPUT);
  pinMode(BTN, INPUT);
  pinMode(BUZ, OUTPUT);
  pinMode(POT, INPUT);
}

/*
LOOP() FUNCTION
Checks for updated inputs on button and potentiometer, calls page function if
an input has been updated, and then calls the output functions for the LED's
and the buzzer.
*/

void loop() {
  reading = analogRead(POT);
  if (reading != last_reading || btn_pressed || btn_pressed_long) {
    pages[page]();
  }
  button();
  buzz(set_frequency, set_duration);
  illuminate(led_count);
  last_reading = reading;
}

/*
PAGE 0
User selects the resonant frequency. Then the program finds the nearest pitch
and assembles a pitch array centered around it.
*/

void page_0(){
  set_duration = 0;
  reading_map = map(reading, 0, 1023, 0, 5);
  led_count = map(reading, 0, 1023, 0, 5);
  led_count = 0;
  set_frequency = reading_map*1000;
  if (btn_pressed == true || btn_pressed_long == true) {
    res_freq = set_frequency;
    transition(1);
  }
}
void page_1(){
  led_count = 1;
  int target;
  for (int i = 0; i < sizeof(pitches) - 1; i++) {
    if (res_freq > pitches[i]) {
      target = i;
      break;
    }
  }
  int index = 0;
  for (int i=target-2; i < target+4; i++) {
    pitch_range[index] = pitches[i];
    index++;
  }
  transition(2);
}

/*
PAGE 1
User selects the pitch, volume, and duration for the first sound.
*/

void page_2(){
  // Selecting pitch
  reading_map = map(reading, 0, 1023, 0, 5);
  led_count = 2;
  set_frequency = pitch_range[reading_map];
  if(btn_pressed == true || btn_pressed_long == true) {
    pitch_1 = pitch_range[reading_map];
    transition(0);
  }
}

/*
PAGE 2
User selects the pitch, volume, and duration for the second sound.
*/

void page_3(){}

/*
PAGE 3
User selects the gap duration between sounds and selects how many times they
would like their sound pattern to be looped through.
*/

void page_4(){}

/*
PAGE 4
Plays the user-designed sound pattern of two pitches at defined volumes,
durations, and spacing.
*/

void page_5(){}

/*
BUTTON() FUNCTION
Tracks button presses, times button holds, and returns the state and recent
action(s) of the momentary push button.
*/

void button () {
  if (digitalRead(BTN) == LOW) {
    if (btn_active == false) {
      btn_active = true;
      timer_btn = millis();
    }
    if ((millis() - timer_btn > time_limit_btn) && (btn_active_long == false)) {
      btn_active_long = true;
    }
  } else {
    if (btn_active == true) {
      btn_active = false;
      btn_pressed = true;
    }
    if (btn_active_long == true) {
      btn_active_long = false;
      btn_pressed = false;
      btn_pressed_long = true;
    }
  }
}

/*
BUZZ() FUNCTION
Calculates the period and duty cycle to buzz the piezo at the desired frequency
and volume. Also uses a timer to sound buzzer for a given duration.
*/

void buzz (int pitch, int duration) {
  int period = 1000000/2*pitch;
  // These delays need to be replaced with timers
  analogWrite(BUZ, 255);
  delayMicroseconds(period);
  analogWrite(BUZ, 0);
  delayMicroseconds(period);
}

/*
ILLUMINATE() FUNCTION
Illuminates the target LED(s) via bit-banged PWM. Note: Need an LED refresh rate
of >50Hz for proper persistence of vision (no flickering).
*/

void illuminate (int target) {
  reset_pins();
  /*
  Right now this just illuminates a single target LED. When ready to illuminate
  all LED's up to and including the target, will need to implement some timers
  for bit-banging PWM. - BDH, 01/31/17
  */
  pinMode(highs[target], OUTPUT);
  pinMode(lows[target], OUTPUT);
  digitalWrite(highs[target], HIGH);
  digitalWrite(lows[target],LOW);
}

/*
RESET_PINS() FUNCTION
Sets all LED pins in Charlieplexed array to inputs and pulls them low.
*/

void reset_pins() {
  pinMode(LED_A, INPUT);
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);
  digitalWrite(LED_A, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
}

/*
TRANSITION() FUNCTION
Resets LED pins in Charleiplexed array and updates page/section.
*/

void transition(int new_page) {
  for (int j = 0; j < 100; j++) {
    for (int i = 0; i < 6; i++) {
      illuminate(i);
      delay(1);
    }
  }
  reset_pins();
  reset_button();
  page = new_page;
}

/*
RESET_BUTTON() FUNCTION
Resets the button press (short and long) states to FALSE.
*/

void reset_button() {
  btn_pressed = false;
  btn_pressed_long = false;
}
