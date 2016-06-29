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

void deleteBook(struct Book *first_book, struct Book *delete_book);
void freeBooks(struct Book *first);

void printhelp();

int main()
{
	printf(" -- Books -- \n");
	
	struct Book *first_book = NULL;
	
	int bookCount = 0;
	
	char cmd[CMDLEN];
	int loop = 1;
	while (loop)
	{

		printf(" > ");
		fgets(cmd, CMDLEN, stdin);
		denewline(cmd);
		
		if ( command_is("l") )
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
		
		
		if ( command_is("d") )
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
		
		
		if ( command_is("c") )
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
		
		
		if ( command_is("a") )
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
		
		if( command_is("r") )
		{
			int  id;
			char id_str[CMDLEN];
			
			printf(" Book ID: ");
			fgets(id_str, CMDLEN, stdin);
			denewline(id_str);
			id = atoi(id_str);
			
			struct Book *del_book = getBook(first_book, id);	// Get the pointer to the book we want to delete,

			deleteBook(first_book, del_book);
		}
		
		if ( command_is("s") )
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
		
		if ( command_is("o") )
		{
			char address[STRLEN];
			
			printf(" Load: ");
			fgets(address, STRLEN, stdin);
			denewline(address);
			
			freeBooks(first_book);
			first_book = NULL;
			
			first_book = books_file_read(address);
			
			/*
			if (first_book == NULL) {
				printf("failed.\n");
				continue;
			}	*/
			
			bookCount = countBooks(first_book);
		}
		
		if ( command_is("x") )
		{
			freeBooks(first_book);
			first_book = NULL;
		}
		
		if ( command_is("h") || command_is("help") )
		{
			printhelp();
		}
		
		if ( command_is("q") )
		{
			freeBooks(first_book);
			loop = 0;
		}
	}
	
	return 0;
	
error:
	if (first_book) 	freeBooks(first_book);
	return 0;
}

void printhelp()
{
	printf(" A = Add a new book\n");
	printf(" R = Remove a book\n");
	
	printf(" \n");
	
	printf(" L = List all books\n");
	printf(" D = List all books with details\n");
	printf(" C = Count the number of books\n");
	
	printf(" \n");
	
	printf(" S = Save the book collection to a file\n");
	printf(" O = Open a book collection from a file\n");
	
	printf(" \n");
	
	printf(" X = Delete the current collection\n");
	
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
	
	while(current_book->next)
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

void deleteBook(struct Book *first_book, struct Book *delete_book)
{	
	struct Book *current_book = first_book;
	struct Book *prev = NULL;

	if (! current_book || ! delete_book)		// In case first_book is NULL or we are on the last book
		return;
	
	printf(" Deleting book #%d...   ", delete_book->book_id);
	
	while(current_book->book_id != delete_book->book_id	&&
		  current_book->next)
	{
		prev = current_book; 		// to keep the pointer; current_book will increment.
		current_book = current_book->next;
	}
	prev->next = current_book->next;
	free(current_book);
	
	printf("done.\n");
	return;
	
error:
	return;
}

void freeBooks(struct Book *first_book)
{
	struct Book *current_book = first_book;
	struct Book *next;

	if (! current_book)		// In case first_book is NULL or we are on the last book
		return;
	
	while(current_book->next)
	{
		next = current_book->next;
		
		free(current_book);
		
		current_book = next;
	}

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