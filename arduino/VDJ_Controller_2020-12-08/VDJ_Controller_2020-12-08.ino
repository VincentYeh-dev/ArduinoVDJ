#define DEBUG false
//You can use Arduino Serial Plotter for debug by changing DEBUG to true,but it will make your MIDI board unable to send MIDI Message.

#define VALUE_MIN_RANGE 1
#define DEBOUNCE_MS 100
#define INPUT_TYPE_ANALOG 0
#define INPUT_TYPE_DIGITAL 127

#define MIDI_MSG_NOTE_OFF 128 //or 0x80
#define MIDI_MSG_NOTE_ON 144 //or 0x90
#define MIDI_MSG_CTRL_CHANGE 176 //or 0xB0

const byte LED_PIN[] = {2, 3, 4, 8, 9, 10, 11, 12};

const byte INPUT_TYPE_MAP[2][8] = {
  {
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_ANALOG,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL
  },
  {
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_ANALOG,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL,
    INPUT_TYPE_DIGITAL
  }
};

byte previous_values[2][8];
byte previous_M_slide_value;
unsigned long btn_timer[2][8];
//unsigned long timer = 0;

void setup() {

  Serial.begin(115200);
  previous_M_slide_value = -1;
  for (int i = 0; i < 8; i++) {
    previous_values[0][i] = -1;
    previous_values[1][i] = -1;
    btn_timer[0][i] = 0;
    btn_timer[1][i] = 0;
  }

  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);


  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

}

void loop() {
  byte input[3] = {0, 0, 0};
  if (Serial.available() > 2) {
    Serial.readBytes(input, 3);
    byte statusByte = input[0];
    if (statusByte >= 0x90 && statusByte < 0xA0) {
      byte chennal = statusByte - 0x90;
      digitalWrite(LED_PIN[input[1]],input[2]==127?HIGH:LOW);
    }
  }
  for (int i = 0; i < 8; i++) {
    select(i);
    int curr_value[2];
    curr_value[0] = analogRead(A0);
    curr_value[1] = analogRead(A1);
    for (byte side = 0; side < 2; side++) {
      if (INPUT_TYPE_MAP[side][i] == INPUT_TYPE_DIGITAL) {
        if (previous_values[side][i] != (ATD(curr_value[side]) == 1 ? 0 : 127) && millis() >= btn_timer[side][i]) {

          //        When button is pressed.
          byte val = ATD(curr_value[side]) == 1 ? 0 : 127;
          previous_values[side][i] = val;
          if (val == 127) {
            if (!DEBUG)sendNoteOn(0x39 + side + i * 2, val, 1);
          } else if (val == 0) {
            if (!DEBUG)sendNoteOff(0x39 + side + i * 2, val, 1);
            btn_timer[side][i] = millis() + DEBOUNCE_MS;
          }
        }

      } else if (INPUT_TYPE_MAP[side][i] == INPUT_TYPE_ANALOG) {

        int analog_val = map(curr_value[side], 0, 1023, 0, 127);
        if (range(previous_values[side][i], analog_val, VALUE_MIN_RANGE) != previous_values[side][i]) {
          //        When slider value is changed.
          previous_values[side][i] = analog_val;
          if (!DEBUG)sendCtrlChange(0x39 + side + i * 2, previous_values[side][i], 1);
        }
      }
    }
    if (DEBUG)for (byte si = 0; si < 2; si++) {
        Serial.print(String(previous_values[si][i] + i * 150 + si * 150 * 8));
        Serial.print(",");
      }
  }
  if (DEBUG)Serial.println();

  int analog_val = map(analogRead(A2), 0, 1023, 0, 127);
  if (range(previous_M_slide_value, analog_val, VALUE_MIN_RANGE) != previous_M_slide_value) {
    previous_M_slide_value = analog_val;
    if (!DEBUG)sendCtrlChange(0x57, previous_M_slide_value, 1);
  }


}


void printChart(int val) {
  Serial.print(val);
  Serial.print(",");
}
int range(int previous, int current, int d) {
  //  This method compare previous value and current value.
  //  If current value only increase or decrease "d",this method will return previous value.
  //  If current value increase or decrease "d" + >0 value ,this method will return current value.

  //example:
  //  previous=5;current=4;d=1
  //  return 5

  //  previous=5;current=3;d=1
  //  return 3

  if (current == 127)return 127;
  else if (current == 0)return 0;
  if (current > previous && current - d == previous)return previous;
  else if (current < previous && previous - d == current)return previous;
  else if (current == previous)return previous;
  return current;
}

bool ATD(int analog_val) {
  //  Analog to digital
  //  analog_val:0~1023
  //  return 0 or 1.
  if (((float)analog_val * 0.0048828125) > 2.94) {
    return 1;
  } else if (((float)analog_val * 0.0048828125) <= 1.47) {
    return 0;
  }
}

void select(int select) {
  //  CD4051 select
  //  C (MSB) : S2  13  D7
  //  B       : S1  12  D6
  //  A (LSB) : S0  11  D5

  for (int inputPin = 0; inputPin < 3; inputPin++)
  {
    int pinState = bitRead(select, inputPin);
    digitalWrite(inputPin + 5, pinState);
  }
}

//MIDI------------------------------------------
void sendNoteOn(byte note, byte velocity, byte channel) {
  sendChannelVoiceMsg(MIDI_MSG_NOTE_ON | (channel - 1), note, velocity);

}

void sendNoteOff(byte note, byte velocity, byte channel) {
  sendChannelVoiceMsg(MIDI_MSG_NOTE_OFF | (channel - 1), note, velocity);
}

void sendCtrlChange(byte cc, byte value, int channel) {
  sendChannelVoiceMsg(MIDI_MSG_CTRL_CHANGE | (channel - 1), cc, value);
}

void sendChannelVoiceMsg(byte command, byte note, byte velocity) {
  //send MIDI Channel Voice Message
  Serial.write(command);
  Serial.write(note);
  Serial.write(velocity);
}
