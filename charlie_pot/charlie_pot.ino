#define LED_A 3
#define LED_B 4
#define LED_C 5
#define LED_D 6
#define BUT_A 7
#define BUZ_A 9
#define POT_A 15
#define POT_B 16
#define POT_C 17
#define POT_D 18

int highs[] = { LED_A, LED_B, LED_A, LED_C, LED_B, LED_C };
int lows[] = { LED_B, LED_A, LED_C, LED_A, LED_C, LED_B };
int i = 0;

void setup() {
  
  pinMode(LED_A, INPUT);
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);
  pinMode(BUT_A, INPUT);
  pinMode(BUZ_A, OUTPUT);
  pinMode(POT_A, INPUT);
  pinMode(POT_B, INPUT);

}

void loop() {

  i = analogRead(POT_A);
  i = map(i, 0, 1023, 0, 5);
  illuminate(i);
  delay(1);

}

void illuminate (int target) {
  
  reset_pins();

  pinMode(highs[target], OUTPUT);
  pinMode(lows[target], OUTPUT);

  digitalWrite(highs[target], HIGH);
  digitalWrite(lows[target],LOW);
  
}

void reset_pins() {
  
  pinMode(LED_A, INPUT); 
  pinMode(LED_B, INPUT);
  pinMode(LED_C, INPUT);

  digitalWrite(LED_A, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
  
}
