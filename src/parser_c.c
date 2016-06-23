/***************************************************************************
    begin                : Thu Jun 17 2004
    copyright            : (C) 2004 - 2005 by Alper Akcan
    email                : distchx@yahoo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "main.h"

int s_parser_c_get_valid_name (char *buf, char **name)
{
        int l;
        int i = 0;
	char *str;
	char *tmp;
	char *nam;

        str = buf;
        l = strlen(str);

	/* [a-zA-Z_]+[a-zA-Z_0-9]* */
	if ((*str == '_') ||
	    ((*str >= 'a') && (*str <= 'z')) ||
	    ((*str >= 'A') && (*str <= 'Z'))) {
		/* [a-zA-Z_]+ */
		tmp = str;
		do {
			/* [a-zA-Z_0-9]* */
			if (++i >= l) {
				goto err;
			}
			tmp++;
		} while ((*tmp == '_') ||
		         ((*tmp >= 'a') && (*tmp <= 'z')) ||
		         ((*tmp >= 'A') && (*tmp <= 'Z')) ||
		         ((*tmp >= '0') && (*tmp <= '9')));

                l = tmp - str;

		(*name) = (char *) calloc(sizeof(char), l + 1);
		if (*name == NULL) {
			printf("%s : %d\n", __FILE__, __LINE__);
			exit(0);
		}

		nam = *name;
		while (l--) {
			*nam++ = *str++;
		}
		*nam = '\0';
		
		return 0;
	}

err:	return -1;
}

int s_parser_c_get_valid_names (char *buf, s_list_t *li)
{
	int i;
	int l;
	char *str;
	char *name;

	str = buf;
	l = strlen(str);

        i = 0;
        while (i < l) {
		while (isspace(*str)) {
			if (i++ >= l) {
				goto err;
			}
			str++;
		}
		if (s_parser_c_get_valid_name(str, &name)) {
			return i;
			i++;
			str++;
		} else {
			i += strlen(name);
			str += strlen(name);
			s_list_add(li, name, -1);
		}
	}

err:	return -1;
}

int s_parser_c_get_parameter_list (char *buf, char **plist, char c)
{
        int i;
	int l;
	int p;
	int s;
	char *pl;
	char *str;
	char *tmp;

	str = buf;
	l = strlen(str);

	if (*str == '(') {
		i = 1;
		p = 1;
		tmp = str++;
		while (1) {
			if (i >= l) {
				goto err;
			}
			switch (*str) {
				case '(': p++; break;
				case ')': p--; break;
			}
			if (p == 0) {
				l = str - tmp + 1;
				while (isspace(*++str)) {
				}
				if (*str != c) {
					goto err;
				}
				(*plist) = (char *) calloc(sizeof(char), l + 1);
				if (*plist == NULL) {
					printf("%s : %d\n", __FILE__, __LINE__);
					exit(0);
				}
				pl = *plist;
				s = 0;
				while (l--) {
					if (isspace(*tmp)) {
						if (s == 0) {
							*pl++ = *tmp;
						}
						s = 1;
					} else {
						s = 0;
						*pl++ = *tmp;
					}
					tmp++;
				}
				*pl = '\0';
				return 0;
			}
			i++;
			str++;
		}
	}

err: return -1;
}

