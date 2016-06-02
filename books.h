#ifndef BOOKS
  #define BOOKS
  
  #include "modules/text.h"
  
  struct Book
  {
	  char title[CMDLEN];
  	  char author[CMDLEN];
  	  char file[STRLEN];
  	  
  	  int genre;		
  	
  	  int book_id;
  	
  	  struct Book *next;
  };

#endif