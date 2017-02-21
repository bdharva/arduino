/*

PIEZO BUZZER TOOL

This code will allow a user to:
* Determine the approximate resonant frequency of a piezo buzzer
* Select from a range of pure pitches about the resonant frequency
* Set duration, spacing, and loops to compose a two pitch sound design

Author: Benjamin D. Harvatine
Started: Thursday, January 26, 2017
Updated: Monday, February 20, 2017

*/

// Debug Scaffolding

#define DEBUG 0

// Pins

#define LED_A 3
#define LED_B 4
#define LED_C 5
#define LED_D 6

#define BUZ 10
#define BTN 12
#define POT 0

// Buzzer state machine variables

#define BUZ_OFF 0
#define BUZ_HI 1
#define BUZ_HI_WAIT 2
#define BUZ_LO 3
#define BUZ_LO_WAIT 4

#define BUZ_TIMEOUT_DUT OCR2A
uint8_t fsm_buz_state = BUZ_OFF;
uint16_t fsm_buz_timer_dut, fsm_buz_timer_dur, fsm_buz_timeout_dut, fsm_buz_timeout_dur;

// Button state machine variables

#define BTN_INACTIVE 0
#define BTN_ACTIVE 1
#define BTN_LONG 2

uint8_t fsm_btn_state = BTN_INACTIVE;
uint16_t fsm_btn_timer, fsm_btn_timeout;
bool btn_pressed, btn_pressed_long;

// LED array state machine variables

#define LED_OFF 0
#define LED_ON 1
#define LED_ON_WAIT 2
#define LED_BLINK 3
#define LED_BLINK_WAIT 4
#define LED_ONS 5
#define LED_ONS_WAIT 6
#define LED_BLINKS 7
#define LED_BLINKS_WAIT 8

uint8_t fsm_led_state = LED_OFF;
uint16_t fsm_led_timer_dut, fsm_led_timer_dur, fsm_led_timeout_dut, fsm_led_timeout_dur;
int set_led, current_led;

// UI state machine variables

#define UI_RES_FREQ 0
#define UI_PITCH_1 1
#define UI_VOLUME_1 2
#define UI_DURATION_1 3
#define UI_PITCH_2 4
#define UI_VOLUME_2 5
#define UI_DURATION_2 6
#define UI_GAP_1 7
#define UI_GAP_2 8
#define UI_LOOPS 9
#define UI_PLAY 10

uint8_t fsm_ui_state = UI_RES_FREQ;

// Charlieplexed LED array mapping

int highs[] = { LED_A, LED_B, LED_C, LED_D, LED_A, LED_B, LED_C, LED_D, LED_A, LED_B, LED_C, LED_D };
int lows[] = { LED_B, LED_A, LED_D, LED_C, LED_C, LED_D, LED_A, LED_B, LED_D, LED_C, LED_B, LED_A };

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

// Current properties to render

unsigned long current_micros, current_millis;
int current_pot = 0;
int last_pot = 0;
int res_freq, set_freq, set_vol, set_dur, set_duty_on, set_duty_off;

// Mapped pitches

int pitch_range[6];

// Selected pitches

int pitch_1, pitch_1_vol, pitch_1_dur, pitch_1_gap;
int pitch_2, pitch_2_vol, pitch_2_dur, pitch_2_gap;
int loops;

unsigned long waiting = 0;
unsigned long waited = 1000;

/*
SETUP() FUNCTION
Set all LED pins to INPUT (high impedance), button and potentiometer to input,
and buzzer to output.
*/

void setup() {

  pinMode(LED_A, INPUT);
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);
  pinMode(LED_D, INPUT);
  pinMode(BTN, INPUT);
  pinMode(BUZ, OUTPUT);
  pinMode(POT, INPUT);
  pinMode(13, OUTPUT);

  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  cli();//stop interrupts
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
  
  start_buzzer(5000, 100, 1000);

}

ISR(TIMER1_COMPA_vect){
  if (fsm_buz_state == 1) {
    Serial.println("Low");
    digitalWrite(13, LOW);
    //OCR1A = 3124;
    digitalWrite(BUZ, LOW);
    fsm_buz_state = 0;
  } else if (fsm_buz_state == 0) {
    Serial.println("High");
    digitalWrite(13, HIGH);
    //OCR1A = 1000;
    digitalWrite(BUZ, HIGH);
    fsm_buz_state = 1;
  }
  //buzzer_state();
}

