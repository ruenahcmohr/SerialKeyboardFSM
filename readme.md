# Self clocked, serial output, keyboard scaning, ROM based FSM

This uses a ROM as a core for a few things (at once)

 - Provide an inverter for an oscillator
 - Provide a non-inverter oscillator output for an FSM feedback latch
 - Run a 4 bit FSM to detect keystrokes, and transmitt them as serial data
 
 This circuit can be made quite compact if a flash chip (29F800) and 16 bit latch (74xx16374) are used.
 9600 Baud is difficult to achieve because or how slow it is. (just sayin)

  main.c will build a program to construct the rom image. the compiled programs output is for a 64k ROM chip, named output.bin 

  I think this can be optimized. If an output can be freed up, you can create a second serial stream that would output a different value (say, the binary nibble for the key instead of the ASCII value)

---
