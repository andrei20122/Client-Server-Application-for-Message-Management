#include <bits/stdc++.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <climits>
#include <cstring>
#include <string.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <map>
#define BUFLEN 1600
#include "utils.h"

using namespace std;

struct client{
    string id;
    int socket;
    vector<string> topics;
    bool connected;
};

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}



int main(int argc, char *argv[])
{
    char buff[1552];
    char new_buff[51];
    struct sockaddr_in serv_addr;
    // //se creaza socketul UDP
    int sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(sock_udp < 0, "socket_udp");
    //se creaza socketul TCP
    int sock_tcp = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sock_tcp < 0, "socket_tcp");
    //se atribuie numarul portului
    int port_no = atoi(argv[1]);
    /*Setare struct sockaddr_in pentru a asculta pe portul respectiv pentru UDP*/
    struct sockaddr_in addr_udp;
    socklen_t socklen = sizeof(addr_udp);
    struct sockaddr_in addr_tcp;
    //clilen_tcp = sizeof(addr_tcp);
    addr_udp.sin_family = AF_INET;
    addr_udp.sin_port = htons(port_no);
    addr_udp.sin_addr.s_addr = INADDR_ANY;
    // Completare in addr_tcp adresa serverului pentru bind, familia de adrese si portul rezervat pentru server
    memset((char *)&addr_tcp, 0, sizeof(addr_tcp));
    addr_tcp.sin_family = AF_INET;
    addr_tcp.sin_port = htons(port_no);
    addr_tcp.sin_addr.s_addr = INADDR_ANY;
    // se asociaza socketul UDP cu portul ales
    int b_udp = bind(sock_udp, (struct sockaddr *)&addr_udp, sizeof(addr_udp));
    DIE(b_udp < 0, "bind udp");

    // se asociaza socketul TCP cu portul ales
    int b_tcp = bind(sock_tcp, (struct sockaddr *)&addr_tcp, sizeof(struct sockaddr_in));
    DIE(b_tcp < 0, "bind tcp");

    int ret = listen(sock_tcp, INT_MAX);
    //Setarea descriptorilor socketilor
    fd_set tmp_fds, read_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    FD_SET(sock_tcp, &read_fds);
    FD_SET(sock_udp, &read_fds);        
    FD_SET(0, &read_fds);
    //int fdmax = sock_tcp;
    int fdmax;
    char id[256];
    //send_msg tcp_mess;
    int n;
    vector <client> c;
    int exista_ID=1;

    fdmax = max(sock_tcp,sock_udp);
    while (true)
    {
        tmp_fds = read_fds;
        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");
       

        for(int i=0;i<=fdmax;i++)
        {
            if(FD_ISSET(i, &tmp_fds))
            {
                if(i == sock_udp){
                    //se primeste mesaj de la clientul udp
                    memset(buff, 0, BUFLEN);
					socklen_t udp_socklen = sizeof(sockaddr);
                    int r = recvfrom(sock_udp, buff, 1552, 0, (struct sockaddr *)&addr_udp, &socklen);
                    DIE(r < 0, "accept udp");

                    //extrag din buff topic, datatype , mesaj si continut 
                    strncpy(new_buff,buff,50);//copiez primele 50 elem intr un nou buffer
                    string topic = ""; topic.append(new_buff);
                    for(int j=0;j<c.size();j++)//am trimis mai departe
                    {   
                        for(int l=0;l<c[j].topics.size();l++)
                        {
                            if(c[j].topics[l].compare(topic))
                            {
                                cout<<"Sending to "<<c[j].id<<"\n";
                                cout<<ntohl(*(uint32_t *)(buff + 52));
                                send(c[j].socket,buff,1552,0);
                            }
                        }
                    }

                } else if(i == sock_tcp){
                    //cout << "tcp";
                    int newsock = accept(i, (struct sockaddr *) &addr_tcp, &socklen);
					DIE(newsock < 0, "accept");
                    setsockopt(sock_tcp,IPPROTO_TCP,TCP_NODELAY,(char*)&exista_ID,sizeof(int));
                    FD_SET(sock_tcp,&read_fds);
                    fdmax = max(fdmax,newsock);
                    memset(buff, 0, BUFLEN);
					n = recv(newsock, buff, sizeof(buff), 0);
					DIE(n < 0, "recv");
                   
                    int ok =0;

                    for(int k=0;k<c.size();k++)
                    {
                        if(c[k].id.compare(buff) == 0)//daca am gasit id ul il trimit la client
                            {   cout << "Client " << c[k].id << " already connected.\n";
                                
                                if(c[k].connected == true){
                                    
                                    strcpy(buff, "NO");
                                    send(newsock, buff, strlen(buff), 0);
                                    close(newsock);
                                    FD_CLR(newsock,&read_fds);
                                }else{
                                    //strcpy(new_buff,"exit");
                                    // printf("New client %s connected from %s : %hu.\n", c[k].id,
                                    //     inet_ntoa(addr_tcp.sin_addr), ntohs(addr_tcp.sin_port));
                                    // send(sock_tcp,new_buff,strlen(new_buff),0);
                                    ok =1;
                                    // c[k].id.pop_back();
                                    c[k].socket = newsock;
                                    c[k].connected = true;
                                    while(c[k].topics.empty() == 0)
                                    {
                                        send(c[k].socket,&c[k].topics.front(),BUFLEN,0);
                                        c[k].topics.pop_back();
                                    }
                                }
                                
                                
                                ok = 1;
                                break;
                            }
                             else{
                                //nu face nimic
                               //cout << ".....\n";
                                
                            }
                           
                            
                    }
                   if(ok == 0)// adaug noul client
                   {    FD_SET(newsock,&read_fds);
                        fdmax = max(fdmax,newsock);
                        client new_c;
                        new_c.id = ""; new_c.id.append(buff);
                        new_c.connected = true;
                        new_c.socket = newsock;
                        new_c.topics = vector <string>();
                        c.push_back(new_c);
                        cout << "New client " << new_c.id << " connected from " 
                            << inet_ntoa(addr_tcp.sin_addr) << " :" << ntohs(addr_tcp.sin_port)<<"\n";
                   }
                   else{
                       continue;
                   }
                }else if(i==0){
                    //cout << "in";
                    string comanda_tastatura;
                    cin >> comanda_tastatura;
                    if(comanda_tastatura == "exit")
                    {
                        for (int g=0;g<c.size();g++)
                        {
                            strcpy(buff, "NO");
                            send(c[g].socket, buff, strlen(buff), 0);
                            close(c[g].socket);
                        }
                        close(sock_tcp);
                        close(sock_udp);
                        FD_ZERO(&read_fds);
                        FD_ZERO(&tmp_fds);
                        exit(0);
                    }
                }else{
                    //cout<<"someone";
                    memset(buff, 0, BUFLEN);
                    n = recv(i, buff, BUFLEN, 0);
                    DIE(n < 0, "recv");
                    //cout << "mama";
                    
                    if(strncmp(buff,":(((((((",8) == 0){
                        //cout<< "mama";
                        close(i);
                        FD_CLR(i,&read_fds);
                        for(client &x : c)
                        {
                            if(x.socket == i){
                                
                                //char new_buff[60];
                                x.connected = false;
                                //x.socket = -1;
                                cout << "Client " << x.id << " disconnected.\n";
                                
                                break;
                            }
                        }
                        for(int j=0;j<c.size();j++)
                        {
                            if(c[j].socket == i)
                            {
                                c.erase(c.begin() + j);
                                break;
                            }
                        }
                    }
                    // if(strncmp(buff,"exit",4) == 0)
                    // {
                    //     for(int y=0;y<c.size();y++)
                    //     {
                    //         if(c[y].socket == i){
                    //             c.erase(c.begin() + y);//sterge clientul
                    //             cout << "Clientul s-a deconectat\n";
                    //         }
                    //     }
                    //}
                    else if(strncmp(buff,"subscribe",9) == 0)
                    {
                        string topic = "";
                        topic.append(buff + 10, 50);
                        for(int h=0;h<c.size();h++)
                        {
                            if(c[h].socket == i){

                                c[h].topics.push_back(topic);
                                
                                // send(c[h].socket, buff, strlen(buff), 0);
                                // close(c[h].socket);                        
                            }
                        
                        }
                    }else if(strncmp(buff,"unsubscribe",11) == 0)
                    {
                        string top;
                        top.append(buff + 11, 50);
                        for(int h=0;h<c.size();h++)
                        {
                            if(c[h].socket == i){
                                for(int t=0;t<c[h].topics.size();t++)
                                {
                                    if(top == c[t].topics[t])
                                    {
                                        c[h].topics.erase(c[h].topics.begin()+t);
                                        break;
                                    }
                                }
                            
                            }
                        
                        }

                    }

                
                }
               
            }
        }




    }









    return 0;
}