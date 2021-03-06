#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for malloc
#include <assert.h>

#include "books.h"
#include "books_file.h"
#include "modules/text.h"
#include "modules/dbg.h"

struct Book *getlastbook(struct Book *first);
char *getgenre(int genre);
char *i_to_str(int number);

struct Book *getBook(struct Book *first_book, int search_id);

void deleteBook(struct Book **first_book, struct Book *delete_book);	// Pointer-to-pointers because they will be changed in the function.
void freeBooks(struct Book **first);

void printhelp();

int main(int argc, char *argv[])
{	
	struct Book *first_book = NULL;
	
	int bookCount = 0;
	
	printf(" -- Books -- \n");
	
	/* Handle commandline arguments */
	if (argc > 1)
	{
		if (! strcmp(argv[1], "-h") || ! strcmp(argv[1], "--help"))
		{
			printf(" ./books [file.col]\n\n");
			return 0;
		}
		else if (! strcmp(argv[1], "-v") || ! strcmp(argv[1], "--version"))
		{
			printf(" Books %s\n", BOOKS_VERSION);
			printf(" Build %d\n\n", BOOKS_BUILD);
			return 0;
		}
		else
		{
			char address[STRLEN];
			strncpy(address, argv[1], STRLEN);
			
			first_book = books_file_read(address);
			if (! first_book) {
				printf("failed.\n");
			}
		}
	}
	
	/* Main loop */
	
	char cmd[CMDLEN];
	int loop = 1;
	while (loop)
	{

		printf(" > ");
		fgets(cmd, CMDLEN, stdin);
		denewline(cmd);
		
		if ( command_is("l") || command_is("list") )
		{
			if (first_book) 	// If we actually have any books...
			{
				struct Book *current_book = first_book;
				
				int loop = 1;
				while (loop)
				{
					printf(" %s\n", current_book->title);
					
					if (current_book->next == NULL)
					{
						loop = 0;
					}
					else
					{
						current_book = current_book->next;
					}
				}
			}
			else
			{
				printf("-> no books.\n");
			}
		}
		
		
		if ( command_is("d") || command_is("details") )
		{
			if (first_book) 	// If we actually have any books...
			{
				struct Book *current_book = first_book;
				
				printf(" ID   | Title            | Author     | Genre   | Filename     \n");
				printf("------+------------------+------------+---------+--------------\n");
				
				int loop = 1;
				while (loop)
				{
					printf(" %s | %s | %s | %s | %s \n", 
						text_fill(i_to_str( current_book->book_id ), 4),
						text_fill(current_book->title,				16),
						text_fill(current_book->author,				10),
						text_fill(getgenre(current_book->genre),	 7),
						text_fill(current_book->file,				13)  );
					
					if (current_book->next == NULL)
					{
						loop = 0;
					}
					else
					{
						current_book = current_book->next;
					}
				}
			}
			else
			{
				printf("-> no books.\n");
			}
		}
		
		
		if ( command_is("c") || command_is("count") )
		{
			if (first_book == NULL)
			{
				printf("-> There are no books in the collection.\n");
			}
			else
			{
				printf("-> The number of books in the collection is %d.\n", countBooks(first_book) );
			}
		}
		
		
		if ( command_is("a") || command_is("add") )
		{
			char title[CMDLEN];
			char author[CMDLEN];
			char file[STRLEN];
			
			char genre_str[CMDLEN];
			int  genre;
			
			struct Book *current_book = calloc(1, sizeof(struct Book));
			check(current_book != NULL , "calloc returned null pointer.");
			
			printf(" Book title: ");
			fgets(title, CMDLEN, stdin);
			denewline(title);
			
			printf(" Book author: ");
			fgets(author, CMDLEN, stdin);
			denewline(author);
			
			printf(" Filename: ");
			fgets(file, CMDLEN, stdin);
			denewline(file);
			
			strcpy(current_book->title , title );
			strcpy(current_book->author, author);
			strcpy(current_book->file  , file  );
			
			printf("\n\
 Genres: 0 = Other,   \n\
         1 = Mistery, \n\
         2 = Comedy,  \n\
         3 = Sciende, \n\
         4 = Sci-fi,  \n\
         5 = Fantasy  \n");
			do
			{
				printf(" Book genre (0-5): ");
				fgets(genre_str, CMDLEN, stdin);
				denewline(genre_str);
				genre = atoi(genre_str);
			} while (genre > 5);
			
			current_book->genre   = genre;
			current_book->book_id = bookCount;
			
			printf("\n Book ID = %d\n", current_book->book_id);
			
			current_book->next = NULL;
			
			if (first_book == NULL)
			{
				first_book = current_book;
			}
			else
			{
				getlastbook(first_book)->next = current_book;
			}
			
			bookCount++;
			
		}
		
		if ( command_is("m") || command_is("modify") )
		{
			/* Get the book ID */
			
			int  id;
			char id_str[CMDLEN];
			
			printf(" Book ID: ");
			fgets(id_str, CMDLEN, stdin);
			denewline(id_str);
			id = atoi(id_str);
			
			struct Book *mod_book = getBook(first_book, id);	// Get the pointer to the book we want to modify
			
			if (! mod_book)
			{
				printf(" No book with ID #%d\n", id);
				continue;
			}
			
			/* Modify the contents */
			
			printf(" Press RETURN to keep existing.\n");
			
			char title[CMDLEN];
			char author[CMDLEN];
			char file[STRLEN];
			
			char genre_str[CMDLEN];
			int  genre;
			
			printf(" Book title ['%s']: ", mod_book->title);
			fgets(title, CMDLEN, stdin);
			denewline(title);
			
			printf(" Book author ['%s']: ", mod_book->author);
			fgets(author, CMDLEN, stdin);
			denewline(author);
			
			printf(" Filename ['%s']: ", mod_book->file);
			fgets(file, CMDLEN, stdin);
			denewline(file);
			
			/* Overwrite the current data if a different string is given. */
			
			if ( strlen(title ) > 0 )	strcpy(mod_book->title , title );
			if ( strlen(author) > 0 )	strcpy(mod_book->author, author);
			if ( strlen(file)   > 0 )	strcpy(mod_book->file  , file  );
			
			printf("\n\
 Genres: 0 = Other,   \n\
         1 = Mistery, \n\
         2 = Comedy,  \n\
         3 = Sciende, \n\
         4 = Sci-fi,  \n\
         5 = Fantasy  \n");
			do
			{
				printf(" Book genre [%s](0-5): ", getgenre(mod_book->genre));
				fgets(genre_str, CMDLEN, stdin);
				denewline(genre_str);
				genre = atoi(genre_str);
			} while (genre > 5);
			
			if ( strlen(genre_str) > 0 )	mod_book->genre = genre;
			
		}
		
		if ( command_is("v") || command_is("view") )		// View details
		{
			int  id;
			char id_str[CMDLEN];
			
			printf(" Book ID: ");
			fgets(id_str, CMDLEN, stdin);
			denewline(id_str);
			id = atoi(id_str);
			
			struct Book *view_book = getBook(first_book, id);	// Get the pointer to the book we want to delete,
			
			if (! view_book)
			{
				printf(" No book with ID #%d\n", id);
				continue;
			}
			
			printf(" Book title  = %s\n", view_book->title);
			printf(" Book author = %s\n", view_book->author);
			printf(" Book file   = %s\n", view_book->file);
			printf(" Book genre  = %s\n", getgenre((int) view_book->genre));
			printf(" \n");
			printf(" Book ID     = %d\n", view_book->book_id);
		}
		
		if ( command_is("r") || command_is("remove") )		// Remove a book
		{
			int  id;
			char id_str[CMDLEN];
			
			printf(" Book ID: ");
			fgets(id_str, CMDLEN, stdin);
			denewline(id_str);
			id = atoi(id_str);
			
			struct Book *del_book = getBook(first_book, id);	// Get the pointer to the book we want to delete,
			
			if (! del_book)
			{
				printf(" No book with ID #%d\n", id);
				continue;
			}
			
			deleteBook(&first_book, del_book);
		}
		
		if ( command_is("s") || command_is("save") )
		{
			if (first_book == NULL)
			{
				printf("-> No books to save!\n");
				continue;
			}
			if (countBooks(first_book) > 65534)
			{
				printf("-> Collection of books is too large to store in a .col file. Sorry.\n");
				continue;
			}
			
		  //char  address[STRLEN];
			char* address = calloc(STRLEN, sizeof(char));
			
			printf(" Save as: ");
			fgets(address, STRLEN, stdin);
			denewline(address);
			
			if (! strcmp(text_getslice(strlen(address)-4, text_lowercase(address), 4), ".col") )
			{
				;  	//pass
			}
			else
			{
				address = text_append(".col", address);
				printf(" Saved as '%s'.\n", address);
			}
			
			books_file_write(first_book, address);
		}
		
		if ( command_is("o") || command_is("open") )
		{
			char address[STRLEN];
			
			printf(" Load: ");
			fgets(address, STRLEN, stdin);
			denewline(address);
			
			freeBooks(&first_book);
			
			first_book = books_file_read(address);
			
			if (first_book == NULL) {
				printf("failed.\n");
				continue;
			}
			
			bookCount = countBooks(first_book);
		}
		
		if ( command_is("x") || command_is("delete") )
		{
			freeBooks(&first_book);
		}
		
		if ( command_is("f") || command_is("find") )
		{
			/* Search for a book */
			
			struct Book *current_book = first_book;
			struct Book *next;
			char search_string[STRLEN];

			if (! first_book) {		// In case there are no books yet and first_book is NULL
				printf(" No books to search!\n");
				continue;
			}
			
			printf(" Search string: ");
			fgets(search_string, STRLEN, stdin);
			denewline(search_string);
				
			//search_string = text_lowercase( search_string );	// To make it case-insensitive
				
			printf(" Searching %d books for '%s'...\n\n", countBooks(first_book), search_string);
				
			printf(" ID   | Title            | Author     | Genre   | Filename     \n");
			printf("------+------------------+------------+---------+--------------\n");
				
			while(current_book)
			{
				next = current_book->next;
							
				if ( istrstr( text_lowercase(search_string), text_lowercase(current_book->title ) )  ||
					 istrstr( text_lowercase(search_string), text_lowercase(current_book->author) )  ||
					 istrstr( text_lowercase(search_string), text_lowercase(current_book->file  ) )  )
				{
					printf(" %s | %s | %s | %s | %s \n", 
					text_fill(i_to_str( current_book->book_id ), 4),
					text_fill(current_book->title,				16),
					text_fill(current_book->author,				10),
					text_fill(getgenre(current_book->genre),	 7),
					text_fill(current_book->file,				13)  );
					
				}
				
				current_book = next;
			}
		}
		
		if ( command_is("h") || command_is("help") )
		{
			printhelp();
		}
		
		if ( command_is("q") || command_is("quit") )
		{
			freeBooks(&first_book);
			loop = 0;
		}
	}
	
	return 0;
	
error:
	if (first_book) 	freeBooks(&first_book);
	return 0;
}

