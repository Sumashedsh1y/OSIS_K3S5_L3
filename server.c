#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct info{
    int id;
    double x;
    double y;
    double z;
    double time;
    double temp;
    double dx;
};

int main()
{
    int sock, listener;
    struct sockaddr_in address;
    char buf[1024];
    int bytes_read;
    pid_t proc;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        printf("Socket dont created");
        exit(1);
    }
    printf("Socket created\n");
    
    address.sin_family = AF_INET;
    address.sin_port = htons(3425);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&address.sin_zero,sizeof(address.sin_zero));
    if(bind(listener, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Bind error");
        exit(2);
    }
    printf("Bind completed\n");

    if(listen(listener, 10)==-1)
    {
        printf("Server dont started listening");
        exit(3);
    }
    printf("Server started listening\n");

    while(true) {
        sock = accept(listener, NULL, NULL);
        if ((proc = fork()) == 0) {
            if(sock < 0){
                printf("Dont accepted");
                exit(4);
            }
            printf("Accept completed\n");

            char aunt[] = "Enter password (3 syms)\n";
            char bufpass[3];
            char pass[] = "onf";
            send(sock, aunt, sizeof(aunt), 0);
            bytes_read = recv(sock, bufpass, sizeof(bufpass), 0);
            if(bytes_read <= 0){
                printf("AUNT DONT OK\n");
                exit(5);
            }
            if(bufpass[0]==pass[0] && bufpass[1]==pass[1] && bufpass[2]==pass[2])
                printf("AUNT OK\n");
            else {
                printf("AUNT DONT OK\n");
                exit(5);
            }

            double X,Y,Z;
            recv(sock, &X, sizeof(X), 0);
            printf("x : %lf\n",X);
            recv(sock, &Y, sizeof(Y), 0);
            printf("y : %lf\n",Y);
            recv(sock, &Z, sizeof(Z), 0);
            printf("z : %lf\n",Z);

            struct info allinfo[10825];
            FILE *BD_Coords;

            printf("Start reading files\n");
            BD_Coords = fopen("BD_Coords.txt", "r");
            char _id1[100], _id2[100] , _x[100], _y[100], _z[100],_time[100],_temp[100],_dx1[100],_dx2[100];
            fscanf(BD_Coords, "%s\t%s\t%s\t%s\t%s\n", &_id1,&_id2,&_x,&_y,&_z);
            for (int i=0;i<10824;i++) {
                fscanf(BD_Coords, "%i\t%lf\t%lf\t%lf\n", &allinfo[i].id, &(allinfo[i].x), &(allinfo[i].y), &(allinfo[i].z));
            }

            FILE *BD;
            BD = fopen("BD.txt", "r");
            fscanf(BD, "%s\t%s\t%s\t%s\t%s\t%s\n", &_time,&_id1,&_id2,&_temp,&_dx1,&_dx2);
            for (int i=0;i<10824;i++) {
                fscanf(BD, "%lf\t%i\t%lf\t%lf\n", &allinfo[i].time, &allinfo[i].id, &allinfo[i].temp, &allinfo[i].dx);
            }
            printf("End reading files\n");

            printf("%s %s\t%s\t%s %s\t%s\n", _id1,_id2,_time,_dx1,_dx2,_temp);

            int z = 0;
            for (int i = 0;i < 10824; i++){
                if (allinfo[i].x==X && allinfo[i].z==Z && allinfo[i].y==Y){
                    z++;
                    printf("%i\t%3lf\t%2le\t%2lf\n",allinfo[i].id,allinfo[i].time,allinfo[i].dx,allinfo[i].temp);
                }
            }
            send(sock, &z, sizeof(z), 0);
            for (int i = 0;i < 10824; i++){
                if (allinfo[i].x==X && allinfo[i].z==Z && allinfo[i].y==Y) {
                    send(sock, &allinfo[i].time, sizeof(allinfo[i].time), 0);
                    send(sock, &allinfo[i].dx, sizeof(allinfo[i].dx), 0);
                    send(sock, &allinfo[i].temp, sizeof(allinfo[i].temp), 0);
                }
            }

            bool flag = true;
            while(flag){
                bytes_read = recv(sock, buf, 1024, 0);
                if(bytes_read <= 0) flag = false;
                send(sock, buf, bytes_read, 0);
            }
            printf(buf);
            fclose(BD_Coords);
            fclose(BD);
            close(sock);
        }
        else if (proc < 0) {
            printf("Error\n");
            close(sock);
            break;
        }
    }
    close(listener);
    return 0;
}