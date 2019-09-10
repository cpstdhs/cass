#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024
#define X64_SIZE 8
#define X86_SIZE 4

char *desc[] = {
	"###가천 과제 제출 시스템###\n",
	"1. 학생 정보 등록\n",
	"2. 과제 제출\n",
	"3. 과제 삭제\n",
	"4. 내 정보 확인\n",
	"5. 나가기\n"
};

typedef struct user {
	char name[30];
	char age[30];
	char sex[30];
	char phone[30];
	char email[30];
	int assignment;
}USER;

void writeToFile(USER* users)
{
	FILE* fp;
	char *name = (char *)malloc(strlen("name: ") + strlen(users->name));
	char *age = (char *)malloc(strlen("age: ") + strlen(users->age));
	char *sex = (char *)malloc(strlen("sex: ") + strlen(users->sex));
	char *phone = (char *)malloc(strlen("phone: ") + strlen(users->phone));
	char *email = (char *)malloc(strlen("e-mail: ") + strlen(users->email));

	sprintf(name, "name: %s", users->name);
	sprintf(age, "age: %s", users->age);
	sprintf(sex, "sex: %s", users->sex);
	sprintf(phone, "phone: %s", users->phone);
	sprintf(email, "e-mail: %s", users->email);
	
	fp = fopen("lists.txt", "a+");
	fputs(name, fp);
	fputs(age, fp);
	fputs(sex, fp);
	fputs(phone, fp);
	fputs(email, fp);
	fputs("assigment: 0\n", fp);
	fclose(fp);

	printf("%s:등록완료\n", users->name);

	free(email);
	free(phone);
	free(sex);
	free(age);
	free(name);
}

void init(USER* users)
{
	setvbuf(stdin, 0, 2, 0);
	setvbuf(stdout, 0, 2, 0);

	memset(users->name, 0, 30);
	memset(users->age, 0, 30);
	memset(users->sex, 0, 30);
	memset(users->phone, 0, 30);
	memset(users->email, 0, 30);
}
void menu(int fd)
{
	unsigned int i;
	for(i=0; i<(sizeof(desc)/X64_SIZE); i++)
		write(fd, desc[i], strlen(desc[i]));
}

void add(int fd, USER* users)
{
	write(fd, "\n이름: ", strlen("\n이름: "));
	read(fd, users->name, 30);
	
	write(fd, "\n나이: ", strlen("\n나이: "));
	read(fd, users->age, 30);
	
	write(fd, "\n성별(M/F): ", strlen("\n성별(M/F): "));
	read(fd, users->sex, 30);

	write(fd, "\n전화번호: ", strlen("\n전화번호: "));
	read(fd, users->phone, 30);

	write(fd, "\n이메일: ", strlen("\n이메일: "));
	read(fd, users->email, 30);

	writeToFile(users);
}

void submit(int fd)
{
	FILE* fp;
	char tmp[255];
	char* pStr;
	char name[20];

	write(fd, "\n이름을 입력해주세요: ", strlen("\n이름을 입력해주세요: "));
	read(fd, tmp, 20);
	sprintf(name, "name: %s", tmp);
	
	fp = fopen("lists.txt", "r+");
	
	while(!feof(fp))
	{
		pStr = fgets(tmp, sizeof(tmp), fp);
		printf("pStr: %s\nname: %s\n", pStr, name);
		
		if(!strcmp(pStr, name))
		{
			for(int i=0; i<4; i++)
				fgets(tmp, sizeof(tmp), fp);
			fputs("assignment: 1\n", fp);
			write(fd, "\n제출이 완료되었습니다.\n", strlen("\n제출이 완료되었습니다.\n"));
		}
	}
	fclose(fp);
	
}

void delete(int fd)
{
	FILE* fp;
	char tmp[255];
	char* pStr;
	char name[20];

	write(fd, "\n이름을 입력해주세요: ", strlen("\n이름을 입력해주세요: "));
	read(fd, tmp, 20);
	sprintf(name, "name: %s", tmp);
	
	fp = fopen("lists.txt", "r+");
	
	while(!feof(fp))
	{
		pStr = fgets(tmp, sizeof(tmp), fp);
		
		if(!strcmp(pStr, name))
		{
			for(int i=0; i<4; i++)
				fgets(tmp, sizeof(tmp), fp);
			fputs("assignment: 0\n", fp);
			write(fd, "\n삭제가 완료되었습니다.\n", strlen("\n삭제가 완료되었습니다.\n"));
		}
	}
	fclose(fp);
	
}

void info(int fd)
{
	FILE* fp;
	char tmp[255];
	char* pStr;
	char name[20];
	
	write(fd, "\n이름을 입력해주세요: ", strlen("\n이름을 입력해주세요: "));
	read(fd, tmp, 20);
	sprintf(name, "name: %s", tmp);
	
	fp = fopen("lists.txt", "r");

	while(!feof(fp))
	{
		pStr = fgets(tmp, sizeof(tmp), fp);
		printf("pStr: %s\nname: %s\n", pStr, name);

		if(!strcmp(pStr, name))
		{
			write(fd, pStr, strlen(pStr));
			
			for(int i=0; i<5; i++)
			{
				pStr = fgets(tmp, sizeof(tmp), fp);
				write(fd, pStr, strlen(pStr));
			}
		}
	}

	fclose(fp);
}


int main(void)
{
	char sel[4];
	int pid;

	int	server_socket;
	int	client_socket;
	int	client_addr_size;

	struct	sockaddr_in server_addr;
	struct	sockaddr_in client_addr;
	
	char	buf_recv[BUFF_SIZE+5];
	char	buf_send[BUFF_SIZE+5];
	
	USER	users;

	init(&users);

	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(server_socket == -1)
	{
		printf("server socket 생성 실패\n");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		printf("bind() 실행 에러\n");
		exit(1);
	}

	while(1)
	{
		if(listen(server_socket, 5) == -1)
		{
			printf("listen() 실행 에러\n");
			exit(1);
		}

		client_addr_size = sizeof(client_addr);
		client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
		if(client_socket == -1)
		{
			printf("client soekct 생성 실패\n");
			exit(1);
		}

		pid = fork();
		
		if(pid==0)
		{
			while(1)
			{
				menu(client_socket);
				read(client_socket, sel, 4);

				switch(atoi(sel))
				{
					case 1:
						add(client_socket, &users);
						break;
					case 2:
						submit(client_socket);
						break;
					case 3:
						delete(client_socket);
						break;
					case 4:
						info(client_socket);
						break;
					case 5:
						close(client_socket);
					default:
						write(client_socket, "제대로 입력해주세요.\n", strlen("제대로 입력해주세요.\n"));
						break;
				}
			}
		}
		else
			close(client_socket);
		/*
		read(client_socket, buf_recv, BUFF_SIZE);
		printf("receive: %s\n", buf_recv);

		sprintf(buf_send, "%d: %s", (int)strlen(buf_recv), buf_recv);
		write(client_socket, buf_send, strlen(buf_send)+1);
		*/
	}
	return 0;
}