void printhelp()
{
	printf(" Books %s\n", BOOKS_VERSION);
	printf(" Build %d\n", BOOKS_BUILD);
	
	printf(" \n");
	
	printf(" A = Add a new book\n");
	printf(" R = Remove a book\n");
	printf(" M = Modify a book\n");
	printf(" V = View details\n");
	
	printf(" \n");
	
	printf(" L = List all books\n");
	printf(" D = List all books with details\n");
	printf(" C = Count the number of books\n");
	
	printf(" \n");
	
	printf(" S = Save the book collection to a file\n");
	printf(" O = Open a book collection from a file\n");
	
	printf(" \n");
	
	printf(" X = Delete the current collection\n");
	printf(" F = Find a book\n");
	
	printf(" \n");
	
	printf(" H = Help\n");
	printf(" Q = Quit\n");
}

char *getgenre(int genre)
{
	if (genre == 0)   return "Other";
	if (genre == 1)   return "Mistery";
	if (genre == 2)   return "Comedy";
	if (genre == 3)   return "Science";
	if (genre == 4)   return "Sci-fi";
	if (genre == 5)   return "Fantasy";
	
	else              return "Other";
}

struct Book *getlastbook(struct Book *first)
{
	check(first != NULL, "Null-pointer to 'struct Book *first'");
	
