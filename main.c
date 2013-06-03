#include "global.h"


#define check_error(id,str) {\
if (id==-1){\
perror(str);\
exit(EXIT_FAILURE);\
}\
}

#define skip_error(id,str){\
if (id==-1)\
{\
perror(str);\
continue;\
}\
}

#define return_error(id,str){\
if (id==-1)\
{\
perror(str);\
return;\
}\
}

int port[100];
char root_dir[100][200];
int vhost_num;
int max_connect;

void process_request(int client_sockfd);
void fork_process_request(int client_sockfd);
void read_viutualhost_conf();
void read_conf();

int main(int argc, char **argv) {
    read_viutualhost_conf();
    read_conf();
    int i;
    for (i = 0; i < vhost_num; i++) {
        int pid = fork();
        skip_error(pid, "创建虚拟主机子进程时错误");

        if (pid == 0) {
            current_dir = root_dir[i];
            current_port = port[i];
            int server_sockfd, client_sockfd;
            int server_len, client_len;
            struct sockaddr_in server_address;
            struct sockaddr_in client_address;


            server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
            check_error(server_sockfd, "socket出错");


            server_address.sin_family = AF_INET;
            server_address.sin_addr.s_addr = htonl(INADDR_ANY);
            srand((unsigned int) (time(NULL)));
            server_address.sin_port = htons(current_port);
            server_len = sizeof (server_address);
            int bind_id = bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
            check_error(bind_id, "bind出错");

            int listen_id = listen(server_sockfd, max_connect);
            check_error(listen_id, "listen出错");
            while (1) {
                printf("server at port %d in %s waiting\n", current_port, current_dir);
                client_len = sizeof (client_address);
                client_sockfd = accept(server_sockfd,
                        (struct sockaddr *) &client_address, &client_len);
                skip_error(client_sockfd, "accept出错");

                process_request(client_sockfd);


            }

            exit(EXIT_SUCCESS);
        }
    }

    exit(EXIT_SUCCESS);
}

void read_viutualhost_conf() {
    int fd;
    char str[100000];
    int i = 0;
    char *str_t = str;

    fd = open("vhost.conf", O_RDONLY);
    check_error(fd, "打开vhost.conf出错");
    int read_id = read(fd, str, sizeof (str));
    check_error(read_id, "读vhost.conf出错");


    while (sscanf(str_t, "listen %d,dir=%s", &port[i], (char *) (&root_dir[i])) == 2) {
        char *temp = strchr(str_t, '\n');
        str_t = temp + 1;
        i++;
    }

    vhost_num = i;

}

void read_conf() {
    int fd;
    char str[100000];

    fd = open("ml_server.conf", O_RDONLY);
    check_error(fd, "打开ml_server.conf出错");
    int read_id = read(fd, str, sizeof (str));
    check_error(read_id, "读xl_server.conf出错");

    if (sscanf(str, "server_name=%s", server_name) != 1) {
        perror("服务器启动错误，无server_name");
        exit(EXIT_FAILURE);
    }

    if (sscanf(str, "%*s\nmax_connect=%d", &max_connect) != 1) {
        max_connect = 10;
    }



    printf("%d\n", max_connect);
    printf("%s\n", server_name);

}

void process_request(int client_sockfd) {
    int pid = fork();
    return_error(pid, "fork出错");

    if (pid == 0) {
        fork_process_request(client_sockfd);
    }

}

void fork_process_request(int client_sockfd) {
    char buf[REQUEST_STR_MAX];
    char get_resource[MAX_GET_STR];

    int readnum = recv(client_sockfd, buf, REQUEST_STR_MAX, 0);
    check_error(readnum, "read出错了");
    buf[readnum] = '\0';
    printf("%s\n", buf);


    sscanf(buf, "%[^[\n]]", get_resource);
    sscanf(get_resource, "%*s %s %*s", get_resource);
    printf("%s\n", get_resource);
    /*
            char *buf_t=buf;
            int len;
            do {
               sscanf(buf_t, "%[^[\n]]", line);
               if (buf_t[0]=='\0')
                   break;

                len=strlen(line);
                buf_t+=(len+1);
                printf("%s\n",line);
            }while(1);
     */
    dup2(client_sockfd, STDOUT_FILENO);
    response_resource(get_resource);

    close(client_sockfd);
    exit(EXIT_SUCCESS);
}

