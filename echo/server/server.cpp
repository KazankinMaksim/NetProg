#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Добавлено для поддержки функции exit
#include <arpa/inet.h> // Добавлено для поддержки функции inet_ntoa

int main(){ // Добавлен возвращаемый тип для функции main
	int s, new_s, rd;
	struct sockaddr_in saddr, caddr;
	socklen_t clen;
	char buf[100];

	// Создаем сокет
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s<0) {
		perror("udps: не удается создать сокет");
		exit(1);
	}

	// Резервируем порт 7
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	saddr.sin_port=htons(7);

	if(bind(s, (struct sockaddr*)&saddr, sizeof(saddr))==-1) {
		perror("udps: не удается занять порт");
		exit(1);
	}

	// Начинаем прослушивание соединений
	if(listen(s, 10) == -1) {
		perror("udps: ошибка при попытке прослушивания порта");
		exit(1);
	}

	clen = sizeof(caddr);

	while(1) {
		// Принимаем входящее соединение
		new_s = accept(s, (struct sockaddr*)&caddr, &clen);
		if(new_s == -1){
			perror("udps: ошибка при принятии входящего соединения");
			exit(1);
		}

		// Ожидаем поступления запроса
		rd = recv(new_s ,buf ,99 ,0);
		if(rd == -1){
			perror("udps: ошибка при получении данных");
			exit(1);
		}
		buf[rd] = '\0';

		printf("Received: %s\n", buf);

		// Отправляем обратно данные клиенту (эхо)
		send(new_s, buf, strlen(buf), 0);

		close(new_s); // Закрываем соединение с клиентом
	}
	return 0; // Добавлен возврат значения из функции main
}