/*
LOOP() FUNCTION
Checks for updated inputs on button and potentiometer, then updates UI state
and corresponding buzzer and LED states.
*/

void loop() {

  /*current_micros = micros();
  current_millis = millis();
  Serial.print("-----\n\nLoop Start: ");
  Serial.print(micros());
  Serial.print("us\n");
  //current_pot = analogRead(POT);
  //if (last_pot == 0) { current_pot = 1; }
  //button_state();
  if (current_pot != last_pot || btn_pressed == true || btn_pressed_long == true) {
    ui_state();
  }
  buzzer_state();
  last_pot = current_pot;
  Serial.print("Loop End: ");
  Serial.print(micros());
  Serial.print("us\n");*/
  
}

/*
START_BUZZER() FUNCTION
Takes arguments for frequency, volume, and duration
*/

void start_buzzer(int target_freq, int target_vol, int target_dur) {

  int period = 500000/target_freq; // period in us
  set_duty_on = period/2*(target_vol/100);
  set_duty_off = period/2*(2-(target_vol/100));
  set_freq = target_freq;
  fsm_buz_state = BUZ_HI;
  fsm_buz_timer_dur = current_millis;
  fsm_buz_timeout_dur = target_dur;
  #ifdef DEBUG
    Serial.println("Starting buzzer...");
    Serial.print("Frequency: ");
    Serial.print(target_freq);
    Serial.print("Hz\nVolume: ");
    Serial.print(target_vol);
    Serial.print("%\nDuration: ");
    Serial.print(target_dur);
    Serial.print("ms\n");
  #endif

}

/*
START_LEDS() FUNCTION
Takes arguments for target LED, period, duration, and bool for multiple LED's
*/

void start_leds(int target, int target_per, int target_dur, bool multi_led) {

  set_led = target;
  fsm_led_timeout_dut = target_per;
  fsm_led_timeout_dur = target_dur;
  if (multi_led == true) {
    fsm_led_state = LED_ONS;
  } else {
    fsm_led_state = LED_ON;
  }
  fsm_led_timer_dur = current_millis;

}

// BUTTON STATE MACHINE

void button_state() {

  switch(fsm_btn_state) {

    case BTN_INACTIVE:
      fsm_btn_timer = fsm_btn_timeout = 0;
      btn_pressed = btn_pressed_long = false;
      if (digitalRead(BTN) == LOW) {
        fsm_btn_timer = millis();
        fsm_btn_state = BTN_ACTIVE;
        break;
      }
      break;

    case BTN_ACTIVE:
      if (digitalRead(BTN) == HIGH) {
        btn_pressed = true;
        fsm_btn_timer = 0;
        fsm_btn_state = BTN_INACTIVE;
        break;
      }
      if (current_millis - fsm_btn_timer > fsm_btn_timeout) {
        fsm_btn_timer = 0;
        fsm_btn_state = BTN_LONG;
        break;
      }
      break;

    case BTN_LONG:
      if (digitalRead(BTN) == HIGH) {
        btn_pressed_long = true;
        fsm_btn_state = BTN_INACTIVE;
        break;
      }
      break;
  }

}

// UI STATE MACHINE

void ui_state() {

  switch (fsm_ui_state) {

    case UI_RES_FREQ:
      set_freq = map(current_pot, 0, 1023, 0, 1000*(sizeof(highs)-1));
      set_vol = 100;
      //start_buzzer(set_freq, set_vol, set_dur);
      set_led = map(current_pot, 0, 1023, 0, sizeof(highs)-1);
      start_leds(set_led, 1000, 0, true);
      if (btn_pressed == true || btn_pressed_long == true) {
        res_freq = set_freq;
        int target;
        for (int i = 0; i < sizeof(pitches) - 1; i++) {
          if (res_freq < pitches[i]) {
            target = i;
            break;
          }
        }
        int index = 0;
        for (int i=target-(sizeof(highs)/2-1); i < target+(sizeof(highs)/2); i++) {
          pitch_range[index] = pitches[i];
          index++;
        }
        fsm_ui_state = UI_PITCH_1;
        break;
      }
      break;

    case UI_PITCH_1:
      set_freq = map(current_pot, 0, 1023, 0, sizeof(highs)-1);
      set_freq = pitch_range[set_freq];
      //start_buzzer(set_freq, set_vol, set_dur);
      if(btn_pressed == true || btn_pressed_long == true) {
        pitch_1 = pitch_range[set_freq];
        fsm_ui_state = UI_VOLUME_1;
        break;
      }
      break;

    case UI_VOLUME_1:
      set_vol = map(current_pot, 0, 1023, 0, 100);
      start_buzzer(set_freq, set_vol, set_dur);
      if(btn_pressed == true || btn_pressed_long == true) {
        pitch_1_vol = set_vol;
        //fsm_ui_state = UI_DURATION_1; Left off here
        break;
      }
      break;

  }

}

