#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

Adafruit_SSD1306 display(-1);

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
#define CLOCK_STOP A6
#define PROG_MODE A7

//constants
#define PULSE_DURATION 10

//variables
int confirmButton = 0;
bool programMode = false;

//reset all modules
void resetCPU(){
	
	//CPU reset attempt#1
	//Serial.println("\nCPU Reset Signal");
	display.clearDisplay();
	display.setCursor(0,0);
	display.println("CPU Reset Signal");
	//display.display();
	digitalWrite(CPU_RESET, HIGH);
	delay(PULSE_DURATION);
	digitalWrite(CPU_RESET, LOW);

	//Check if CPU reset worked properly and repeat if necessary.
	int resetCounter = 2;
	while(digitalRead(COUNTER_0)){
		//Serial.println("CPU Reset Attempt#"+resetCounter);
		display.println("CPU Reset Attempt#"+resetCounter);
		//display.display();
		resetCounter++;
		digitalWrite(CPU_RESET, HIGH);
		delay(PULSE_DURATION);
		digitalWrite(CPU_RESET, LOW);
	}
	//Serial.println("CPU Reset Successful");
	display.println("CPU Reset Successful");
	display.display();
	delay(900);
}

//shift 4 bits to SR2 and latch to memory address register.
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
	//Serial.print(outputText+": ");

}

//shift 8 bits to SR1 and latch to memory data.
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
	//Serial.println(outputText);
	pinMode(MEM_WRITE, OUTPUT);
	digitalWrite(MEM_WRITE, LOW);
	delay(PULSE_DURATION);
	pinMode(MEM_WRITE, INPUT);
}

void setup() {

	//initialize with the I2C addr 0x3C
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  

	//clear the buffer.
	display.clearDisplay();

	display.setTextSize(1);
	display.setTextColor(WHITE);

	//initalize serial connection
	Serial.begin(115200);
	Serial.println();

	//initialize pins
	digitalWrite(CPU_RESET, LOW);
	digitalWrite(SR1_INPUT, LOW);
	digitalWrite(SR2_INPUT, LOW);
	digitalWrite(SR1_CLOCK, LOW);
	digitalWrite(SR2_CLOCK, LOW);
	digitalWrite(SR1_LATCH, LOW);
	digitalWrite(SR2_LATCH, LOW);
	pinMode(CPU_RESET, OUTPUT);
	pinMode(SR1_INPUT, OUTPUT);
	pinMode(SR2_INPUT, OUTPUT);
	pinMode(SR1_CLOCK, OUTPUT);
	pinMode(SR2_CLOCK, OUTPUT);
	pinMode(SR1_LATCH, OUTPUT);
	pinMode(SR2_LATCH, OUTPUT);
	
	pinMode(CONFIRM_BUTTON, INPUT);
	pinMode(MEM_WRITE, INPUT);
	pinMode(COUNTER_0, INPUT);
	pinMode(CLOCK_STOP, INPUT);
	pinMode(PROG_MODE, INPUT);

	//notify user to disable the clock and switch to programming mode.
	//program execution will not continue unless these conditions are satisfied.
	programMode = (analogRead(CLOCK_STOP) < 410 && analogRead(PROG_MODE) < 410) ? true : false;
	if(!programMode){
		//Serial.println("Please switch Clock to manual and Memory to programming mode.");
		display.setCursor(0,0);
		display.println("Please switch Clock\nto manual and Memory to programming mode.");
		display.display();
	}
	while(!programMode){
		programMode = (analogRead(CLOCK_STOP) < 410 && analogRead(PROG_MODE) < 410) ? true : false;
	}

	//wait for user to ensure all DIP switches are in the ON (HIGH) position. These switches
	//are inverted in Ben's design, hence moving them to the UP position disconnects them.
	
	//Serial.println("Ensure ALL DIP switches are in the ON (HIGH) position.\nPress button to continue.");
	display.clearDisplay();
	display.setCursor(0,0);
	display.println("Ensure ALL DIP\nswitches are in the\nON (HIGH) position.\n\nPress button to\ncontinue.");
	display.display();

	//loop until the user presses the button.
	confirmButton = digitalRead(CONFIRM_BUTTON);
	while(confirmButton != 1){
		confirmButton = digitalRead(CONFIRM_BUTTON);
	}

	resetCPU();
	display.clearDisplay();
	display.setCursor(0,0);

	//disable memory address dip switch input.
	//Serial.println("\nProgramming the memory");
	display.println("Programming the\nmemory");
	byte code[] = {224, 47, 116, 96, 63, 224, 128, 100, 0, 0, 0, 0, 0, 0, 0, 1};
	for (int address = 0; address < 16; address++){
		setMemoryAddress(address);
		writeToRAM(code[address]);
		display.print(".");
		display.display();
		delay(10);
	}

	//disable shift register latches, and reset all modules again.
	digitalWrite(SR1_LATCH, LOW);
	digitalWrite(SR2_LATCH, LOW);
	resetCPU();
	//Serial.println("\nPlease switch to run mode and enable the clock...");

	programMode = false;
}

void loop() {
  	programMode = (analogRead(CLOCK_STOP) > 410 && analogRead(PROG_MODE) > 410) ? true : false;
	if(programMode){
		display.clearDisplay();
		display.setCursor(40,30);
		display.println("RUN MODE");
		display.display();
	}
	else{
		display.clearDisplay();
		display.setCursor(0,0);
		display.println("Please switch to run\nmode and enable the\nclock...");
		display.display();
	}
	delay(100);
}