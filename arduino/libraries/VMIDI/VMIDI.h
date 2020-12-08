#ifndef VMIDI_h
#define VMIDI_h
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "HardwareSerial.h"
#define BAUD 115200
#define MIDI_MSG_NOTE_ON 0x90
#define MIDI_MSG_NOTE_OFF 0x80
#define MIDI_MSG_CTRL_CHANGE 0xB0

class VMIDI {
 public:
	VMIDI(HardwareSerial &print);
	VMIDI(SoftwareSerial &print);
	void sendNoteOn(byte note,byte velocity,int channel);
	void sendNoteOff(byte note,byte velocity,int channel);
	void sendCtrlChange(byte cc,byte value,int channel);
	int read();
	int available();
	void setTimeout(long timeout);
	byte readBytes(byte* buffer,int length);
	
 private:
	HardwareSerial* MIDISerial;
	//SoftwareSerial* MIDISerial;
	void send_msg(int command, byte note, byte velocity);
};

#endif