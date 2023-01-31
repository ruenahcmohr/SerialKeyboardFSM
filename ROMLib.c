#include "ROMLib.h"

int fd;
uint32_t A;

void setup(void ) {

  if ((fd = open ( "output.bin", O_CREAT|O_WRONLY|O_TRUNC, 00666)) == -1)  {
    printf("Cant open output file\n");
    exit(0);
  } 

}


void cleanup(void) {

  close(fd);

}





// the bit list is LSB first.
void spliceValueFromField(uint16_t *value, uint32_t field,  uint16_t count, ... ) {
  uint16_t mask = 0x01;
  uint8_t  bitCount;
  va_list      ap;
  
  va_start(ap, count);
  
  for( *value = 0, bitCount = 0; bitCount < count; bitCount++, mask<<=1) 
    if ( IsHigh(va_arg(ap, int), field ))   (*value) |= mask;    
  
  return;
}



void spliceFieldFromValue(uint32_t *field, uint16_t value, uint16_t count, ...) {
  uint32_t mask = 0x01;
  uint8_t  bitCount;
  va_list  ap;
  uint8_t  bit;
  
  va_start(ap, count);
  
 // for( *field = 0, bitCount = 0; bitCount < count; bitCount++, mask<<=1) {
  for( bitCount = 0; bitCount < count; bitCount++, mask<<=1) {
    bit = va_arg(ap, int);
    if ((mask & value) != 0)   (*field) |= 1<<bit    ;    
  }
  
  return;
}




