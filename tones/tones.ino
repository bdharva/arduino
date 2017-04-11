#define BUZ 10
#define NOTE 16*150

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

const byte num_chars = 32;
char input_string[num_chars];
boolean string_complete = false;

char command[32] = {0};
int gap = 0;
int octave = 0;
int tones[32] = {0};
int durations[32] = {0};
boolean play_notes = false;

void setup() {
 Serial.begin(9600);
 Serial.println("Welcome to the Piezo Tool (try 'help' for a list of commands)...\n");
 parse_input();
}


void loop() {
  if (string_complete) {
    parse_input();
    
    string_complete = false;
  }
  if (play_notes == true) {
    for (int this_note = 0; this_note < (sizeof(tones)/sizeof(int)); this_note++) {
      if (durations[this_note] > 0) {
        int duration = NOTE / durations[this_note];
        tone(BUZ, pitches[(octave-1)*12+tones[this_note]], duration);
        int pause = duration*1.3;
        delay(pause);
        noTone(BUZ);
      } else {
        break;
      }
    }
    delay(gap);
  }
}

 
void parse_input() {
  char *token = strtok(input_string, ",");
  strcpy(command, token);
  if (String(command) == "gap") {
    token = strtok(NULL, ",");
    gap = atoi(token);
  } else if (String(command) == "octave") {
    token = strtok(NULL, ",");
    octave = atoi(token);
  } else if (String(command) == "stats") {
    print_input();
  } else if (String(command) == "start") {
    play_notes = true;
  } else if (String(command) == "stop") {
    play_notes = false;
  } else if (String(command) == "help") {
    Serial.println("\nList of commands:");
    Serial.println("\n  gap: \t'gap,x' where x is an integer with units of milliseconds");
    Serial.println("\n  octave: \t'octave,x' where x is an integer 1-7");
    Serial.print("\n  notes: \t'notes,a,x,a,x,...' where 'a' represents a musical note and 'x' represents the note size");
    Serial.print("\n\tas follows: a length of 8 makes a 1/8 note, 4 makes 1/4 note, etc. Valid musical notes ");
    Serial.print("\n\tare: ");
    for(int i = 0; i < (sizeof(notes)/sizeof(String)); i++){
      if (i > 0) { Serial.print(", "); }
      Serial.print(notes[i]);
    }
    Serial.print("\n");
    Serial.println("\n  start: \t'start' to start playing sounds");
    Serial.println("\n  stats: \t'stats' to view all currently defined paramters");
    Serial.println("\n  stop: \t'stop' to stop playing sounds");
  } else if (String(command) == "notes") {
    token = strtok(NULL, ",");
    int index = 0;
    for(int i = 0; i < sizeof(tones)/sizeof(int); i++){
      tones[i] = 0;
    }
    for(int i = 0; i < sizeof(durations)/sizeof(int); i++){
      durations[i] = 0;
    }
    while (token != NULL) {
      if (index % 2 == 0) {
        int target = 0;
        for (int i=0; i < sizeof(notes)/sizeof(String); i++) {
          String str = String(token);
          str.toUpperCase();
          if (notes[i] == str) {
            target = i;
            break;
          }
        }
        tones[index/2] = target;
      } else {
        durations[(index-1)/2] = atoi(token);
      }
      index ++;
      token = strtok(NULL, ",");
    }
  }
  for(int i = 0; i < sizeof(input_string); i++ ){
    input_string[i] = "\0";
  }
}

void print_input() {
  Serial.print("\n  Gap: \t");
  Serial.print(gap); 
  Serial.println("ms");
  Serial.print("\n  Octave: \t");
  Serial.print(octave);
  if (octave == 1) {
    Serial.print("st ");
  } else if (octave == 2) {
    Serial.print("nd ");
  } else if (octave == 3) {
    Serial.print("rd ");
  } else {
    Serial.print("th ");
  }
  Serial.println("octave");
  Serial.print("\n  Notes: ");
  for(int i = 0; i < sizeof(tones)/sizeof(int); i++){
    if(durations[i] != 0) {
      Serial.print("\t");
      Serial.print(notes[tones[i]]);
      Serial.print(", ");
      Serial.print("1/");
      Serial.print(durations[i]);
      Serial.println(" note");
    } else {
      if (i == 0) {
        Serial.println("\tNo notes defined.");
      }
      break;
    }
  }
}

void serialEvent() {
  static byte index = 0;
  char input_char;
  while (Serial.available() && string_complete == false) {
    input_char = Serial.read();
    if (input_char == '\n') {
      input_string[index] = '\0';
      index = 0;
      Serial.print("\n> ");
      Serial.println(input_string);
      string_complete = true;
    } else {
      input_string[index] = input_char;
      index++;
      if (index >= num_chars) {
        index = num_chars - 1;
      }
    }
  }
}