// BUZZER STATE MACHINE

void buzzer_state() {

  #ifdef DEBUG
  Serial.print(fsm_buz_timer_dut);
  Serial.print("/");
  Serial.print(fsm_buz_timeout_dut);
  Serial.print("us at ");
  Serial.print(micros());
  Serial.print("us\n");
  #endif

  switch (fsm_buz_state) {
    
    case BUZ_OFF:
      //fsm_buz_timer_dut = fsm_buz_timer_dur = fsm_buz_timeout_dut = fsm_buz_timeout_dur = 0;
      analogWrite(BUZ, 0);
      break;

    case BUZ_HI:
      //fsm_buz_timer_dut = current_micros;
      //fsm_buz_timeout_dut = set_duty_on;
      /*if (fsm_buz_timeout_dur > 0 && current_millis - fsm_buz_timer_dur >= fsm_buz_timeout_dur) {
        fsm_buz_timer_dur = 0;
        fsm_buz_state = BUZ_OFF;
        #ifdef DEBUG
        Serial.println("BUZ_OFF...");
        #endif
        break;
      }*/
      analogWrite(BUZ, 255);
      break;

    /*case BUZ_HI_WAIT:
      if (fsm_buz_timeout_dur > 0 && current_millis - fsm_buz_timer_dur >= fsm_buz_timeout_dur) {
        fsm_buz_timer_dur = 0;
        fsm_buz_state = BUZ_OFF;
        #ifdef DEBUG
        Serial.println("BUZ_OFF...");
        #endif
        break;
      }
      if (current_micros - fsm_buz_timer_dut >= fsm_buz_timeout_dut) {
        fsm_buz_timer_dut = 0;
        fsm_buz_state = BUZ_LO;
        #ifdef DEBUG
        Serial.println(current_micros - fsm_buz_timer_dut);
        Serial.println("BUZ_LO...");
        #endif
        break;
      }
      break;*/

    case BUZ_LO:
      //fsm_buz_timer_dut = current_micros;
      //fsm_buz_timeout_dut = set_duty_off;
      /*if (fsm_buz_timeout_dur > 0 && current_millis - fsm_buz_timer_dur >= fsm_buz_timeout_dur) {
        fsm_buz_timer_dur = 0;
        fsm_buz_state = BUZ_OFF;
        #ifdef DEBUG
        Serial.println("BUZ_OFF...");
        #endif
        break;
      }*/
      analogWrite(BUZ, 0);
      break;

    /*case BUZ_LO_WAIT:
      if (fsm_buz_timeout_dur > 0 && current_millis - fsm_buz_timer_dur >= fsm_buz_timeout_dur) {
        fsm_buz_timer_dur = 0;
        fsm_buz_state = BUZ_OFF;
        #ifdef DEBUG
        Serial.println("BUZ_OFF...");
        #endif
        break;
      }
      if (current_micros - fsm_buz_timer_dut >= fsm_buz_timeout_dut) {
        fsm_buz_timer_dut = 0;
        fsm_buz_state = BUZ_HI;
        #ifdef DEBUG
        Serial.println(current_micros - fsm_buz_timer_dut);
        Serial.println("BUZ_HI...");
        #endif
        break;
      }
      break;*/
  }

}

// LED STATE MACHINE

