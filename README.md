# 8Bit_CPU_Loader

This is an Arduino Nano based program loader for [Ben Eater's 8bit TTL CPU](https://eater.net/8bit) project. I built the project on breadboards in the past. The CPU is fully programmable with 16 bytes of memory and 11 Op Codes that can be used to write programs, one instruction per byte. This programming has to be done manually using DIP switches every time the device is powered on. It is a bit tedious as you have to enter instructions in binary for each memory location by hand.
<br><br>
I have been learning PCB design and would like to convert this project to a PCB build in the near future. One of the things I wanted to add was a program loader that can automatically load programs into the memory on boot. Before implementing this on a PCB, I wanted to have a working prototype that connects with my Breadboard CPU. More details https://raghavmarwah.com/arduino-8bit-loader/
<br><br>
The build uses an Arduino Nano and two 74HC595 Shift Registers to shift out values for the memory address and data to their respective DIP switches. When using this program loader, please ensure all DIP switches are set to the HIGH (UP) position. These switches are inverted in Ben's design, hence moving them to the UP position disconnects them (from GND).
<br><br>
I use a 3rd party platform called PlatformIO for writing Arduino programs in VS Code. But you can simply copy the code from src/main.cpp and run that in the Arduino IDE.
<br><br>

## Pin Mapping
![loader_pin_map](https://user-images.githubusercontent.com/10029166/184461127-7dc9f32e-117d-4731-95f6-aab1b64a49fe.png)
<br><br>
**Arduino**<br>
D2 to Shift Register 1 Pin 14<br>
D3 to Shift Register 2 Pin 14<br>
D4 to Shift Register 1 Pin 12<br>
D5 to Shift Register 2 Pin 12<br>
D6 to Shift Register 1 Pin 11<br>
D7 to Shift Register 2 Pin 11<br>
A0 to Push Button input with 10K pulldown resistor<br>
A1 to Output 0 on 74LS138 Decoder<br>
A2 to CPU_RESET<br>
A3 to MEM_RESET<br>
A4 to OLED I2C SDA<br>
A5 to OLED I2C SCL<br>
A6 to the clock enable/disable button<br>
A7 to the programming/run mode button<br><br>
**Shift Register 1**<br>
Pin 15 to Data DIP0 switch<br>
Pin 1 to Data DIP1 switch<br>
Pin 2 to Data DIP2 switch<br>
Pin 3 to Data DIP3 switch<br>
Pin 4 to Data DIP4 switch<br>
Pin 5 to Data DIP5 switch<br>
Pin 6 to Data DIP6 switch<br>
Pin 7 to Data DIP7 switch<br><br>
**Shift Register 2**<br>
Pin 15 to Address DIP0 switch<br>
Pin 1 to Address DIP1 switch<br>
Pin 2 to Address DIP2 switch<br>
Pin 3 to Address DIP3 switch<br><br>
**Note:** Vin pin on the Arduino can optionally be connected to a +7-12 VDC input.<br>
***Please ensure Vin and USB are not both connected at the same time.***
<br>

## Programming
### Supported Op Codes
| Instruction Bits | OP CODE | Description |
| ------ | ------ | ------ |
| 0000 | NOP | No Operation |
| 0001 | LDA | Load Register A |
| 0010 | ADD | Add Register A and B |
| 0011 | SUB | Subtract Register B from A |
| 0100 | STA | Store Register A value to memory |
| 0101 | LDI | Load Instruction |
| 0110 | JMP | Jump |
| 0111 | JC | Jump Carry |
| 1000 | JZ | Jump Zero |
| 1101 | OUT | Display on Output register |
| 1111 | HLT | Halt system clock |

The data to be programmed to the memory is stored in a byte array named code (in decimal format). 16 values are programmed in 16 memory locations, each value is converted to binary in their respective function call. The below code adds a number (saved in memory location 15 i.e. 1111) repeatedly to itself until the upper limit (255) is reached, it then repeatedly subtracts that value from itself until it reaches 0, and it keeps looping.
```sh
byte code[] = {224, 47, 116, 96, 63, 224, 128, 100, 0, 0, 0, 0, 0, 0, 0, 1};
```
| Memory Location | Array Value (Decimal) | Binary Equivalent | Op Codes |
| ------ | ------ | ------ | ------ |
| 0000 | 224 | 1110 0000 | OUT |
| 0001 | 47 | 0010 1111 | ADD 15 |
| 0010 | 116 | 0111 0100 | JC 4 |
| 0011 | 96 | 0110 0000 | JMP 0 |
| 0100 | 63 | 0011 1111 | SUB 15 |
| 0101 | 224 | 1110 0000 | OUT |
| 0110 | 128 | 1000 0000 | JZ 0
| 0111 |  |  |  |
| 1000 |  |  |  |
| 1001 |  |  |  |
| 1010 |  |  |  |
| 1011 |  |  |  |
| 1100 |  |  |  |
| 1101 |  |  |  |
| 1110 |  |  |  |
| 1111 | 1 | XXXX 0001 |  |
