#include "global.h"

void response_resource(char *resource) {
    char *file;
    int len;
    struct stat sb;
    char location[1000];



    if (chdir(current_dir) == -1)
        send_error(400, "Bad Request", (char*) 0, "Server Internal Error.");

    if (resource[0] != '/')
        send_error(400, "Bad Request", (char*) 0, "Bad filename.");
    file = &(resource[1]);
    //  strdecode( file, file );
    if (file[0] == '\0')
        file = "./";
    len = strlen(file);
    //这里的检查file字符串的目的就是防止程序访问到服务器根目录以外的地方，比如说/home/xxx或者../jj等形式的字符串
    if (file[0] == '/' || strcmp(file, "..") == 0 || strncmp(file, "../", 3) == 0 || strstr(file, "/../") != (char*) 0 || strcmp(&(file[len - 3]), "/..") == 0)
        send_error(400, "Bad Request", (char*) 0, "Illegal filename.");
    if (stat(file, &sb) < 0)
        send_error(404, "Not Found", (char*) 0, "File not found.");

    if (S_ISDIR(sb.st_mode)) {
        //如果直接的请求一个目录的话，路径名一定要以'/'结尾
        if (file[len - 1] != '/') {
            (void) snprintf(
                    location, sizeof (location), "Location: %s/", resource);
            send_error(302, "Found", location, "Directories must end with a slash.");
        }
        //默认是取得index.html作为默认文件,关于php的以后再添加
        (void) snprintf(location, sizeof (location), "%sindex.html", file);
        if (stat(location, &sb) >= 0) {
            //现在file指向目标文件
            file = location; //加上
            do_file(file, &sb);
            return;
        }
        //执行到这里说明没有index.html这个文件，所以只能列出文件来了
        send_dir_list(file, &sb);
    } else {
        do_file(file, &sb);
    }

}

void do_file(char *file, struct stat *sb) {
    char *mime_type_str;
    FILE *fp;
    int ich;

    fp = fopen(file, "r");
    if (fp == (FILE*) 0)
        send_error(403, "Forbidden", (char*) 0, "File is protected.");

    if (is_php_file(file)) {
        process_php_file(file, sb);
        return;
    }
    mime_type_str = get_mime_type(file);
    send_headers(200, "Ok", (char*) 0, mime_type_str, sb->st_size, sb->st_mtime);
    while ((ich = getc(fp)) != EOF)
        putchar(ich);
}

void send_dir_list(char *file, struct stat *sb) {
    struct dirent **dl;
    int i;
    int dir_file_num;
    send_headers(200, "Ok", (char*) 0, "text/html", -1, sb->st_mtime);
    (void) printf("<html><head><title>Index of %s</title></head>\n<body><h4>Index of %s</h4><div id=\"link_list\">", file, file);
    //扫描该目录下的文件，n为数目。dl应该存储的是一个迭代器有关的信息
    (void) printf("<style>*{padding:0px;margin:0px;} #link_list a{line-height:2em;text-decoration:underline;color:blue;} #link_list a:hover{text-decoration:none;}</style>");
    dir_file_num = scandir(file, &dl, NULL, alphasort);
    if (dir_file_num < 0)
        perror("scandir出错");
    else//遍历输出文件列表
        for (i = 0; i < dir_file_num; ++i)
            file_details(file, dl[i]->d_name);
    (void) printf("</div><hr>\n<address><a href=\"/\">%s</a></address>\n</body></html>\n", server_name);
}

void send_error(int status, char* title, char* extra_header, char* text) {
    send_headers(status, title, extra_header, "text/html", -1, -1);
    (void) printf("<html><head><title>%d %s</title></head>\n<body><h4>%d %s</h4>\n", status, title, status, title);
    (void) printf("%s\n", text);
    (void) printf("<hr>\n<address><a href=\"/\">%s</a></address>\n</body></html>\n", server_name);
    (void) fflush(stdout);
    exit(EXIT_FAILURE);
}

void send_headers(int status, char* title, char* extra_header, char* mime_type, off_t length, time_t mod) {
    time_t now;
    char timebuf[100];
    //这里的\015和\012其实是回车键和换行键的意思
    (void) printf("%s %d %s\015\012", PROTOCOL, status, title);
    (void) printf("Server: %s\015\012", server_name);
    //这里的now是一个long值，其值等于从1970年1月1日至今的秒数
    now = time((time_t*) 0);
    //将秒数转变为规范的日期字符串
    (void) strftime(timebuf, sizeof (timebuf), RFC1123FMT, gmtime(&now));
    (void) printf("Date: %s\015\012", timebuf);
    if (extra_header != (char*) 0)
        (void) printf("%s\015\012", extra_header);
    if (mime_type != (char*) 0)
        (void) printf("Content-Type: %s\015\012", mime_type);
    if (length >= 0)
        (void) printf("Content-Length: %lld\015\012", (int64_t) length);
    if (mod != (time_t) - 1) {
        (void) strftime(timebuf, sizeof (timebuf), RFC1123FMT, gmtime(&mod));
        (void) printf("Last-Modified: %s\015\012", timebuf);
    }
    (void) printf("Connection: close\015\012");
    (void) printf("\015\012");
}

char* get_mime_type(char* name) {
    char* dot;

    dot = strrchr(name, '.');
    if (dot == (char*) 0)
        return "text/plain; charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0 || strcmp(dot, ".php") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp(dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";
    return "text/plain; charset=utf-8";
}

void file_details(char* dir, char* name) {
    // static char encoded_name[1000];
    char path[2000];
    struct stat sb;
    char timestr[16];

    //  strencode( encoded_name, sizeof(encoded_name), name );
    (void) snprintf(path, sizeof (path), "%s/%s", dir, name);
    //和软链接有关的
    if (lstat(path, &sb) < 0)
        (void) printf("<div><a href=\"%s\">%-32.32s</a></div> ?????", name, name);
    else {
        (void) strftime(timestr, sizeof (timestr), "%d%b%Y %H:%M", localtime(&sb.st_mtime));
        (void) printf("<div><a  href=\"%s\">%-32.32s</a>&nbsp;&nbsp;&nbsp;%15s %14lld</div>", name, name, timestr, (int64_t) sb.st_size);
    }
}
