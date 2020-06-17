/*
 * Copyright (C) 2020 Esteban López Rodríguez <gnu_stallman@protonmail.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <arguments_parser.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Initialize arguments_t with default values
 */
static arguments_t*
init_arguments()
{
	arguments_t *args = malloc(sizeof(arguments_t));

	args->height = -1;
	args->width = -1;
	args->use_terminal_dimensions = 0;
	args->permill_obstacles = -1;

	return (args);
}

/*
 * Displays help menu
 */
static void
display_help(char *executable)
{
	const int OPT_WIDTH = 32;

	printf("Usage: %s [-t | -H HEIGHT -W WIDTH] [-h]\n", executable);
	puts("\nSnake Curses game");
	puts("\nOptions:");
	printf("\t%-*sMap dimensions following terminal size\n", OPT_WIDTH,
			"-t, --use-terminal-dimensions");
	printf("\t%-*sSet height of the map\n", OPT_WIDTH, "-H, --height <height>");
	printf("\t%-*sSet width of the map\n", OPT_WIDTH, "-W, --width <width>");
	printf("\t%-*sSet permill of obstacles in the map\n", OPT_WIDTH,
			"-o, --obstacles <permill>");
	printf("\t%-*sDisplay this help\n", OPT_WIDTH, "-h, --help");
}

arguments_t*
parse_arguments(int argc, char *argv[])
{
	arguments_t *args = init_arguments();
	int op;

	struct option long_options[] = {
		{"use-terminal-dimensions", no_argument, NULL, 't'},
		{"height", required_argument, NULL, 'H'},
		{"width", required_argument, NULL, 'W'},
		{"obstacles", required_argument, NULL, 'o'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};
	while ((op = getopt_long(argc, argv, ":tH:W:o:h", long_options, NULL)) != -1)
	{
		switch (op)
		{
			case 't':
				args->use_terminal_dimensions = 1;
				break;
			case 'H':
				args->height = atoi(optarg);
				break;
			case 'W':
				args->width = atoi(optarg);
				break;
			case 'o':
				args->permill_obstacles = atoi(optarg);
				break;
			case 'h':
				display_help(argv[0]);
				delete_arguments(args);
				exit(0);
				break;
			case ':':
				fputs("Missing required argument\n", stderr);
				delete_arguments(args);
				exit(1);
				break;
			case '?':
				fputs("Unknown argument\n", stderr);
				delete_arguments(args);
				exit(1);
				break;
		}
	}

	if (args->use_terminal_dimensions &&
			(args->height != -1 || args->width != -1))
	{
		fputs("--use-terminal-dimensions incompatible with ", stderr);
		fputs("--with and --height arguments\n", stderr);
		delete_arguments(args);
		exit(1);
	}

	return (args);
}

void
delete_arguments(arguments_t *args)
{
	free(args);
}
