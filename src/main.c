/***************************************************************************
    begin                : Tue Jun 15 2004
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
#include <string.h>
#include <unistd.h>

#include "main.h"

int main (int argc, char *argv[])
{
	char c;
	int dprint = 0;
	int fprint = 0;
	int gprint = 0;
        char *path = NULL;
	s_sourceparser_t *srcprs;

	if (argc == 1) {
		goto help;
	}
		
	while ((c = getopt(argc, argv, "t:dfgh?")) != -1) {
		switch (c) {
			case 't':
				path = strdup(optarg);
				break;
			case 'd':
				dprint = 1;
				break;
			case 'f':
				fprint = 1;
				break;
			case 'g':
				gprint = 1;
				break;
			case '?':
			case 'h':
help:				printf("Usage :\n"
				       "\t%s [-t directory] [-d] [-g] [-f] [-h]\n"
				       "\t\t-t : parse given directory\n"
				       "\t\t-d : print directory tree\n"
				       "\t\t-g : print graphviz file\n"
				       "\t\t-f : print found functions\n"
				       "\t\t-h : this text\n", argv[0]);
				exit(0);
		}
	}

	srcprs = (s_sourceparser_t *) malloc(sizeof(s_sourceparser_t));

	if (path == NULL) {
		path = strdup(".");
	}

        if (s_dir_get(path, &srcprs->dirarray)) {
		exit(0);
	}
	free(path);

	if (dprint && s_dir_print(srcprs->dirarray)) {
		exit(0);
	}

        if (s_parse_dir(srcprs->dirarray)) {
		exit(0);
	}

	if (fprint && s_function_print(srcprs->dirarray)) {
		exit(0);
	}

	if (gprint && s_function_gprint(srcprs->dirarray)) {
		exit(0);
	}

	s_dir_uninit(srcprs->dirarray);

	free(srcprs);

	return 0;
}
