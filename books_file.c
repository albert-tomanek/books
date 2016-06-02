/* 
 *  Byte          Use
 *  1 - 10        magic 'number' ("COLLECTION")
 *  11            file version
 *  12            padding
 *  13, 14        number of books
 *  15, 16        padding
 *
 */


#define BOOKS_FILE_VERSION 1

void fputstrn(char *chars, FILE *file, int count);



void books_file_write(struct Book *first_book, char *file)
{
	int bookCount = countBooks(first_book);
	
	assert(bookCount < 65535);
	
	uint8_t bookCount_bytes[2];
	cvt16to8( (uint16_t) bookCount, bookCount_bytes);
	
	FILE *out = fopen(file, "w");
	assert(out != NULL);
	
	fputs("COLLECTION", out);
	fputc(BOOKS_FILE_VERSION, out);
	
	fputc(0x00, out); 		// padding
	
	fputc(bookCount_bytes[0], out);
	fputc(bookCount_bytes[1], out);
	
	fputc(0x00, out); 		// padding
	fputc(0x00, out);
	
	

	
}




void fputstrn(char *chars, FILE *file, int count)
{
	for (int i = 0; i < count; i++)
	{
		fputc(chars[i], file);
	}
}