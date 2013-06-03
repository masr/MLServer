/* 
 * File:   resource_action.h
 * Author: maosuhan
 *
 * Created on March 9, 2011, 7:19 PM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>


#include "resource_action.h"
#include "php_process.h"

#define REQUEST_STR_MAX 100000
#define RET_STR_MAX 1000000
#define MAX_GET_STR 1000
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"


char * current_dir;
int current_port;
char server_name[200];


#endif	/* RESOURCE_ACTION_H */

