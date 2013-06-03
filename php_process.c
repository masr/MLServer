#include "global.h"

void process_php_file(char *file, struct stat *sb) {
    FILE *pin;
    char buf[10000];
    char cmd[200];
    int n;
    sprintf(cmd, "php-cgi -f %s%s", current_dir, file);
    pin = popen(cmd, "r");
    send_headers(200, "Ok", (char*) 0, "text/html; charset=utf-8", sb->st_size, sb->st_mtime);
    while ((n = fread(buf, sizeof (char), 10000, pin)) > 0) {
        printf("%s", buf);
    }

}

int is_php_file(char *file) {
    char* dot;
    dot = strrchr(file, '.');

    if (strcmp(dot, ".php") == 0)
        return 1;
    else
        return 0;
}
