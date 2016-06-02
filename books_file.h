#ifndef BOOKS_FILE
  #define BOOKS_FILE
  
  #include <stdio.h>
  #include <stdint.h>
  #include <assert.h>
  #include "modules/ints.h"
  #include "books.h"
  
  void books_file_write(struct Book *first_book, char *file);
#endif