int s_parser_c_get_valid_functions (char *buf, s_file_t *file)
{
	int i;
	int j;
	int l;
	int p;
	char *str;
	char *ptmp;
	char *plist;
	char *fsign;
	s_list_t *li;
	s_function_t *fs;

        fs = NULL;
	str = buf;
	fsign = NULL;
	l = strlen(str);

        if (s_list_init(&li)) {
		printf("%s : %d\n", __FILE__, __LINE__);
		exit(0);
	}
	
	/* function signature :
	   	valid_name[s] [\*]* valid_name '(' [.] ')' '{'
	   function call :
	   	valid_name '(' [.] ')' ';'
	 */

	i = 0;
	while (i < l) {
		j = s_parser_c_get_valid_names(str, li);
		if (j <= 0) {
			j = 1;
		}
		i += j;
		str += j;

                if (li->nb_elt) {
			/* found valid_name[s] */
			p = 0;
			while (isspace(*str) ||
			       (*str == '*')) {
				/* look for pointer */
				if (*str == '*') {
					p++;
				}
				i++;
				str++;
			}
			if (p) {
				/* valid_name[s] [\*]+ valid_f_name*/
                                char *nam;
				char *name = (char *) calloc(sizeof(char), p + 1);

				nam = name;
				while (p--) {
					*nam++ = '*';
				}
				*nam = '\0';

				s_list_add(li, name, -1);

				/* look for a valid function name */
				while (isspace(*str)) {
					if (i++ >= l) {
						goto err;
					}
					str++;
				}
				if (!s_parser_c_get_valid_name(str, &nam)) {
					/* found valid function name */
					i += strlen(nam);
					str += strlen(nam);
					s_list_add(li, nam, -1);
				}
			}

			/* valid_name[s] valid_f_name */

                        {
				/* if, for, while, switch */
				char *ifws;
				ifws = (char *) s_list_get(li, li->nb_elt - 1);

				if ((strcmp(ifws, "if") == 0) ||
				    (strcmp(ifws, "for") == 0) ||
				    (strcmp(ifws, "while") == 0) ||
				    (strcmp(ifws, "return") == 0) ||
				    (strcmp(ifws, "switch") == 0)) {
					goto listclear;
				}
			}
                        
			/* look for parameter list
			   '(' [.] ')'
			 */
			while (isspace(*str)) {
				if (i++ >= l) {
					goto err;
				}
				str++;
			}
			ptmp = NULL;
			if (!s_parser_c_get_parameter_list(str, &ptmp, '{')) {
				/* found parameter list
				   function signature
				 */
				plist = strdup(ptmp);
				i += strlen(plist);
				str += strlen(plist);
				if (fsign != NULL) {
					free(fsign);
					fsign = NULL;
				}
				fsign = strdup((char *) s_list_get(li, li->nb_elt - 1));
				s_list_add(li, plist, -1);

				if (s_function_init(&fs)) {
					printf("%s : %d\n", __FILE__, __LINE__);
					exit(0);
				}
				fs->param = strdup((char *) s_list_get(li, li->nb_elt - 1));
				fs->name = strdup((char *) s_list_get(li, li->nb_elt - 2));

				j = 0;
				p = 0;
				while (p < li->nb_elt - 2) {
					char *name = (char *) s_list_get(li, p++);
					j += strlen(name);
				}
				fs->ret = (char *) calloc(sizeof(char *), j + li->nb_elt - 1);
				*fs->ret = '\0';

				p = 0;
				while (p < li->nb_elt - 2) {
					char *name = (char *) s_list_get(li, p++);
					strcat(fs->ret, name);
					if (p != li->nb_elt - 2)
					strcat(fs->ret, " ");
				}
				if (s_functions_init(&fs->calls)) {
					printf("%s : %d\n", __FILE__, __LINE__);
					exit(0);
				}
				s_list_add(file->functions, fs, -1);
			} else if ((!s_parser_c_get_parameter_list(str, &ptmp, ';') ||
			            !s_parser_c_get_parameter_list(str, &ptmp, ')'))) {
//			           (li->nb_elt >= 1)) {
				/* found parameter list
				   function call
				 */
				plist = strdup(ptmp);
				i += strlen(plist);
				str += strlen(plist);
				s_list_add(li, plist, -1);

				if (fs != NULL) {
					s_function_t *fsc;
					if (s_function_init(&fsc)) {
						printf("%s : %d\n", __FILE__, __LINE__);
						exit(0);
					}
					fsc->name = strdup((char *) s_list_get(li, li->nb_elt - 2));

					{
						int count = 0;
						s_function_t *call;
						while (!s_list_eol(fs->calls, count)) {
							call = (s_function_t *) s_list_get(fs->calls, count++);
							if (strcmp(call->name, fsc->name) == 0) {
								s_function_uninit(fsc);
								goto donotadd;
							}
						}
					}

//					if (strncmp(fsc->name, "s_", 2) != 0) {
//						s_function_uninit(fsc);
//					} else {
						fsc->param = strdup((char *) s_list_get(li, li->nb_elt - 1));
						s_list_add(fs->calls, fsc, -1);
//					}
				}
			}
donotadd:		free(ptmp);

listclear:		p = 0;
			while (!s_list_eol(li, p)) {
				char *name = (char *) s_list_get(li, p);
				s_list_remove(li, p);
				free(name);
			}
		}
	}

        free(fsign);
        
	s_list_uninit(li);

err:	return 0;
}

int s_parser_c_clear_comments (char *buf)
{
        int i;
        int l;

        l = strlen(buf);

	for (i = 0; i < l; i++) {
		if (strncmp(buf + i, "/*", 2) == 0) {
			buf[i++] = ' ';
			buf[i++] = ' ';
			while (strncmp(buf + i, "*/", 2) != 0) {
				i++;
				if (i >= l) {
					return 0;
				}
			}
		}
	}

	l = strlen(buf);
	for (i = 0; i < l; i++) {
		if (buf[i] == '/' &&
		    buf[i + 1] == '/') {
			while (buf[i] && buf[i] != '\n') {
				buf[i++] = ' ';
			}
		}
	}

	return 0;
}

int s_parser_c (s_file_t *file)
{
	int fd;
	char *buf;
	struct stat stbuf;

	if (stat(file->path, &stbuf)) {
		goto err;
	}

	fd = open(file->path, O_RDONLY);
	if (fd < 0) {
		goto err;
	}

	buf = (char *) malloc(stbuf.st_size);
	buf[read(fd, buf, stbuf.st_size) - 1] = '\0';

//	printf("%s\n", file->path);

	s_parser_c_clear_comments(buf);

	if (s_functions_init(&file->functions)) {
		printf("%s : %d\n", __FILE__, __LINE__);
		exit(0);
	}
	
	s_parser_c_get_valid_functions(buf, file);

        free(buf);
	close(fd);

	return 0;
err:	return -1;
}
