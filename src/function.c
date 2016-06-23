/***************************************************************************
    begin                : Mon Jun 21 2004
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
#include <stdlib.h>

#include "main.h"

int s_function_init (s_function_t **f)
{
	(*f) = (s_function_t *) calloc(1, sizeof(s_function_t));
	if (*f == NULL) {
		return -1;
	}

	(*f)->file = NULL;
	(*f)->name = NULL;
	(*f)->param =  NULL;
	(*f)->ret = NULL;
	(*f)->calls = NULL;

	return 0;
}

int s_function_uninit (s_function_t *f)
{
	free(f->file);
	free(f->name);
	free(f->param);
	free(f->ret);
	s_functions_uninit(f->calls);
	free(f);

	return 0;
}

int s_functions_init (s_list_t **fs)
{
	int r;

	r = s_list_init(fs);

	return r;
}

int s_functions_uninit (s_list_t *fs)
{
	s_function_t *f;

	while (!s_list_eol(fs, 0)) {
		f = (s_function_t *) s_list_get(fs, 0);
		s_list_remove(fs, 0);
		s_function_uninit(f);
	}
	free(fs);

	return 0;
}

int s_functions_print (s_dir_t *dir)
{
        int i;
        int j;
        s_dir_t *d;
        s_file_t *f;
        s_function_t *func;
	
        i = 0;
        while (!s_list_eol(dir->dirs, i)) {
		d = (s_dir_t *) s_list_get(dir->dirs, i);
		s_functions_print(d);
		i++;
	}

        i = 0;
        while (!s_list_eol(dir->files, i)) {
		f = (s_file_t *) s_list_get(dir->files, i);
		j = 0;
		while (!s_list_eol(f->functions, j)) {
			func = (s_function_t *) s_list_get(f->functions, j++);
			printf("%s %s %s;\n", func->ret, func->name, func->param);
		}
		i++;
	}

	return 0;
}

int s_functions_gprint (s_dir_t *dir)
{
        int i;
        int j;
        int l;
        s_dir_t *d;
        s_file_t *f;
        s_function_t *func;
        s_function_t *call;
	
        i = 0;
        while (!s_list_eol(dir->dirs, i)) {
		d = (s_dir_t *) s_list_get(dir->dirs, i);
		s_functions_gprint(d);
		i++;
	}

        i = 0;
        while (!s_list_eol(dir->files, i)) {
		f = (s_file_t *) s_list_get(dir->files, i);
		j = 0;
		while (!s_list_eol(f->functions, j)) {
			func = (s_function_t *) s_list_get(f->functions, j++);
			l = 0;
			while (!s_list_eol(func->calls, l)) {
				call = (s_function_t *) s_list_get(func->calls, l++);
				printf("\t\"%s\" -> \"%s\";\n", func->name, call->name);
			}
		}
		i++;
	}

	return 0;
}

int s_alper (void)
{
        return 0;
}

int s_function_print (s_dir_t *dir)
{
	s_functions_print(dir);
	return (s_alper());
}

int s_function_gprint (s_dir_t *dir)
{
        printf("digraph G {\n"
               "\tnode [color=\"0.650 0.200 1.000\" style=filled];\n");
	s_functions_gprint(dir);
	printf("}\n");
	return 0;
}
