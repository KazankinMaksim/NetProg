#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h> // Добавлено для поддержки функции exit
#include <arpa/inet.h> // Добавлено для поддержки функции inet_ntoa

int main(){ // Добавлен возвращаемый тип для функции main
	int s, so, rd, proto;
	socklen_t clen;
	struct sockaddr_in saddr, caddr;
	struct sockaddr *sa, *ca;
	struct hostent *rhost;
	struct timeval timeout;
	char buf[100], *host;

	sa=(struct sockaddr*)&saddr; // Исправлено приведение типов
	ca=(struct sockaddr*)&caddr; // Исправлено приведение типов

	// Получаем номер протокола UDP
	proto=getprotobyname("udp")->p_proto;
	
	// Создаем сокет
	s=socket(AF_INET, SOCK_DGRAM, proto);
	if(s<0) {
		perror("udpc: не удается создать сокет");
		exit(1);
	}

	// Разрешаем отпраку широковещательных пакетов
	so=1;
	rd=setsockopt(s,SOL_SOCKET,SO_BROADCAST,&so,sizeof(so));
	if(rd==-1) {
		perror("udpc: не удается установить параметры сокета");
		exit(1);
	}

	// Устанавливаем предельное время ожидания ответа
	timeout.tv_sec=3;
	timeout.tv_usec=0;
	rd=setsockopt(s,SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	if(rd==-1) {
		perror("udpc: не удается установить параметры сокета");
		exit(1);
	}

	// Резервируем порт
	caddr.sin_family=AF_INET;
	caddr.sin_addr.s_addr=INADDR_ANY;
	caddr.sin_port=0;

	if(bind(s, ca, sizeof(caddr))==-1) {
		perror("udpc: не удается занять порт");
		exit(1);
	}

	// Задаем адрес получателя
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(13);
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	clen=sizeof(saddr);

	// Отправляем запрос
	rd=sendto(s, buf, 1, 0, sa, clen);
	if(rd==-1){
		perror("udpc: ошибка при отправке запроса");
		exit(1);
	}	

	while(1){
	// Ожидаем ответ
	rd=recvfrom(s ,buf ,99 ,0 ,sa , &clen); // Исправлено на &clen
			if(rd==-1){
				// Если превышено время ожидания ответа, то выход
					if(errno==EAGAIN) break;
						// Иначе ошибка
						perror("udpc: ошибка при получении ответа");
						exit(1);
					}
	buf[rd]=(char)0;

	// Преобразуем адрес хоста отправителя в его имя
	rhost=gethostbyaddr((char*)(&saddr.sin_addr),
	sizeof(saddr.sin_addr), AF_INET);
	if(h_errno){
		printf("gethostbyaddr error: %d",h_errno);
		host=inet_ntoa(caddr.sin_addr);
	}
	else{
		host=rhost->h_name;
	}

	// Выводим информацию о поступившем ответе
	printf("%s - reply from %s\n", buf, host);
	}
		return 0; // Добавлен возврат значения из функции main
}
