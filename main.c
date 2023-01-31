/***********************************************************************************

Inspried by PHPPLD.
2021 Rue Mohr.
This generic ROM-generator framework was customized for:

scanless keyboard encoder LUT, serial output via self-clocked FSM

This is set up for a 4x4 grid

INPUTS
 A0 : oscillator feedback
 
 A1 A2 A3 A4 : state counter feedback
 
 A5 A6 A7 A8 A9 A10 A11 A12: keyboard rows and columns

OUTPUTS

D0 : inverted oscillator output
D1 : non-inverted oscillator output for feedback latch

D2 D3 D4 D5: state counter output

D6 : serial data out

D7 : clock line for keyboard holding latch
 
FEEDBACK
 D2 D3 D4 D5 -> A1 A2 A3 A4 



Address bits      8 bit rom size

       -- no parallel roms available --
     8                  2 k
     9                  4 k
     10                 8 k
     
       -- eeproms available from here --
     11                 16 k  (28C16)
     12                 32 k  (28C32)
     
       -- eprom practical sizes from here --
     13                 64 k  (2764)
     14                 128 k (27128)
     15                 256 k 
     16                 512 k
     17                 1 M  (27010)
     18                 2 M
     19                 4 M
     20                 8 M

       -- flash from here up --



**************************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include "ROMLib.h"


// the number of address lines you need !!!???!!!
#define InputBits 13

// the output data size, 8 or 16
#define OutputBits 8

// default output value
#define DFOutput  0x00



// Tuck this one away!. Bit reverser!  Please dont use this in real fft code,
//   YOU KNOW how many bits your working with, and you can use a 
//   specific case generator for it.
uint8_t uniReverse(uint8_t i, uint8_t bits) {

  uint8_t r, m, b;
  r = 0;             // result
  m = 1 << (bits-1); // mask will travel right
  b = 1;             // bit will travel left
  
  while(m) {
    if (i&b) r |=m;
    b <<= 1;
    m >>= 1;  
  }
  
  return r;  

}


// count set bits, unrolled edition.
// if using assember shift into the carry and use addc, 0
uint8_t bitCount(uint16_t n) {  
   uint8_t rv;
   rv = 0;
   if (n & 0x0001) rv++;
   if (n & 0x0002) rv++;
   if (n & 0x0004) rv++;
   if (n & 0x0008) rv++;
   if (n & 0x0010) rv++;
   if (n & 0x0020) rv++;
   if (n & 0x0040) rv++;
   if (n & 0x0080) rv++;   
   if (n & 0x0100) rv++;
   if (n & 0x0200) rv++;
   if (n & 0x0400) rv++;
   if (n & 0x0800) rv++;
   if (n & 0x1000) rv++;
   if (n & 0x2000) rv++;
   if (n & 0x4000) rv++;
   if (n & 0x8000) rv++; 
   
   return rv;
}

// convert a character and bit position to a serial level, 10 bits per character. 
uint8_t SerialChar(char c, uint16_t bit) {
  
  switch (bit) {
    case 0: return 0; // start bit
    case 1: return (c & 0x01)!=0;
    case 2: return (c & 0x02)!=0;
    case 3: return (c & 0x04)!=0;
    case 4: return (c & 0x08)!=0;
    case 5: return (c & 0x10)!=0;
    case 6: return (c & 0x20)!=0;
    case 7: return (c & 0x40)!=0;
    case 8: return (c & 0x80)!=0;
    case 9: return 1; // stop bit          
  }

}

uint8_t keyLUT(uint8_t d) { // this decodes my 16 key matrix keyboard into ASCII
  switch(d) {

    case 0xEE:   return '0';   break;    
    case 0xED:   return '1';   break;
    case 0xEB:   return '2';   break;
    case 0xE7:   return '3';   break;   
     
    case 0xDE:   return '4';   break;   
    case 0xDD:   return '5';   break;   
    case 0xDB:   return '6';   break;   
    case 0xD7:   return '7';   break;   
    
    case 0xBE:   return '8';   break;    
    case 0xBD:   return '9';   break;
    case 0xBB:   return 'A';   break;
    case 0xB7:   return 'B';   break;   
     
    case 0x7E:   return 'C';   break;   
    case 0x7D:   return 'D';   break;   
    case 0x7B:   return 'E';   break;   
    case 0x77:   return 'F';   break;     
        
    default:     return 0;     break;  // invalid
    
  }
}


int main(void) {

  uint16_t bitcI;
  uint16_t counterI;
  uint16_t inverterI;
  uint16_t keyscanI;
  
  uint8_t  TxDO; 
  uint8_t  counterO;
  uint8_t  inverterO;
  uint8_t  keyLatchO;


  uint32_t out;  // leave it alone!

  setup();       // open output file.
  
  
  // loop thru each address
  for( A=0; A<(1<<InputBits); A++) { // A is a bitfield (a mash-up of the vars we want)
       
     // reset vars  
     bitcI     = 0;
     keyLatchO = 0;
     TxDO      = 1;
     
     // build input values
    spliceValueFromField( &inverterI,          A,  1,   0);  
    spliceValueFromField( &counterI,           A,  4,   1, 2, 3, 4);  
    spliceValueFromField( &keyscanI,           A,  8,   5, 6, 7, 8, 9, 10, 11, 12);  
    
    counterO = counterI;
    
    inverterO = 1- inverterI;
    if (inverterO == 0) counterO++;


    switch (counterI) {
    
      case 0:
       keyLatchO = 1;
      break;

      case 1:
       if (keyLUT(keyscanI) == 0x00) counterO = 0; // loop till valid key
      break;
      
      case 2:
        TxDO = SerialChar(keyLUT(keyscanI), 0); // start
      break;
      
      case 3:
        TxDO = SerialChar(keyLUT(keyscanI), 1); // D0
      break;
      
      case 4:
        TxDO = SerialChar(keyLUT(keyscanI), 2);// D1
      break;

      case 5:
        TxDO = SerialChar(keyLUT(keyscanI), 3);// D2
      break;
      
      case 6:
        TxDO = SerialChar(keyLUT(keyscanI), 4);// D3
      break;
      
      case 7:
        TxDO = SerialChar(keyLUT(keyscanI), 5);// D4
      break;     
       
      case 8:
        TxDO = SerialChar(keyLUT(keyscanI), 6);// D5
      break;

      case 9:
        TxDO = SerialChar(keyLUT(keyscanI), 7);// D6
      break;
      
      case 10:
        TxDO = SerialChar(keyLUT(keyscanI), 8);// D7
      break;
      
      case 11:
        TxDO = SerialChar(keyLUT(keyscanI), 9); // stop
      break;
      
      case 12:
        keyLatchO = 1;
      break;

      case 13:
        if (keyscanI != 0xFF) {
          counterO = 12; // loop till no key
        } else {
          counterO = 0; // goto 0
        }
      break;
      
      case 14:
      break;
      
      case 15:
      break;        
    
    }

   
     // reconstitute the output
     // assign default values for outputs     
     out = DFOutput;
     
     
     spliceFieldFromValue( &out, inverterO,    1,  0); 
     spliceFieldFromValue( &out, inverterI,    1,  1); 
     spliceFieldFromValue( &out, counterO,     4,  2 , 3, 4, 5); // insert counterO into out, as 4 bits, placed in positions 2, 3, 4, and 5 (of out)
     spliceFieldFromValue( &out, TxDO,         1,  6); 
     spliceFieldFromValue( &out, keyLatchO,    1,  7); 
     
         
     // submit entry to file
     write(fd, &out, OutputBits>>3);  // >>3 converts to bytes, leave it!
  }
  
  cleanup(); // close file
  
  return 0;
}









