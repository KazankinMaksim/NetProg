#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h> // Добавлено для поддержки функции exit
#include <arpa/inet.h> // Добавлено для поддержки функцииinet_ntoa

int main(){ // Добавлен возвращаемый тип для функции main
	int s, rd, proto;
	socklen_t clen;
	struct sockaddr_in saddr, caddr;
	struct sockaddr *sa, *ca;
	struct hostent *rhost;
	time_t itime;
	char buf[2048], *tstr, *host;

	sa=(struct sockaddr*)&saddr; // Исправлено приведение типов
	ca=(struct sockaddr*)&caddr; // Исправлено приведение типов

	// Получаем номер протокола UDP
	proto=getprotobyname("udp")->p_proto;

	// Создаем сокет
	s=socket(PF_INET, SOCK_DGRAM, proto);
	if(s<0)
	{
		perror("udps: не удается создать сокет");
		exit(1);
	}

	// Резервируем порт 13
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	saddr.sin_port=htons(13);

	if(bind(s, sa, sizeof(saddr))==-1)
	{
		perror("udps: не удается занять порт");
		exit(1);
	}

	caddr.sin_family=AF_INET;
	clen = sizeof(caddr); // Изменено на socklen_t

	while(1)
	{
		// Ожидаем поступления запроса
		rd=recvfrom(s, buf, 1, 0, ca, &clen); // Исправлено на &clen
		if(rd==-1)
		{
			perror("udps: ошибка при получении данных");
			exit(1);
		}

		// Преобразуем адрес хоста отправителя в его имя
		rhost=gethostbyaddr((char*)(&caddr.sin_addr),
		sizeof(caddr.sin_addr), AF_INET);
	if(h_errno)
	{
		printf("gethostbyaddr error: %d\n", h_errno);
		host=inet_ntoa(caddr.sin_addr);
	}
	else
	{
		host=rhost->h_name;
	}

	// Получаем строку содержащую дату и время
	itime=time(NULL);
	tstr=ctime(&itime);

	// Выводим время поступления запроса,
	// адрес и порт отправителя
	printf("%s request from %s:%d\n", tstr, host,
	htons(caddr.sin_port));

	// Отправляем дату и время клиенту
	sendto(s, tstr, strlen(tstr), 0, ca, sizeof(caddr));

	}
	return 0; // Добавлен возврат значения из функции main
}
