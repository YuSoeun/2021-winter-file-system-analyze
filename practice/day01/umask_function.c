
#define _POSIX_SOURCE #include <fcntl.h>
#include <sys/stat.h> #include <sys/types.h> #include <unistd.h> #undef _POSIX_SOURCE #include <stdio.h> #include <stdlib.h>
int main() {
int fd;
mode_t oldmask; if(open("umask.file", O_RDWR) > 0) {		// 존재하면 0 이상이 출력
remove("umask.file"); }
//oldmask return your umask number and umask set new umask number
printf("Your old umask is %i\n",oldmask=umask(S_IRWXG));	// IW ??
// creat bit - new umask bit is umask.file’s permision.
if ((fd = creat("umask.file", S_IRWXU|S_IRWXG)) < 0)	// 파일에 관한 권한 설
	perror("creat() error");
else {
system("ls -l umask.file"); close(fd);
}
umask(oldmask); }
