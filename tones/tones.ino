#define BUZ 10

int pitches[] = {
  33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,                           // C1 to B1
  65, 69, 73, 78, 82, 87, 93, 98, 104, 110, 117, 123,                       // C2 to B2
  131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,               // C3 to B3
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,               // C4 to B4
  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,               // C5 to B5
  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,   // C6 to B6
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951    // C7 to B7
};

String notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

int a, b, c, d, e, f;
String inputString = "";
boolean stringComplete = false;
boolean playNotes = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to the piezo tool!");
  Serial.println("Create compositions with the following format:");
  Serial.println("[Range],[Note],[Length],[Range],[Note],[Length]");
  Serial.println("Range can vary from 0 to 6");
  Serial.println("Note can vary from 0 to 11 as follows:");
  for(int i = 0; i < (sizeof(notes)/sizeof(String)); i++){
    if (i > 0) { Serial.print(", "); }
    Serial.print(notes[i]);
    Serial.print(":");
    Serial.print(i);
  }
  Serial.print("\n");
  Serial.println("------");
  inputString.reserve(200);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    parse_input(inputString);
    Serial.println(inputString);
    inputString = "";
    stringComplete = false;
    playNotes = true;
  }
  if (playNotes == false) {
    int melody[] = {
      pitches[36], pitches[31], pitches[31], pitches[33], pitches[31], 0, pitches[35], pitches[36]
    };
    
    // note durations: 4 = quarter note, 8 = eighth note, etc.:
    int noteDurations[] = {
      4, 8, 8, 4, 4, 4, 4, 4
    };
    for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(int)); thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(BUZ, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(BUZ);
    }
    delay(1000);
  }
  if (playNotes == true) {
    int melody[] = {pitches[12*a+b], pitches[12*d+e]};
    int noteDurations[] = {c, f};
    for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(int)); thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(BUZ, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(BUZ);
    }
    delay(1000);
  }
}

void parse_input(String inputs) {
  if (sscanf(inputs.c_str(), "%d,%d,%d,%d,%d,%d", &a, &b, &c, &d, &e, &f) == 6) {
    Serial.print("Note 1: ");
    Serial.print(notes[b]);
    Serial.print(a+1);
    Serial.print(", 1/");
    Serial.print(c);
    Serial.print(" note");
    Serial.print("\n");
    Serial.print("Note 2: ");
    Serial.print(notes[e]);
    Serial.print(d+1);
    Serial.print(", 1/");
    Serial.print(f);
    Serial.print(" note");
    Serial.print("\n");
  }
}

void serialEvent() {
  while (Serial.available() && stringComplete == false) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
