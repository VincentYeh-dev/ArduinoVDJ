#include <VMIDI.h>
#include <SoftwareSerial.h>
#define DEBUG false
int privious_values[2][8];
int privious_M_slide_value;

VMIDI HardWare_MIDI(Serial);

void setup() {
  Serial.begin(115200);
  privious_M_slide_value=-1;
  for (int i = 0; i < 8; i++) {
    privious_values[0][i] = -1;
    privious_values[1][i] = -1;
  }

  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);

}


unsigned long timer = 0;
void loop() {

  for (int i = 0; i < 8; i++) {
    select(i);
    int curr_value[2];
    curr_value[0] = analogRead(A0);
    curr_value[1] = analogRead(A1);
    for (byte side = 0; side < 2; side++) {
      if (i != 3 && privious_values[side][i] != (ATD(curr_value[side]) == 1 ? 0 : 127)) {
        byte val = ATD(curr_value[side]) == 1 ? 0 : 127;
        privious_values[side][i] = val;
        if (val == 127) {
          if (!DEBUG)HardWare_MIDI.sendNoteOn(0x39+side+i*2, val, 1);
        } else if (val == 0) {
          if (!DEBUG)HardWare_MIDI.sendNoteOff(0x39+side+i*2, val, 1);
        }
      } else if (i == 3) {
        int analog_val = map(curr_value[side], 0, 1023, 0, 127);
        if (range(privious_values[side][i], analog_val, 1) != privious_values[side][i]) {
          privious_values[side][i] = analog_val;
          if (!DEBUG)HardWare_MIDI.sendCtrlChange(0x39+side+i*2, privious_values[side][i], 1);
        }
      }
    }
    if(DEBUG)for(byte si=0;si<2;si++){
      Serial.print(String(privious_values[si][i] + i * 150+si*150*8));
      Serial.print(",");
    }
  }
  if(DEBUG)Serial.println();
  
  int analog_val = map(analogRead(A2), 0, 1023, 0, 127);
  if (range(privious_M_slide_value, analog_val, 1) != privious_M_slide_value) {
    privious_M_slide_value = analog_val;
    if (!DEBUG)HardWare_MIDI.sendCtrlChange(0x57, privious_M_slide_value, 1);
  }
  
  
}

void printChart(int val) {
  Serial.print(val);
  Serial.print(",");
}
int range(int privious, int current, int d) {
  if (current == 127)return 127;
  else if (current == 0)return 0;
  if (current > privious && current - d == privious)return privious;
  else if (current < privious && privious - d == current)return privious;
  else if (current == privious)return privious;
  return current;
}
bool ATD(int value) {
  if (((float)value * 0.0048828125) > 2.94) {
    return 1;
  } else if (((float)value * 0.0048828125) <= 1.47) {
    return 0;
  }

}
short select(int select) {
  //C:4
  //B:3
  //A:2
  for (int inputPin = 0; inputPin < 3; inputPin++)
  {
    int pinState = bitRead(select, inputPin);
    digitalWrite(inputPin + 2, pinState);
  }
}
