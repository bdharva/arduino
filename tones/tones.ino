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
String input_string = "";
boolean string_complete = false;
boolean play_notes = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to the piezo tool!");
  Serial.println("Create compositions with the following format:");
  Serial.println("[Octave],[Note],[Length],[Octave],[Note],[Length]");
  Serial.println("Octave is valid from 0 to 6, with 3 corresponding the the 4th (middle) octave.");
  Serial.println("Note is valid from 0 to 11 as follows:");
  for(int i = 0; i < (sizeof(notes)/sizeof(String)); i++){
    if (i > 0) { Serial.print(", "); }
    Serial.print(notes[i]);
    Serial.print(":");
    Serial.print(i);
  }
  Serial.print("\n");
  Serial.println("Length determines the note as follows: a length of 8 makes a 1/8 note, 4 makes 1/4 note, etc.");
  Serial.println("------");
  input_string.reserve(200);
  // Fur Elise
  //int melody[] = { pitches[52], pitches[51], pitches[52], pitches[51], pitches[52], pitches[47], pitches[50], pitches[48], pitches[45] };
  //int durations[] = { 6, 6, 6, 6, 6, 6, 6, 6, 6 };
  // NBC chimes
  int melody[] = { pitches[55], pitches[64], pitches[60] };
  int durations[] = { 4, 4, 2 };
  for (int this_note = 0; this_note < (sizeof(melody)/sizeof(int)); this_note++) {
    int duration = 1000 / durations[this_note];
    tone(BUZ, melody[this_note], duration);
    int pause = duration * 1.30;
    delay(pause);
    noTone(BUZ);
  }
}

void loop() {
  if (string_complete) {
    parse_input(input_string);
    input_string = "";
    string_complete = false;
    play_notes = true;
  }
  if (play_notes == true) {
    int melody[] = {pitches[12*a+b], pitches[12*d+e]};
    int durations[] = {c, f};
    for (int this_note = 0; this_note < (sizeof(melody)/sizeof(int)); this_note++) {
      int duration = 1000 / durations[this_note];
      tone(BUZ, melody[this_note], duration);
      int pause = duration * 1.30;
      delay(pause);
      noTone(BUZ);
    }
    delay(1000);
  }
}

void parse_input(String inputs) {
  Serial.print("Input: ");
  Serial.print(inputs);
  Serial.print("\n");
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
  /*
  //http://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string/1237
  // Calculate based on max input size expected for one command
  #define INPUT_SIZE 30
  ...

  // Get next command from Serial (add 1 for final 0)
  char input[INPUT_SIZE + 1];
  byte size = Serial.readBytes(input, INPUT_SIZE);
  // Add the final 0 to end the C string
  input[size] = 0;

  // Read each command pair
  char* command = strtok(input, "&");
  while (command != 0)
  {
      // Split the command in two values
      char* separator = strchr(command, ':');
      if (separator != 0)
      {
          // Actually split the string in 2: replace ':' with 0
          *separator = 0;
          int servoId = atoi(command);
          ++separator;
          int position = atoi(separator);

          // Do something with servoId and position
      }
      // Find the next command in input string
      command = strtok(0, "&");
  }*/
}

void serialEvent() {
  while (Serial.available() && string_complete == false) {
    char input_char = (char)Serial.read();
    input_string += input_char;
    if (input_char == '\n') {
      string_complete = true;
    }
  }
}
