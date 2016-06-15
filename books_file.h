#ifndef BOOKS_FILE
  #define BOOKS_FILE
  
  #include <stdio.h>
  #include <stdint.h>
  #include <assert.h>
  #include "modules/ints.h"
  #include "books.h"
  
  #define BOOKS_FILE_VERSION 1
  
  void         books_file_write(struct Book *first_book, char *file);
  struct Book* books_file_read(char *file);  // This function returns a pointer to books.
  
#endif