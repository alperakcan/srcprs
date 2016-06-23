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

#include <fnmatch.h>

#include "main.h"

int s_parse (s_file_t *file)
{
        int r;
        
        if (fnmatch("*.c", file->path, FNM_CASEFOLD)) {
		return -1;
	}

        r = s_parser_c(file);

        return r;
}

int s_parse_dir (s_dir_t *dir)
{
	int p;
	s_dir_t *d;
	s_file_t *f;

	p = 0;
	while(!s_list_eol(dir->dirs, p)) {
		d = (s_dir_t *) s_list_get(dir->dirs, p);
		s_parse_dir(d);
		p++;
	}

        p = 0;
	while (!s_list_eol(dir->files, p)) {
		f = (s_file_t *) s_list_get(dir->files, p);
		s_parse(f);
		p++;
	}
	
	return 0;
}
