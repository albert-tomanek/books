/* 
 *  Byte          Use
 *  1 - 10        magic 'number' ("COLLECTION") 	\
 *  11            file version						|
 *  12            padding							|--  Header
 *  13, 14        number of books					|
 *  15, 16        padding							/
 *  17+ 		  Book chunks
 *
 *  Length        Use
 *  CMDLEN        Title 		\
 *  CMDLEN        Author		|
 *  STRLEN        File			|__ Book chunk
 *  1             padding		|
 *  1             Genre			|
 *  2             Book ID		/
 *  2             padding
 */

#include "books_file.h"
#include "modules/ints.h"
#include "modules/dbg.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <byteswap.h>

//#define checkend(F) if (feof(F)) break;	

/*struct Book_chunk
{
	char title [CMDLEN]; 	// 64
	char author[CMDLEN];	// 64
	char file  [STRLEN];	// 256
	
	uint8_t padding1;
	
	uint8_t genre;
	
	uint16_t book_id;
	
	uint16_t padding2;
};*/

void fputstrn(char *chars, FILE *file, int count);
unsigned char *p_fgetstrn(FILE *file, int count);
void fgetstrn(char *out, int count, FILE *in);

/*
struct col_header
{
	char mnumber[10];
	uint8_t format_ver;
	
	uint8_t padding1;
	
}*/

void books_file_write(struct Book *first_book, char *file)
{
	check(first_book != NULL, "*first_book is a null pointer.");
	
	int bookCount = countBooks(first_book);
	
	check(bookCount < 65535, "No more than 65534 books can be stored in a .col file.");
	
	uint8_t bookCount_bytes[2];
	cvt16to8( (uint16_t) bookCount, bookCount_bytes);
	
	FILE *out = fopen(file, "w");
	check(out != NULL, "-> Could not open file '%s'.", file);
	
	fputs("COLLECTION", out);
	fputc(BOOKS_FILE_VERSION, out);
	
	fputc(0x00, out); 		// padding
	
	fputc(bookCount_bytes[0], out);
	fputc(bookCount_bytes[1], out);
	
	fputc(0x00, out); 		// padding
	fputc(0x00, out);


	/** Writes each Book struct to the file.**/
	
	struct Book *current_book = first_book;
	
	uint8_t book_id_bytes[2];
	
	int loop = 1;
	while (loop)
	{
		
		fputstrn(current_book->title , out, CMDLEN);
		fputstrn(current_book->author, out, CMDLEN);
		fputstrn(current_book->file  , out, STRLEN);
		
		fputc(0x00, out); 		// padding
		
		fputc( (uint8_t) current_book->genre , out);
		
		cvt16to8( (uint16_t) current_book->book_id, book_id_bytes);
		fputc(book_id_bytes[0], out);
		fputc(book_id_bytes[1], out);
		
		fputc(0x00, out); 		// padding
		fputc(0x00, out);
		
		if (current_book->next == NULL)
		{
			loop = 0;
		}
		else
		{
			current_book = current_book->next;
		}
	}
	
	fclose(out);
	
	return;

error:
	if (out			 )  fclose(out);
	if (current_book ) 	free(current_book);
	if (first_book   ) 	free(first_book);
	return;
}

struct Book* books_file_read(char *file)
{
	/* Variable declarations */
	struct Book 	*current_book = NULL;
	struct Book		*first_book   = NULL;

	printf(" Reading file...   ");
	
	/* Opening the file */
	FILE *in = fopen(file, "r");
	//check(in != NULL, "Could not open file '%s'.", file);
	if (! in) goto error;	// To replace check().
	
	char mnumber[12]; 		//mnumber = magic number
	fgets(mnumber, 13, in);		// 13 and not 12 because fgetc reads n-1 bytes and not n bytes.	
	
	if ( strcmp(mnumber, "COLLECTION\1"))
	{
		printf("error.\n Wrong file format or version.\n");
		return NULL;
	}
	
	int bookCount = 0;
	uint8_t bookCount_bytes[2];
	bookCount_bytes[0] = fgetc(in);
	bookCount_bytes[1] = fgetc(in);
	
	bookCount = cvt8to16(bookCount_bytes[0], bookCount_bytes[1]);
	
	fgetc(in); 		// Two bytes padding
	fgetc(in);
	
	// End of header.
	
	if (bookCount <= 0)
	{
		printf("done.\n No books in collection!\n");
		return NULL;
	}
	
	for (int i = 0; i < bookCount; i++)
	{
		current_book = calloc(1, sizeof(struct Book));
		
		if (first_book == NULL)
		{
			first_book = current_book;
		}
		else
		{
			getlastbook(first_book) ->next = current_book;
		}
		
		fgetstrn(current_book->title,  CMDLEN, in);
		fgetstrn(current_book->author, CMDLEN, in);
		fgetstrn(current_book->file,   STRLEN, in);
			
		fgetc(in); 		// padding
		
		current_book->genre = (uint8_t) fgetc(in); 	// genre byte		
		uint8_t id_1st = fgetc(in); 	// book ID, 2 bytes
		uint8_t id_2nd = fgetc(in);
		
		current_book->book_id = (int) cvt8to16(id_1st, id_2nd);
		
		fgetc(in); 	// *more* padding
		fgetc(in);

	}
	printf("done.\n");
	
	current_book->next = NULL;

	return first_book;

error:
	
	if (in			 )  fclose(in);
	if (current_book ) 	free(current_book);
	if (first_book   ) 	free(first_book);
	return NULL;
}


void fputstrn(char *chars, FILE *file, int count)
{
	for (int i = 0; i < count; i++)
	{
		fputc(chars[i], file);
	}
}


unsigned char *p_fgetstrn(FILE *file, int count)
{
	unsigned char *out = calloc(count, sizeof(unsigned char));
	for (int i = 0; i < count; i++)
	{
		out[i] = fgetc(file);
	}
	
	return out;
}

void fgetstrn(char *out, int count, FILE *in)
{
	for (int i = 0; i < count; i++)
	{
		out[i] = fgetc(in);
	}
}