# Piezo Tester Specifications

## PAGE 0

### Step 1: Set resonant frequency

User turns knob to set resonant frequency. LED's indicate steps of 1 kHz from 0 kHz to 11 kHz with one LED illuminated per 1 kHz.

### Step 2: Find nearest pitch

User presses button and the program loops through pitches.h to find the index of a pitch that is less than the set resonant frequency.

### Step 3: Find six pitches above and six below

The program stores the previous index, 5 steps below it, and 6 steps above it in an array. All LED's blink to indicate that the selection has been stored. The user presses the button to initiate the next step.

## PAGE 1

### Step 1: Define pitch

### Step 2: Define volume

### Step 3: Define duration

The user turns knob to select pitch from the array (frequency). The piezo will play the selected frequency. They press the button to toggle to volume (duty cycle). The piezo will play the selected frequency at the set volume. After selection, they press button again to toggle to duration. The piezo will play the selected frequency at the set volume on and off for the set duration. A short button press will now toggle back to frequency or a long press will toggle to the next selections.

## PAGE 2

### Step 1: Define pitch

### Step 2: Define volume

### Step 3: Define duration

The user turns knob to select pitch from the array (frequency). The piezo will play the selected frequency. They press the button to toggle to volume (duty cycle). The piezo will play the selected frequency at the set volume. After selection, they press button again to toggle to duration. The piezo will play the selected frequency at the set volume on and off for the set duration. A short button press will now toggle back to frequency or a long press will toggle to the next selections.

## PAGE 3
### Step 6: Define gaps and # of loops

Turning the knob will adjust the first gap in quarter second increments, illuminating the LEDs accordingly. The piezo will alternate between the two sounds with the set gap in between. Pressing the button will toggle to the next gap. The same behavior will apply, but now the second gap will match this value. Another button press will move to selection of loops in increments of 1. A short button press will loop back to the first gap or a long press will move to the next step.

##PAGE 4
### Step 7: Play loop

A single button press will start the loop. Another press will stop the loop. A long press will take the user back to step 4.

## Future Feature

* Serial interface for variable readout
* Persistent storage through EEPROM read/write

----

# State Machines (?)

* LED's: Need to track number of LED's and cycle through them. Requires a timer and a pointer?
* Buzzer: Need timers for duty on/off, as well as duration.
* Potentiometer: Poll and record updated readings.
* Button: Poll and record button press events, set a timer for long press events.
