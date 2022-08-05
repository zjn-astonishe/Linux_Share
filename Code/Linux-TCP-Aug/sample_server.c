#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <strings.h>

#include "wrap.h"

#define SERV_PORT 9999

void catch_child(int signum){
	
	while((waitpid(0, NULL, WNOHANG) > 0));
	return;
}

int main(int argc, char *argv[])
{
	int lfd, cfd;
	pid_t pid;
	struct sockaddr_in serv_addr, clt_addr;
	socklen_t clt_addr_len;
	int ret, i;
	char buf[BUFSIZ];

	//memset(&serv_addr, 0, sizeof(serv_addr));
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	lfd = Socket(AF_INET, SOCK_STREAM, 0);

	Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	Listen(lfd, 128);
	clt_addr_len = sizeof(clt_addr);
	while(1){
		cfd = Accept(lfd, (struct sockaddr *)&clt_addr, &clt_addr_len);
		pid = fork();
		if(pid < 0){
			perr_exit("fork error");
		}else if(pid == 0){
			close(lfd);
			break;
		}else{
			struct sigaction act;
			act.sa_handler = catch_child;
			sigemptyset(&act.sa_mask);
			act.sa_flags = 0;
            
			ret = sigaction(SIGCHLD, &act, NULL);
			if(ret != 0)
				perr_exit("sigaction error");
			close(cfd);
			continue;
		}

	}

	if(pid == 0){
		for(;;){
		ret = Read(cfd, buf, sizeof(buf));
		if(ret == 0){
			close(cfd);
			exit(1);
		}
		for(i = 0; i < ret; i++)
			buf[i] = toupper(buf[i]);
		write(cfd, buf, ret);
		write(STDOUT_FILENO, buf, ret);
		}	
	}
	return 0;
}
