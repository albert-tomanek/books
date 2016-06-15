#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for malloc
#include <assert.h>

#include "books.h"
#include "books_file.h"
#include "modules/text.h"

struct Book *getlastbook(struct Book *first);
char *getgenre(int genre);
char *i_to_str(int number);

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
		
		if ( command_is("p") )
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
			
			char address[STRLEN];
			
			printf(" Save as: ");
			fgets(address, STRLEN, stdin);
			denewline(address);
			
			books_file_write(first_book, address);
		}
		
		if ( command_is("l") )
		{
			char address[STRLEN];
			
			printf(" Load: ");
			fgets(address, STRLEN, stdin);
			denewline(address);
			
			first_book = NULL;
			
			first_book = books_file_read(address);
			
			assert(first_book != NULL);
			
			bookCount = countBooks(first_book);
		}
		
		if ( command_is("x") )
		{
			/* _MEMORY_LEAK_- FIX THIS*/
			first_book = NULL;
		}
		
		if ( command_is("h") || command_is("help") )
		{
			printhelp();
		}
		
		if ( command_is("q") )
		{
			loop = 0;
		}
	}
	
	return 0;
}

void printhelp()
{
	printf(" A = Add a new book\n");
	
	printf(" \n");
	
	printf(" P = Print a list of all books\n");
	printf(" D = Print a list of all books with details\n");
	printf(" C = Count the number of books\n");
	
	printf(" \n");
	
	printf(" S = Save the collection to a file\n");
	printf(" L = Load a collection from a file\n");
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
	}
	
	return current;
}

int countBooks(struct Book *first)
{
	assert(first != NULL);
	
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
}

char *i_to_str(int number)
{
	char *out = calloc(1, sizeof(char) * CMDLEN);
	
	sprintf(out, "%d", number);
	
	return out;
}