#include "VMIDI.h"

VMIDI::VMIDI(HardwareSerial &print){ 
	MIDISerial = &print; //operate on the adress of print
	MIDISerial->begin(BAUD);
}

/*
VMIDI::VMIDI(SoftwareSerial &print){ 
	MIDISerial = &print; //operate on the adress of print
	MIDISerial->begin(BAUD);
}*/



void VMIDI::send_msg(int command, byte note, byte velocity) {
  MIDISerial->write(command);//send note on or note off command
  MIDISerial->write(note);//send pitch data
  MIDISerial->write(velocity);//send velocity data
}

void VMIDI::sendNoteOn(byte note,byte velocity,int channel) {
	send_msg(MIDI_MSG_NOTE_ON|(channel-1),note,velocity);
}

void VMIDI::sendNoteOff(byte note,byte velocity,int channel) {
	send_msg(MIDI_MSG_NOTE_OFF|(channel-1),note,velocity);
}
void VMIDI::sendCtrlChange(byte cc,byte value,int channel) {
	send_msg(MIDI_MSG_CTRL_CHANGE|(channel-1),cc,value);
}


int VMIDI::read() {
	return MIDISerial->read();
}

int VMIDI::available() {
	return MIDISerial->available();
}

void VMIDI::setTimeout(long timeout) {
	MIDISerial->setTimeout(timeout);
}
byte VMIDI::readBytes(byte* buffer,int length){
	return readBytes(buffer,length);
}