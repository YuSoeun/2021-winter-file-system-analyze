#include <unistd.h>		// POSIX 운영체제 API에 대한 액세스를 제공하는 헤더파일
#include <stdio.h>

int main()
{
#ifdef  _POSIX_VERSION
	printf("\t_POSIX_VERSION = %ld\n", (long)_POSIX_VERSION);
#else
	printf("\tNot POSIX\n");
#endif

#ifdef  _XOPEN_UNIX
	printf("\tX/OPEN\n");
	#ifdef _XOPEN_VERSION
        	printf("\t_XOPEN_VERSION = %ld\n", (long)_XOPEN_VERSION);
	#else   
        	printf("\tError _XOPEN_UNIX defined, but not _XOPEN_VERSION\n");
	#endif
#else
	printf("\tNOT X/OPEN\n");
#endif

	return 0;
}
        
