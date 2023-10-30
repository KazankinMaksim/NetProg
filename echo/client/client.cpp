#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Добавлено для поддержки функции exit
#include <arpa/inet.h> // Добавлено для поддержки функции inet_ntoa

int main(){ // Добавлен возвращаемый тип для функции main
	int s, rd;
	struct sockaddr_in saddr;
	char buf[100];

	// Создаем сокет
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s<0) {
		perror("udpc: не удается создать сокет");
		exit(1);
	}

	// Задаем адрес сервера
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(7); // Порт 7 обычно используется для службы echo
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// Подключаемся к серверу
	if(connect(s, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
		perror("udpc: не удается подключиться к серверу");
		exit(1);
	}

	// Отправляем запрос
	strcpy(buf, "Hello, server!");
	rd = send(s, buf, strlen(buf), 0);
	if(rd == -1){
		perror("udpc: ошибка при отправке запроса");
		exit(1);
	}

	// Ожидаем ответ
	rd = recv(s ,buf ,99 ,0);
	if(rd == -1){
		perror("udpc: ошибка при получении ответа");
		exit(1);
	}
	buf[rd] = '\0';
	
	printf("Received: %s\n", buf);

	return 0; // Добавлен возврат значения из функции main
}