	struct Book *current = first;
	
	while (current->next)
	{
		current = current->next;
	}
	
	return current;
	
error:
	return first; 	//i.e. NULL
}

int countBooks(struct Book *first)
{
	//check(first != NULL, "Null-pointer to 'struct Book *first'");
	if (! first) 	return 0;
	
	int count = 0;
	
	int loop = 1;
	struct Book *current = first;
	
	while (loop)
	{
		if(current->next == NULL)
		{
			loop = 0;
		}
		else
		{
			current = current->next;
		}
		count++;
	}
	
	return count;
	
error:
	return 0;
}

struct Book *getBook(struct Book *first_book, int search_id)
{
	/* This function searches for a book
	   with the given ID in the linked list given to it,
		and will return a pointer to the book with that ID.
		Returns NULL if not found.
	*/
	
	struct Book *found = NULL;
	
	struct Book *current_book = first_book;
	struct Book *next;

	if (! current_book)		// In case first_book is NULL or we are on the last book
		return NULL;
	
	while(current_book)
	{
		next = current_book->next;
		
		if (current_book->book_id == search_id)
		{
			found = current_book;
			break;
		}
		
		current_book = next;
	}

	return found;
	
error:
	return NULL;
}

void deleteBook(struct Book **first_book, struct Book *delete_book)
{	
	struct Book *current_book = *first_book;
	struct Book *prev = NULL;
	
	printf(" Deleting book #%d...   ", delete_book->book_id);

	if (! current_book || ! delete_book)		// In case first_book or delete_book is NULL
	{
		printf("error.\n");
		return;
	}
	
	/* Loop changing the pointers until current_book is delete_book, so that we get the book before */
	while(current_book->book_id != delete_book->book_id	&&
		  current_book->next)
	{
		prev = current_book; 		// to keep the pointer; current_book will increment.
		current_book = current_book->next;
	}
	
	if (prev) {
		prev->next = current_book->next;
	} else {
		*first_book = NULL;
	}
	
	free(current_book);
	
	printf("done.\n");
	return;
	
error:
	return;
} 

void freeBooks(struct Book **first_book)	// Double-pointer because we will be changing the original pointer
{
	struct Book *current_book = *first_book;
	struct Book *next;

	if (! current_book)		// In case first_book is NULL or we are on the last book
		return;
	
	while(current_book)
	{
		next = current_book->next;
		
		free(current_book);
		
		current_book = next;
	}

	*first_book = NULL;		// Change it to null since the current pointer would be invalid
	free(current_book);	
	return;
	
error:
	return;
}


char *i_to_str(int number)
{
	char *out = calloc(1, sizeof(char) * CMDLEN);
	
	sprintf(out, "%d", number);
	
	return out;
}