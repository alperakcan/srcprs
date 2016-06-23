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
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "main.h"

int s_dir_init (s_dir_t **dir)
{
	*dir = (s_dir_t *) calloc(1, sizeof(s_dir_t));
	if (*dir == NULL) {
		goto err1;
	}

	(*dir)->base = NULL;
	(*dir)->name = NULL;
	(*dir)->path = NULL;
	(*dir)->up = NULL;

	if (s_dirs_init(&((*dir)->dirs))) {
		goto err2;
	}
	if (s_files_init(&((*dir)->files))) {
		goto err3;
	}

	return 0;

err3:	s_dirs_uninit((*dir)->dirs);
err2:	free(*dir);
err1:	return -1;
}

int s_dir_uninit (s_dir_t *dir)
{
        free(dir->base);
	free(dir->name);
	free(dir->path);
	dir->up = NULL;
	s_dirs_uninit(dir->dirs);
	s_files_uninit(dir->files);
	free(dir);

	return 0;
}

int s_dirs_init (s_list_t **dirs)
{
	int r;
	r = s_list_init(dirs);
	return r;
}

int s_dirs_uninit (s_list_t *dirs)
{
	s_dir_t *dir;

	while (!s_list_eol(dirs, 0)) {
		dir = (s_dir_t *) s_list_get(dirs, 0);
		s_list_remove(dirs, 0);
		s_dir_uninit(dir);
	}
	free(dirs);

	return 0;
}

int s_file_init (s_file_t **file)
{
	*file = (s_file_t *) calloc(1, sizeof(s_file_t));
	if (*file == NULL) {
		goto err1;
	}

        (*file)->base = NULL;
	(*file)->name = NULL;
	(*file)->path = NULL;
	(*file)->functions = NULL;

	return 0;

err1:	return -1;
}

int s_file_uninit (s_file_t *file)
{
        free(file->base);
	free(file->name);
	free(file->path);
	s_functions_uninit(file->functions);
	free(file);

	return 0;
}

int s_files_init (s_list_t **files)
{
	int r;
	r = s_list_init(files);
	return r;
}

int s_files_uninit (s_list_t *files)
{
	s_file_t *file;

	while (!s_list_eol(files, 0)) {
		file = (s_file_t *) s_list_get(files, 0);
		s_list_remove(files, 0);
		s_file_uninit(file);
	}
	free(files);

	return 0;
}

int s_dir_walk (char *dpath, s_dir_t *dir)
{
        DIR *dfd;
        char *path;
        struct dirent *dp;

        if ((dfd = opendir(dpath)) ==  NULL) {
		printf("Cannot open %s\n", dpath);
		goto err;
	}

	while ((dp = readdir(dfd)) != NULL) {
		if (strcmp(dp->d_name, ".") == 0 ||
		    strcmp(dp->d_name, "..") == 0) {
			continue;
		}
		path = (char *) calloc(sizeof(char), strlen(dpath) + strlen(dp->d_name) + 2);
		sprintf(path, "%s/%s", dpath, dp->d_name);
		s_dir_file(path, dir);
		free(path);
	}

	closedir(dfd);
	
	return 0;

err:	return -1;
}

int s_dir_path2dir_name (char *path, char **base, char **name)
{
	char *str;

	str = strrchr(path, '/');
	if (str == NULL) {
		(*name) = strdup(path);
	} else {
		(*name) = strdup(str + 1);
	}

	(*base) = strdup(path);
	str = strrchr(*base, '/');
	if (str == NULL) {
	} else {
		*str = '\0';
	}

	return 0;
}

int s_dir_file (char *path, s_dir_t *dir)
{
        s_dir_t *s_dir;
        s_file_t *s_file;
	struct stat stbuf;

	if (stat(path, &stbuf)) {
		printf("Cannot access %s\n", path);
		goto err;
	}
	if (S_ISDIR(stbuf.st_mode & S_IFMT)) {
		s_dir_init(&s_dir);
		s_dir->path = strdup(path);
		s_dir_path2dir_name(path, &s_dir->base, &s_dir->name);
		s_dir->up = dir;
		s_list_add(dir->dirs, s_dir, -1);
		s_dir_walk(path, s_dir);
	}
	if (S_ISREG(stbuf.st_mode & S_IFMT)) {
		s_file_init(&s_file);
		s_file->path = strdup(path);
		s_dir_path2dir_name(path, &s_file->base, &s_file->name);
		s_list_add(dir->files, s_file, -1);
	}
	
	return 0;

err:	return -1;
}

