#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFSIZE 512

int main(int argc, char *argv[]) {
	if(argc != 3){
		printf("./a.out <original file> <copyfile name>\n");
		exit(-1);
	}
int fromfd, tofd ;		//from fd = 읽기 전용, tofd = 읽기 쓰기
ssize_t nread;
char buf[BUFSIZE];
fromfd = open(argv[1], O_RDONLY);	// 읽기 전용으로 읽겠다
tofd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR);	// 쓰기 전용, 파일 안 존재하면 새로 만들겠다, 파일 있으면 초기화를 시키겠다. 권한에 관한(소유자에 대한 읽기 권한 허락, 쓰기 허락)
while ((nread = read(fromfd, buf, sizeof(buf))) > 0) {
	if(write(tofd, buf,nread) != nread){
		exit(-1);
	}
}
close(fromfd); close(tofd); return 0;
}
