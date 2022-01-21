#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	char line[10240];
	
	/* 명령어 수행에 대한 pipe를 호출함 */
	if((fp = popen("ls -al", "r")) == NULL) {
		return 1;
	}
	
	/* ls -al 명령어로 출력하는 내용을 한줄씩 읽어서 처리하는 함 */
	while(fgets(line, 10240, fp) != NULL) {
		printf("%s", line); }
	pclose(fp);
	
	return 0;
}
