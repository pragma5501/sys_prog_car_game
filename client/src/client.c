#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include "client.h"
#include "handle_car.h"
#define SERVER_IP "127.0.0.1"

int sock;
ClientAction cAction;

sem_t map_lock;
sem_t map_t_lock;
MAP_t map_s;
pthread_t pthread_s;
pthread_t pthread_car;

void* handle_receive (void* arg) 
{
        printf("recive\n");
        DGIST* game_info_s = (DGIST*)arg;
        while (1) {
                sem_wait(&map_lock);
                if (recv(sock, game_info_s, sizeof(DGIST), 0) == 0) {
                        perror("recieve error");
                        exit(EXIT_FAILURE);
                        
                }
                usleep(100);
                
                for (int i = 0; i < MAP_ROW; i++) {
                        for (int j = 0; j < MAP_COL; j++) {
                                map_s.map[i][j].row = game_info_s->map[i][j].row;
                                map_s.map[i][j].col = game_info_s->map[i][j].col;
                                map_s.map[i][j].item = game_info_s->map[i][j].item.score;
                        }
                }
                
                sem_post(&map_lock);
                pthread_create(&pthread_car, NULL, handle_car, (void*)&map_s);
                pthread_join(pthread_car, NULL);
                printMap(game_info_s);
        }
}



int main(int argc, char** argv) 
{
        struct sockaddr_in address;
        

        sem_init(&map_lock, 0, 1);
        sem_init(&map_t_lock, 0, 1);

	const int PORT = atoi(argv[1]);

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("sock failed");
                exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_port = htons(PORT);
        if (inet_pton(AF_INET, SERVER_IP, &address.sin_addr) <= 0) {
                perror("inet_pton failed");
                exit(EXIT_FAILURE);
        }


        if (connect(sock, (struct sockaddr*)&address, sizeof(address)) == -1) {
                perror("connect failed");
                exit(EXIT_FAILURE);
        }

        printf("SUCCESS Connecting : %s\n", SERVER_IP);



        DGIST dgist;
        memset(&dgist, 0, sizeof(DGIST));

        pthread_create(&pthread_s, NULL, handle_receive, (void*)&dgist);
        pthread_join(pthread_s, NULL);

}

void send_cAction() {
        if (send(sock, &cAction, sizeof(ClientAction), 0) == -1) {
                perror("send() error");
        }
}


void printMap(void* arg) {
        DGIST* dgist = (DGIST*)arg;
        Item tmpItem;
        
        printf("==========PRINT MAP==========\n");
        sem_wait(&map_lock);
        for (int i = 0; i < MAP_ROW; i++) {
                for (int j = 0; j < MAP_COL; j++) {
                        tmpItem = (dgist->map[i][j]).item;
                        switch (tmpItem.status) {
                                case nothing:
                                printf("- ");
                                break;
                                case item:
                                printf("%d ", tmpItem.score);
                                break;
                                case trap:
                                printf("x ");
                                break;
                        }
                }
                printf("\n");
        }
        sem_post(&map_lock);
        printf("==========PRINT DONE==========\n");

}