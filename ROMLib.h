#ifndef __romlib
#define __romlib


  #include <stdio.h>
  #include <stdint.h>

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  
  #include <stdarg.h> 
  
  #include <stdlib.h>

  #define IsHigh(BIT,BYTE)    ((BYTE & (1<<BIT)) != 0)

  extern int fd;
  extern uint32_t A;

  void setup();
  void cleanup();

/* bitsplice code by rue_mohr */

  void spliceValueFromField(uint16_t *value, uint32_t field, uint16_t count, ... ) ;
  void spliceFieldFromValue(uint32_t *field, uint16_t value, uint16_t count, ...) ;
 

#endif
