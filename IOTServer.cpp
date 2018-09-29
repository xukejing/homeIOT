#include <sys/types.h>
#include <sys/socket.h>
#include<pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>  
#include <netdb.h> 
#include <errno.h> 
#include <mysql/my_global.h>
#include <mysql/mysql.h>

#define err_sys(msg) \
	do { perror(msg); exit(-1); } while(0)
#define err_exit(msg) \
	do { fprintf(stderr, msg); exit(-1); } while(0)

void *thread_udp_server(void *arg);
void *thread_mysql_cilent(void *arg);
void udpBuff2int(char *inChar, int *outData);
static int is_udp = 0;
static int mysql_cilent_idle = 1;
static int udp_Data[8] = { 0 };
int main()
{
	pthread_t tid_udp_server;
	char* p_udp_server = NULL;

	pthread_create(&tid_udp_server, NULL, thread_udp_server, NULL);

	pthread_t tid_mysql_cilent;
	char* p_mysql_cilent = NULL;

	pthread_create(&tid_mysql_cilent, NULL, thread_mysql_cilent, NULL);
	sleep(5);
	pthread_join(tid_mysql_cilent, (void **)&p_mysql_cilent);;
	pthread_join(tid_udp_server, (void **)&p_udp_server);

	//printf("message: %s\n", p_udp_server);

	return 0;
}
void *thread_udp_server(void *arg)
{
	time_t timep;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(60001);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	printf("udp server is started!\n");
	char buff[64];
	int udpData[8] = {0};
	struct sockaddr_in clientAddr;
	int n;
	int len = sizeof(clientAddr);
	int i;
	while (1)
	{
		n = recvfrom(sock, buff, 64, 0, (struct sockaddr*)&clientAddr, &len);
		if (n > 0)
		{
			buff[n] = '\0';
			printf("%s %u says: %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);
			switch (buff[0])
			{
			case 's':
				udpBuff2int(buff, udpData);
				time(&timep);
				printf("%d\t%d\t%d\t%s", udpData[0], udpData[1], udpData[2], ctime(&timep));
				if (mysql_cilent_idle == 1)
				{
					for (i = 0; i < 8; i++)
						udp_Data[i] = udpData[i];
					printf("udp data is recved!\n");
					is_udp = 1;
				}
				else
					printf("mysql cilent is busy!\n");
				break;
			default:
				break;
			}	
			n = 0;
		}	
	}
}
void udpBuff2int(char *inChar,int *outData)
{
	int i,j;
	i = 0;
	j = 0;
	for (i = 0; i < 8; i++)
		outData[i] = 0;
	for (i = 1; i < 64; i++)
	{
		if (inChar[i] == ',')
		{
			j++;
			if (j >= 8)
				return;
		}
		else
		{
			outData[j] = outData[j] * 10 + (inChar[i] - '0');
		}
	}
}
void *thread_mysql_cilent(void *arg)
{
	char query[256];
	int i=0;
	time_t timep;
	printf("MySQL client version: %s\n", mysql_get_client_info());
	MYSQL *mysql;
	mysql = mysql_init(NULL);
	if (!mysql_real_connect(mysql, "192.168.1.102", "apache", "1q2w3e4r", "home_iot", 3306, NULL, 0))
	{
		printf("error in home_iot connecting!\n");
		exit(0);
	}
	else
	{
		printf("DB of home_iot is connected!\n");
	}
	while (1)
	{
		switch (is_udp)
		{
		case 1:
			mysql_cilent_idle = 0;
			printf("inserting data to Table...\n");
			time(&timep);
			sprintf(query, "insert into ctest values ('%d','%d','%d','%d','%s')", i, udp_Data[0], udp_Data[1], udp_Data[2], ctime(&timep));
			mysql_query(mysql, query);
			i++;
			printf("mysql cilent is idle!\n");
			is_udp = 0;
			mysql_cilent_idle = 1;
			break;
		default:
			break;
		}
	}
}

