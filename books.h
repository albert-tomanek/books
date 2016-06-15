#ifndef BOOKS
  #define BOOKS
  
  // Don't change these else you'll have trouble reading .col files.
  #define CMDLEN 64
  #define STRLEN 256
  
  #include <stdint.h>
  #include "modules/text.h"
  
  // Useful marco for the CLI
  #define command_is(S) !strcmp(text_lowercase(cmd), S)
  
  struct Book {
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
  
  char *getgenre(int genre);

#endif