/* 
 * File:   php_process.h
 * Author: maosuhan
 *
 * Created on March 9, 2011, 7:23 PM
 */

#ifndef PHP_PROCESS_H
#define	PHP_PROCESS_H

int is_php_file(char *file);
void process_php_file(char *file, struct stat *sb);

#endif	/* PHP_PROCESS_H */

