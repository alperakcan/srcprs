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

typedef struct s_list_node_s {
        void *next;
        void *element;
} s_list_node_t;

typedef struct s_list_s {
        int nb_elt;
        s_list_node_t *node;
} s_list_t;

typedef struct s_function_s {
	char *ret;
	char *name;
	char *param;
	char *file;
	s_list_t *calls;
} s_function_t;

typedef struct s_file_s {
	char *name;
	char *base;
	char *path;
	s_list_t *functions;
} s_file_t;

typedef struct s_dir_s {
	char *name;
	char *base;
	char *path;

	s_list_t *dirs;
	s_list_t *files;

	struct s_dir_s *up;
} s_dir_t;

typedef struct s_sourceparser_s {
	s_dir_t *dirarray;
} s_sourceparser_t;

/* dir.c */
int s_dir_init (s_dir_t **dir);
int s_dir_uninit (s_dir_t *dir);
int s_dirs_init (s_list_t **dirs);
int s_dirs_uninit (s_list_t *dirs);

int s_file_init (s_file_t **file);
int s_file_uninit (s_file_t *file);
int s_files_init (s_list_t **files);
int s_files_uninit (s_list_t *files);

int s_dir_walk (char *dpath, s_dir_t *dir);
int s_dir_file (char *path, s_dir_t *dir);
int s_dir_path2dir_name (char *path, char **base, char **name);
int s_dir_get (char *path, s_dir_t **dir);
int s_dir_sort (s_dir_t *dir);
int s_dir_get_level (s_dir_t *dir);
int s_dir_find_pos (s_dir_t *dir);
int s_dir_has_trailer (s_dir_t *dir, int level);
int s_dir_print (s_dir_t *dir);

/* function.c */
int s_function_init (s_function_t **f);
int s_function_uninit (s_function_t *f);
int s_functions_init (s_list_t **fs);
int s_functions_uninit (s_list_t *fs);
int s_function_print (s_dir_t *dir);
int s_function_gprint (s_dir_t *dir);

/* list.c */
int s_list_init (s_list_t **li);
int s_list_uninit (s_list_t *li);
int s_list_eol (const s_list_t *li, int i);
void * s_list_get (const s_list_t *li, int pos);
int s_list_remove (s_list_t *li, int pos);
int s_list_add (s_list_t *li, void *el, int pos);
void * s_list_find (s_list_t *list, void *node, int (*cmp_func) (void *, void *));
int s_list_get_pos (s_list_t *list, void *node);

/* parse.c */
int s_parse (s_file_t *file);
int s_parse_dir (s_dir_t *dir);

/* parse_c.c */
int s_parser_c (s_file_t *file);
