#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "utils.h"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_address server_port\n", file);
	exit(0);
}
using namespace std;

int main(int argc, char *argv[]) { 

    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    
    char buff[BUFFLEN];
    struct sockaddr_in serv_addr;
    int sockfd, n, ret;
    int nag = 1;

    fd_set read_fds;
    fd_set tmp_fds;

    // incep si golesc multimile de descriptori utilizate
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    
    DIE (argc < 4, "Mai trebuie parametrii:((\n");

    // creez socketul
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

	DIE (sockfd < 0, "eroare: n am creat tcp\n");
    // Dezactivam algortimul lui Nagle
    ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &nag, sizeof(int));
	DIE (ret < 0, "Setsockopt error\n");
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[3]));
    ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE (ret == 0, "Inet_aton error\n");

    // conectez clientul la server
    //cout<< "ceva\n";
    ret = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	DIE (ret < 0, "Nu s a conectat clientul (TCP)\n");
    //cout << "ccccc\n";
    // idul serverului
    ret = send(sockfd, argv[1], strlen(argv[1]), 0);
	DIE (ret < 0, "not sended ID\n");

    // adaug noii descriptori in multimea read_fds
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    FD_SET(sockfd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    int fdmax = max(STDIN_FILENO,sockfd);

    while (1) {
        tmp_fds = read_fds;

        // Alege un socket pentru interactiune
        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE (ret < 0, "file descriptor => not select\n");

        // Verificam daca socketul apartine multimii tmp_fds
        if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {
            // Citim de la tastatura comenzile
            memset(buff, 0, BUFFLEN);
            fgets(buff, BUFFLEN - 1, stdin);
            buff[strlen(buff)-1] = '\0';

            if (strncmp (buff, "subscribe", 9) == 0) {
                ret = send (sockfd, buff, strlen(buff), 0);
                DIE (ret < 0, "failed");
                cout << "Subscribed to topic.\n";
            }else if (strncmp (buff, "unsubscribe", 11) == 0) {
                ret = send (sockfd, buff, strlen(buff), 0);
                DIE (ret < 0, "send cmp failed\n");
                cout << "Unsubscribed to topic\n";
            } else if (strncmp (buff, "exit", 4) == 0){
                char buff2[1600] = ":(((((((";
                send(sockfd,&buff2,strlen(buff2),0);
                break;
            }
            //printf("%s", buff);
        }
        
        
        if (FD_ISSET(sockfd, &tmp_fds)) {
            // Primim structura de la server
            memset(buff, 0, BUFFLEN);
            n = recv(sockfd, buff, 1552, 0);
            DIE (n < 0, "Recv error\n");
            if( strncmp(buff, "NO", 2) == 0 )
            {
                //cout<<"2";
                close(sockfd);
                return 0;
            }
           
            if (strncmp (buff, "exit", 4) == 0) {
				close(sockfd);
                //cout<<"3";
				return 0;
			} else if(n > 0)
            {
                char topic[51];
                strncpy(topic, buff, 50);
                cout<<topic<<" - ";

                int aux = buff[50];

				if( aux == 0 )
                {
                    printf("INT - ");
                    
                    if( buff[51] == 1 )
                        cout<<"-";
                    cout<<ntohl(*(uint32_t *)(buff + 52));
                }
                else if(aux == 1)
                {
                    printf("SHORT REAL  - ");
                    printf("%.2f\n", 1.0*ntohs(*(uint16_t *)(buff + 51))/100);
                }
                else if(aux == 2)
                {
                    printf("FLOAT  - ");
                    if( buff[51] == 1 )
                        printf("-");
                    
                    double poww = 1.0;
                    int exp = (*(uint8_t *)(buff + 56));

                    while(exp != 0){
                        poww *= 10.0;
                        exp--;
                    }

                    printf("%.4f\n", (double)(1.0*(ntohl(*(uint32_t *)(buff + 52))) / poww));
                }
                else
                {
                    printf("STRING  - ");
                    printf("%s\n", buff + 51);
			}
			}
        }
    }
    
    
    close(sockfd);

    return 0;
}