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

// 메뉴 문자열을 2차원 배열로 구현
char *desc[] = {
	"###가천 과제 제출 시스템###\n",
	"1. 학생 정보 등록\n",
	"2. 과제 제출\n",
	"3. 과제 삭제\n",
	"4. 내 정보 확인\n",
	"5. 나가기\n"
};

// 사용자의 이름, 나이, 성별, 핸드폰 번호, 이메일, 과제 정보를 저장하기 위한 구조체
typedef struct user {
	char name[30];
	char age[30];
	char sex[30];
	char phone[30];
	char email[30];
	int assignment;
}USER;

void writeToFile(USER* users) // USER 구조체를 call by reference로 받음
{
	FILE* fp; // 파일 주소를 얻기 위한 파일 포인터

	// user 구조체의 사용자 정보 길이를 계산하여 동적 할당
	char *name = (char *)malloc(strlen("name: ") + strlen(users->name));
	char *age = (char *)malloc(strlen("age: ") + strlen(users->age));
	char *sex = (char *)malloc(strlen("sex: ") + strlen(users->sex));
	char *phone = (char *)malloc(strlen("phone: ") + strlen(users->phone));
	char *email = (char *)malloc(strlen("e-mail: ") + strlen(users->email));

	// 파일에 포맷에 맞춰 변수에 문자열 재저장
	sprintf(name, "name: %s", users->name);
	sprintf(age, "age: %s", users->age);
	sprintf(sex, "sex: %s", users->sex);
	sprintf(phone, "phone: %s", users->phone);
	sprintf(email, "e-mail: %s", users->email);
	
	// lists.txt 파일을 append 모드로 오픈
	fp = fopen("lists.txt", "a+");

	// 오픈한 lists.txt에 사용자의 정보를 입력함
	fputs(name, fp);
	fputs(age, fp);
	fputs(sex, fp);
	fputs(phone, fp);
	fputs(email, fp);
	fputs("assigment: 0\n", fp);

	// 파일을 닫는다.
	fclose(fp);

	printf("%s:등록완료\n", users->name);

	// 동적 할당된 주소들을 해제
	free(email);
	free(phone);
	free(sex);
	free(age);
	free(name);
}

void init(USER* users)
{
	// 입력 및 출력 버퍼 크기 조정
	setvbuf(stdin, 0, 2, 0);
	setvbuf(stdout, 0, 2, 0);

	// users 구조체 멤버 초기화
	memset(users->name, 0, 30);
	memset(users->age, 0, 30);
	memset(users->sex, 0, 30);
	memset(users->phone, 0, 30);
	memset(users->email, 0, 30);
}

void menu(int fd)
{
	unsigned int i;

	// 2차원 배열에 저장해 놓았던 문자열 for문으로 출력
	for(i=0; i<(sizeof(desc)/X64_SIZE); i++)
		write(fd, desc[i], strlen(desc[i]));
}

void add(int fd, USER* users)
{
	// 클라이언트에게 이름을 30만큼 입력받음
	write(fd, "\n이름: ", strlen("\n이름: "));
	read(fd, users->name, 30);
	
	// 클라이언트에게 나이를 30만큼 입력받음
	write(fd, "\n나이: ", strlen("\n나이: "));
	read(fd, users->age, 30);
	
	// 클라이언트에게 성별을 30만큼 입력받음
	write(fd, "\n성별(M/F): ", strlen("\n성별(M/F): "));
	read(fd, users->sex, 30);

	// 클라이언트에게 전화번호를 30만큼 입력받음
	write(fd, "\n전화번호: ", strlen("\n전화번호: "));
	read(fd, users->phone, 30);

	// 클라이언트에게 이메일을 30만큼 입력받음
	write(fd, "\n이메일: ", strlen("\n이메일: "));
	read(fd, users->email, 30);

	// 입력받은 정보를 토대로 파일에 사용자 정보 입력
	writeToFile(users);
}

void submit(int fd)
{
	FILE* fp;
	char tmp[255];
	char* pStr;
	char name[20];

	// 클라이언트에게 이름을 20만큼 입력받음
	write(fd, "\n이름을 입력해주세요: ", strlen("\n이름을 입력해주세요: "));
	read(fd, tmp, 20);
	// 입력받은 이름을 포맷에 맞게 재 저장
	sprintf(name, "name: %s", tmp);
	
	// lists.txt 파일을 read/write 모드로 오픈
	fp = fopen("lists.txt", "r+");
	
	// 파일의 끝이 아니라면 반복문으로 들어감
	while(!feof(fp))
	{
		// 파일을 한 줄 읽어서 pStr에 저장
		pStr = fgets(tmp, sizeof(tmp), fp);
		
		// 파일에서 읽어온 문자열과 이름이 일치한다면,
		if(!strcmp(pStr, name))
		{
			// assignemnt를 읽어오기 위해 seek를 4줄 down
			for(int i=0; i<4; i++)
				fgets(tmp, sizeof(tmp), fp);

			// assignment: 0을 assignment: 1로 변경
			fputs("assignment: 1\n", fp);
			write(fd, "\n제출이 완료되었습니다.\n", strlen("\n제출이 완료되었습니다.\n"));
			printf("%s 과제 제출\n", name);
		}
	}

	// 파일을 닫음.
	fclose(fp);
	
}

