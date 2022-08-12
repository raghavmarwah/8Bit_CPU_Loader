#include <Arduino.h>

//control pins
#define CPU_RESET A2
#define MEM_WRITE A3
#define SR1_INPUT 2
#define SR2_INPUT 3
#define SR1_CLOCK 4
#define SR2_CLOCK 5
#define SR1_LATCH 6
#define SR2_LATCH 7

//read pins
#define CONFIRM_BUTTON A0
#define COUNTER_0 A1

//constants
#define PULSE_DURATION 10

//variables
int confirmButton = 0;

//reset all modules
void resetCPU(){
	
	//CPU reset attempt#1
	Serial.println("\nCPU Reset Signal");
	digitalWrite(CPU_RESET, HIGH);
	delay(PULSE_DURATION);
	digitalWrite(CPU_RESET, LOW);

	//Check if CPU reset worked properly and repeat if necessary
	int resetCounter = 2;
	while(digitalRead(COUNTER_0)){
		Serial.println("CPU Reset Attempt#"+resetCounter);
		resetCounter++;
		digitalWrite(CPU_RESET, HIGH);
		delay(PULSE_DURATION);
		digitalWrite(CPU_RESET, LOW);
	}
	Serial.println("CPU Reset Successful");
}

//shift 4 bits to SR2 and latch to memory address register
void setMemoryAddress(byte address){
	
	digitalWrite(SR2_LATCH, LOW);
	shiftOut(SR2_INPUT, SR2_CLOCK, MSBFIRST, address);
	digitalWrite(SR2_LATCH, HIGH);

	String outputText = "";
	for (int i = 0; i < 4; i++){
		if(address & 1)
			outputText = "1" + outputText;
		else
			outputText = "0" + outputText;
		address = address >> 1;
	}
	Serial.print(outputText+": ");
}

//shift 8 bits to SR1 and latch to memory data
void writeToRAM(byte data){
	
	digitalWrite(SR1_LATCH, LOW);
	shiftOut(SR1_INPUT, SR1_CLOCK, MSBFIRST, data);
	digitalWrite(SR1_LATCH, HIGH);
	
	String outputText = "";
	for (int i = 0; i < 8; i++){
		if(data & 1)
			outputText = "1" + outputText;	
		else
			outputText = "0" + outputText;
		data = data >> 1;
	}
	Serial.println(outputText);
	pinMode(MEM_WRITE, OUTPUT);
	digitalWrite(MEM_WRITE, LOW);
	delay(PULSE_DURATION);
	pinMode(MEM_WRITE, INPUT);
}

void setup() {
	//initalize serial connection
	Serial.begin(9600);

	//initialize pins
	pinMode(CPU_RESET, OUTPUT);
	pinMode(SR1_INPUT, OUTPUT);
	pinMode(SR2_INPUT, OUTPUT);
	pinMode(SR1_CLOCK, OUTPUT);
	pinMode(SR2_CLOCK, OUTPUT);
	pinMode(SR1_LATCH, OUTPUT);
	pinMode(SR2_LATCH, OUTPUT);
	digitalWrite(CPU_RESET, LOW);
	digitalWrite(SR1_INPUT, LOW);
	digitalWrite(SR2_INPUT, LOW);
	digitalWrite(SR1_CLOCK, LOW);
	digitalWrite(SR2_CLOCK, LOW);
	digitalWrite(SR1_LATCH, LOW);
	digitalWrite(SR2_LATCH, LOW);

	pinMode(CONFIRM_BUTTON, INPUT);
	pinMode(MEM_WRITE, INPUT);
	pinMode(COUNTER_0, INPUT);

	//notify user, wait for user prompt
	Serial.println("\n\nPlease switch Clock to manual and Memory to programming mode.");
	Serial.println("Ensure ALL DIP switches are in the ON (HIGH) position. Press button to continue.");

	//loop until the user presses the button
	confirmButton = digitalRead(CONFIRM_BUTTON);
	while(confirmButton != 1){
		confirmButton = digitalRead(CONFIRM_BUTTON);
	}

	resetCPU();

	//disable memory address dip switch input
	Serial.println("\nProgramming the memory");
	byte code[] = {224, 47, 116, 96, 63, 224, 128, 100, 0, 0, 0, 0, 0, 0, 0, 1};
	for (int address = 0; address < 16; address++){
		setMemoryAddress(address);
		writeToRAM(code[address]);
		delay(10);
	}

	digitalWrite(SR1_LATCH, LOW);
	digitalWrite(SR2_LATCH, LOW);
	resetCPU();
	Serial.println("\nPlease switch to run mode and enable the clock...");
	
}

void loop() {
  // put your main code here, to run repeatedly:
}