int s_dir_get (char *path, s_dir_t **dir)
{
        int r;
        char *str;
        struct stat stbuf;

        while (1) {
		str = strrchr(path, '/');
		if ((str != NULL) && (strlen(str) ==  1)) {
			*str = '\0';
		} else {
			break;
		}
	}
	if (stat(path, &stbuf)) {
		printf("Cannot access %s\n", path);
		goto err;
	}
	if (S_ISDIR(stbuf.st_mode & S_IFMT)) {
		if (s_dir_init(dir)) {
			goto err;
		}
                (*dir)->path = strdup(path);
		s_dir_path2dir_name(path, &(*dir)->base, &(*dir)->name);
                s_dir_walk(path, *dir);
	}

	return 0;

	r = s_dir_sort(*dir);
	return r;

err:	return -1;
}

int s_dir_sort (s_dir_t *dir)
{
	int i;
	int j;

        s_dir_t *d;
	s_dir_t *di;
	s_dir_t *dj;

	s_file_t *fi;
	s_file_t *fj;

	for (i = 0; i < dir->dirs->nb_elt - 1; i++) {
		for (j = 0; j < dir->dirs->nb_elt - 1; j++) {
			di = (s_dir_t *) s_list_get(dir->dirs, j);
			dj = (s_dir_t *) s_list_get(dir->dirs, j + 1);
			if (strcmp(dj->path, di->path) < 0) {
				s_list_remove(dir->dirs, j);
				s_list_add(dir->dirs, di, j + 1);
			}
		}
	}

	for (i = 0; i < dir->files->nb_elt - 1; i++) {
		for (j = 0; j < dir->files->nb_elt - 1; j++) {
			fi = (s_file_t *) s_list_get(dir->files, j);
			fj = (s_file_t *) s_list_get(dir->files, j + 1);
			if (strcmp(fj->path, fi->path) < 0) {
				s_list_remove(dir->files, j);
				s_list_add(dir->files, fi, j + 1);
			}
		}
	}

	i = 0;
	while (!s_list_eol(dir->dirs, i)) {
		d = (s_dir_t *) s_list_get(dir->dirs, i);
		s_dir_sort(d);
		i++;
	}

	return 0;
}

int s_dir_get_level (s_dir_t *dir)
{
	int l = 0;
	s_dir_t *d;

	d = dir;

	while (d->up != NULL) {
		d = d->up;
		l++;
	}

	return l;
}

int s_dir_find_pos (s_dir_t *dir)
{
	int r;
	r = s_list_get_pos(dir->up->dirs, dir);
	return r;
}

int s_dir_has_trailer (s_dir_t *dir, int level)
{
        int i;
        int l;
        int p;
        int r;
	s_dir_t *d = dir;

	l = s_dir_get_level(dir);
	i = l - level;

	while (i > 1) {
		d = d->up;
		i--;
	}

	p = s_dir_find_pos(d);

        r = !s_list_eol(d->up->dirs, p + 1) || !s_list_eol(d->up->files, 0);
        return r;
}

int s_dir_print (s_dir_t *dir)
{
        int i;
        int j;
        int l;
        s_dir_t *d;
        s_file_t *f;
        
        l = s_dir_get_level(dir);
        if (l == 0) {
		printf("+ %s\n", dir->path);
	} else {
		for (i = 0; i < l - 1; i++) {
			if (s_dir_has_trailer(dir, i)) {
				printf("|   ");
			} else {
				printf("    ");
			}
		}
		printf("|---+ %s\n", dir->name);
	}
        
        i = 0;
        while (!s_list_eol(dir->dirs, i)) {
		d = (s_dir_t *) s_list_get(dir->dirs, i);
		s_dir_print(d);
		i++;
	}

        i = 0;
        while (!s_list_eol(dir->files, i)) {
		f = (s_file_t *) s_list_get(dir->files, i);
		for (j = 0; j < l; j++) {
			if (s_dir_has_trailer(dir, j)) {
				printf("|   ");
			} else {
				printf("    ");
			}
		}
		printf("|---- %s\n", f->name);
		i++;
	}

	if (dir->dirs->nb_elt || dir->files->nb_elt) {
		for (i = 0; i < l; i++) {
			if (s_dir_has_trailer(dir, i)) {
				printf("|   ");
			} else {
				printf("    ");
			}
		}
		printf("\n");
	}
	
	return 0;
}