void delete(int fd)
{
	FILE* fp;
	char tmp[255];
	char* pStr;
	char name[20];

	// 클라이언트에게 이름을 20만큼 입력받음
	write(fd, "\n이름을 입력해주세요: ", strlen("\n이름을 입력해주세요: "));
	read(fd, tmp, 20);

	// 입력받은 이름을 포맷에 맞게 재 저장
	sprintf(name, "name: %s", tmp);

	// 파일을 염
	fp = fopen("lists.txt", "r+");
	
	// 파일의 끝이 아니라면,
	while(!feof(fp))
	{
		// pStr에 한줄 읽어옴
		pStr = fgets(tmp, sizeof(tmp), fp);
		
		// 읽어온게 입력한 이름과 같다면
		if(!strcmp(pStr, name))
		{
			// 파일의 seek를 4줄 down
			for(int i=0; i<4; i++)
				fgets(tmp, sizeof(tmp), fp);

			// assignemnt를 0으로 수정
			fputs("assignment: 0\n", fp);
			write(fd, "\n삭제가 완료되었습니다.\n", strlen("\n삭제가 완료되었습니다.\n"));
			printf("%s 과제 삭제\n", name);
		}
	}

	// 파일을 닫음.
	fclose(fp);
	
}

void info(int fd)
{
	FILE* fp;
	char tmp[255];
	char* pStr;
	char name[20];
	
	// 클라이언트에게 이름을 20만큼 입력받음
	write(fd, "\n이름을 입력해주세요: ", strlen("\n이름을 입력해주세요: "));
	read(fd, tmp, 20);

	// 포맺게 맞게 재 저장
	sprintf(name, "name: %s", tmp);
	
	// lists.txt read 모드로 오픈
	fp = fopen("lists.txt", "r");

	// 파일의 끝이 아니라면
	while(!feof(fp))
	{
		// 한줄 읽어와서 pStr에 저장
		pStr = fgets(tmp, sizeof(tmp), fp);

		// pStr과 name이 같다면
		if(!strcmp(pStr, name))
		{
			// 한 줄 읽어와 클라이언트에게 출력
			write(fd, pStr, strlen(pStr));
			
			// 나머지 모든 정보를 읽어와 클라이언트에게 출력
			for(int i=0; i<5; i++)
			{
				pStr = fgets(tmp, sizeof(tmp), fp);
				write(fd, pStr, strlen(pStr));
			}
		}
	}

	// 파일을 닫음
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
	
	USER	users;

	// users 구조체 초기화
	init(&users);

	// 소켓을 얻어온다. (PF_INET: 프로토콜 기반)
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(server_socket == -1)
	{
		printf("server socket 생성 실패\n");
		exit(1);
	}

	// sockaddr_in 구조체의 정보를 입력해 준다.
	// AF_INET: 주소 기반
	// 4000번 포트
	// 어떤 주소가 오든 상관 없음
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(4000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// 셋팅한 sockaddir_in 구조체의 정보를 서버의 소켓에 바인딩 해준다.
	if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		printf("bind() 실행 에러\n");
		exit(1);
	}

	while(1)
	{
		// 셋팅한 서버 소켓을 listening 상태로 변경한다.
		if(listen(server_socket, 5) == -1)
		{
			printf("listen() 실행 에러\n");
			exit(1);
		}

		// 클라이언트의 접속 요청이 들어오면 accpet한 후, 반환된 클라이언트 소켓을 저장한다.
		client_addr_size = sizeof(client_addr);
		client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
		if(client_socket == -1)
		{
			printf("client soekct 생성 실패\n");
			exit(1);
		}

		// 접속한 클라이언트를 자식 프로세스로 fork 시킨다.
		pid = fork();
		
		// 자식 프로세스라면,
		if(pid==0)
		{
			while(1)
			{
				// 메뉴 출력
				menu(client_socket);
				// 클라이언트에게 메뉴를 입력받음
				read(client_socket, sel, 4);

				switch(atoi(sel))
				{
					case 1:
						// 1번이라면 add 함수
						add(client_socket, &users);
						break;
					case 2:
						// 2번이라면 submit 함수
						submit(client_socket);
						break;
					case 3:
						// 3번이라면 delete 함수
						delete(client_socket);
						break;
					case 4:
						// 4번이라면 info 함수
						info(client_socket);
						break;
					case 5:
						// 5번이라면 close 함수
						close(client_socket);
					default:
						write(client_socket, "제대로 입력해주세요.\n", strlen("제대로 입력해주세요.\n"));
						break;
				}
			}
		}
		else
			// 클라이언트 소켓을 닫음
			close(client_socket);
	}
	return 0;
}
