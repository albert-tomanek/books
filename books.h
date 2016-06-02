#ifndef BOOKS
  #define BOOKS
  
  #include <stdint.h>
  #include "modules/text.h"
  
  struct Book
  {
	  char title[CMDLEN];
  	  char author[CMDLEN];
  	  char file[STRLEN];
  	  
  	  uint8_t genre;		
  	
  	  int book_id;
  	
  	  struct Book *next;
  };
  
  
  // Functions
  struct Book *getlastbook(struct Book *first);
  int countBooks(struct Book *first);

#endif