#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(void)
{
	FILE *fp;

	// Try to open a file that doesn't exist
	fp = fopen( "/Does/not/exist.c", "r");
	printf( "error number = %d, error str = %s\n", errno, strerror( errno));
	if (fp)
		fclose( fp);

	remove("/Does/not/exist.c");
	perror("error number = 2,");
	
	return 0;
}

