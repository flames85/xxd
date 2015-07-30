#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUF_SIZE 	(MAX_LINE_SIZE*100) /* Max number of bytes of data */
#define MAX_LINE_SIZE 	(16)

static const char *optString = "lth?";

bool showLineNum = false;
bool showText = false;

/* Display program usage, and exit.
 */
void display_usage( void )
{
	puts( "xxd <file-name> [-l -t -h -?]" );
	exit( EXIT_FAILURE );
}


void text(unsigned char* src, int len)
{
	if(src == NULL || len == 0) return;
	for(int i=0; i<len; i++)
	{
		if(*(src+i)> 0x1f && *(src+i) < 0x7e)
			printf("%c", *(src+i));
		else
			printf(".");
	}
}

void PrintText(unsigned char* buf, int column)
{
	if(showText)
	{
		int _column = column;
		for(; column < MAX_LINE_SIZE; column++)
		{
			printf("  "); // one byte
			if( (column+1) == MAX_LINE_SIZE ) break;
			if( (column+1) % 2 == 0) printf(" "); // split each 2 bytes
		}
		printf("  "); // split data and text
		text(buf - _column, _column);
	}
}

void PrintLineNum()
{
	if(showLineNum)
	{
		static long long int row = 0;
		printf("%07llx: ", (row++)*MAX_LINE_SIZE);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		display_usage();
	}

	char* filename = argv[1];

	/* Process the arguments with getopt(), then 
	 * populate globalArgs. 
	 */
	int opt = getopt( argc-1, &argv[1], optString );
	while( opt != -1 ) {
		switch( opt ) {
			case 'l':
				showLineNum = true;
				break;
			case 't':
				showText = true;
				break;
			case 'h':    /* fall-through is intentional */
			case '?':
			default:
				display_usage();
				break;
		}
		opt = getopt( argc-1, &argv[1], optString );
	}
	FILE *fp = NULL;  
	if( (fp = fopen(filename, "rb") ) == NULL )
	{	
		perror("open file fail");
		return -1;
	}

	int readLen = 0;
	unsigned char buf[MAX_BUF_SIZE] = {0};

	int column = 0;
	int readIndex = 0;
	//! first line num
	PrintLineNum();
	while(1)
	{  
		readLen = fread(buf, sizeof(unsigned char), MAX_BUF_SIZE, fp);  
		if(readLen <= 0 )
		{
			//! text
			PrintText(buf+readIndex, column);
			printf("\n");
			break;
		}

		for(readIndex = 0; readIndex < readLen; readIndex++ )
		{
			printf("%02x", buf[readIndex]);
			if( (column+1) == MAX_LINE_SIZE) 
			{
				//! text
				PrintText(buf+readIndex+1, column+1);
				column = 0;
				printf("\n");
				//! line num
				PrintLineNum();
			}
			else
			{
				if( (column+1) % 2 == 0) printf(" "); // split each 2 byte
				column ++;
			}
		}
	}  
	fclose(fp);  
	return 0;
}
