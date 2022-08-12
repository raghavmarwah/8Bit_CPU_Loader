#include <Arduino.h>

// Control Pins
#define CPU_RESET 3
#define MEM_WRITE 4
#define RAM_D0 5
#define RAM_D7 12
#define MAR_STROBE A1
#define MAR_A0 A2
#define MAR_A3 A5

//Read Pins
#define CONFIRM_BUTTON 2
#define COUNTER_0 A0

//variables
int confirmButton = 0;

//reset all modules
void resetCPU(int delayTime){
	
	//CPU reset attempt#1
	Serial.println("\nCPU Reset Signal");
	digitalWrite(CPU_RESET, HIGH);
	delay(delayTime);
	digitalWrite(CPU_RESET, LOW);

	//Check if CPU reset worked properly and repeat if necessary
	int resetCounter = 2;
	while(digitalRead(COUNTER_0)){
		Serial.print("CPU Reset Attempt#");
		Serial.println(resetCounter);
		resetCounter++;
		digitalWrite(CPU_RESET, HIGH);
		delay(delayTime);
		digitalWrite(CPU_RESET, LOW);
	}
	Serial.println("CPU Reset Successful");
}

//reset pins RAM_D0 through RAM_D7 to a low impedance OUTPUT state
void resetDataPins(){
	for (int pin = RAM_D0; pin <= RAM_D7; pin++) {
    	pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
  	}
}

//reset pins MAR_A0 through MAR_A3 to a high impedance INPUT state 
void resetAddressPins(){
	for (int pin = MAR_A0; pin <= MAR_A3; pin++){
		pinMode(pin, INPUT);
	}
	digitalWrite(MAR_STROBE, LOW);	
}

void setMemoryAddress(int address){
	String outputText = "";
	for (int pin = MAR_A0; pin <= MAR_A3; pin++){
		pinMode(pin, OUTPUT);
		if(address & 1){
			outputText = "1" + outputText;
			digitalWrite(pin, HIGH);
		}
		else{
			outputText = "0" + outputText;
			digitalWrite(pin, LOW);
		}
		address = address >> 1;
	}
	Serial.print(outputText+": ");
}

//write 8 bits to memory
void writeToRAM(byte data){
	String outputText = "";
	for (int pin = RAM_D0; pin <= RAM_D7; pin++){
		if(data & 1){
			outputText = "1" + outputText;
			digitalWrite(pin, HIGH);
		}
		else{
			outputText = "0" + outputText;
			digitalWrite(pin, LOW);
		}
		data = data >> 1;
	}
	Serial.println(outputText);
	pinMode(MEM_WRITE, OUTPUT);
	digitalWrite(MEM_WRITE, LOW);
	delay(600);
	pinMode(MEM_WRITE, INPUT);
}

void setup() {
	//initalize serial connection
	Serial.begin(9600);

	//define output pins
	pinMode(CPU_RESET, OUTPUT);
	pinMode(MAR_STROBE, OUTPUT);
	
	//define input pins
	pinMode(CONFIRM_BUTTON, INPUT);
	pinMode(MEM_WRITE, INPUT);
	pinMode(COUNTER_0, INPUT);
	
	resetAddressPins();
 	resetDataPins();

	Serial.println("\n\nPlease switch Clock to manual and Memory to programming mode.");
	Serial.println("Press button to continue.");

	//loop until the user presses the button
	confirmButton = digitalRead(CONFIRM_BUTTON);
	while(confirmButton != 1){
		confirmButton = digitalRead(CONFIRM_BUTTON);
	}

	resetCPU(600);

	//disable memory address dip switch input
	Serial.println("\nProgramming the memory");
	digitalWrite(MAR_STROBE, HIGH);
	byte code[] = {224, 47, 116, 96, 63, 224, 128, 100, 0, 0, 0, 0, 0, 0, 0, 1};
	for (int address = 0; address < 16; address++){
		setMemoryAddress(address);
		writeToRAM(code[address]);
		//delay(1000);
	}

	resetDataPins();
	resetAddressPins();
	resetCPU(600);

	Serial.println("\nPlease switch to run mode and enable the clock...");
}

void loop() {
  // put your main code here, to run repeatedly:
}