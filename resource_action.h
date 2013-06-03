/* 
 * File:   resource_action.h
 * Author: maosuhan
 *
 * Created on March 9, 2011, 7:25 PM
 */

#ifndef RESOURCE_ACTION_H
#define	RESOURCE_ACTION_H

char* get_mime_type(char* name);
void file_details(char* dir, char* name);
void do_file(char *file, struct stat *sb);
void send_error(int status, char* title, char* extra_header, char* text);
void send_headers(int status, char* title, char* extra_header, char* mime_type, off_t length, time_t mod);
void response_resource(char *resource);
void send_dir_list(char *file, struct stat *sb);



#endif	/* RESOURCE_ACTION_H */