void led_state() {

  switch (fsm_led_state) {

    case LED_OFF:
      pinMode(LED_A, INPUT);
      pinMode(LED_B, INPUT);
      pinMode(LED_C, INPUT);
      pinMode(LED_D, INPUT);
      digitalWrite(LED_A, LOW);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_C, LOW);
      digitalWrite(LED_D, LOW);
      fsm_led_timer_dut = fsm_led_timer_dur = fsm_led_timeout_dut = fsm_led_timeout_dur = 0;
      set_led = current_led = 0;
      break;

    case LED_ON:
      fsm_led_timer_dut = current_micros;
      if (fsm_led_timer_dur > 0 && current_millis - fsm_led_timer_dur >= fsm_led_timeout_dur) {
        fsm_led_state = LED_BLINK;
        break;
      }
      pinMode(highs[set_led], OUTPUT);
      pinMode(lows[set_led], OUTPUT);
      digitalWrite(highs[set_led], HIGH);
      digitalWrite(lows[set_led],LOW);
      fsm_led_state = LED_ON_WAIT;
      break;

    case LED_ON_WAIT:
      if (fsm_led_timer_dur > 0 && current_millis - fsm_led_timer_dur >= fsm_led_timeout_dur) {
        fsm_led_state = LED_OFF;
        break;
      }
      if (fsm_led_timeout_dut > 0 && current_millis - fsm_led_timer_dut >= fsm_led_timeout_dut) {
        fsm_led_state = LED_BLINK;
        break;
      }
      break;

    case LED_BLINK:
      fsm_led_timer_dut = current_micros;
      if (fsm_led_timer_dur > 0 && current_millis - fsm_led_timer_dur >= fsm_led_timeout_dur) {
        fsm_led_state = LED_OFF;
        break;
      }
      pinMode(LED_A, INPUT);
      pinMode(LED_B, INPUT);
      pinMode(LED_C, INPUT);
      pinMode(LED_D, INPUT);
      digitalWrite(LED_A, LOW);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_C, LOW);
      digitalWrite(LED_D, LOW);
      fsm_led_state = LED_BLINK_WAIT;
      break;

    case LED_BLINK_WAIT:
      if (fsm_led_timer_dur > 0 && current_millis - fsm_led_timer_dur >= fsm_led_timeout_dur) {
        fsm_led_state = LED_OFF;
        break;
      }
      if (current_millis - fsm_led_timer_dut >= fsm_led_timeout_dut) {
        fsm_led_state = LED_ON;
        break;
      }
      break;

    case LED_ONS:
      fsm_led_timer_dut = current_micros;
      if (fsm_led_timer_dur > 0 && current_millis - fsm_led_timer_dur >= fsm_led_timeout_dur) {
        fsm_led_state = LED_OFF;
        break;
      }
      current_led = 0;
      pinMode(highs[current_led], OUTPUT);
      pinMode(lows[current_led], OUTPUT);
      digitalWrite(highs[current_led], HIGH);
      digitalWrite(lows[current_led],LOW);
      fsm_led_state = LED_ONS_WAIT;
      break;

    case LED_ONS_WAIT:
      if (fsm_led_timeout_dut > 0 && fsm_led_timer_dur > 0 && current_millis - fsm_led_timer_dur >= fsm_led_timeout_dur) {
        fsm_led_state = LED_OFF;
        break;
      }
      if (current_millis - fsm_led_timer_dut >= fsm_led_timeout_dut) {
        fsm_led_state = LED_BLINKS;
        break;
      }
      current_led ++;
      if (current_led > set_led) {
        current_led = 0;
      }
      pinMode(highs[current_led], OUTPUT);
      pinMode(lows[current_led], OUTPUT);
      digitalWrite(highs[current_led], HIGH);
      digitalWrite(lows[current_led],LOW);
      break;

    case LED_BLINKS:
      fsm_led_timer_dut = current_micros;
      if (fsm_led_timer_dur > 0 && current_millis - fsm_led_timer_dur >= fsm_led_timeout_dur) {
        fsm_led_state = LED_OFF;
        break;
      }
      pinMode(LED_A, INPUT);
      pinMode(LED_B, INPUT);
      pinMode(LED_C, INPUT);
      pinMode(LED_D, INPUT);
      digitalWrite(LED_A, LOW);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_C, LOW);
      digitalWrite(LED_D, LOW);
      fsm_led_state = LED_BLINKS_WAIT;
      break;

    case LED_BLINKS_WAIT:
      if (fsm_led_timer_dur > 0 && current_millis - fsm_led_timer_dur >= fsm_led_timeout_dur) {
        fsm_led_state = LED_OFF;
        break;
      }
      if (current_millis - fsm_led_timer_dut >= fsm_led_timeout_dut) {
        fsm_led_state = LED_ONS;
        break;
      }
      break;

  }

}
