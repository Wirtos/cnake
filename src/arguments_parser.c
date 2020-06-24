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
#include <config.h>
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
	args->starting_delay = -1;
	args->minimum_delay = -1;
	args->step_delay = -1;
	args->two_players = 0;
	args->duration_shortener = -1;
	args->duration_decelerator = -1;
	args->duration_extra_points = -1;
	args->probability_decelerator = -1;
	args->probability_shortener = -1;
	args->probability_extra_points = -1;
	args->score_step_map_change = -1;
	args->disable_map_change = 0;

	return (args);
}

/*
 * Displays help menu
 */
static void
display_help(char *executable)
{
	const int OPT_WIDTH = 39;

	printf("Usage: %s [OPTIONS]\n", executable);
	puts("\nSnake Curses game");
	puts("\nPlayers:");
	printf("\t%-*sEnable two players mode\n", OPT_WIDTH, "-2, --two-players");
	puts("\nSize:");
	printf("\t%-*sMap dimensions following terminal size\n", OPT_WIDTH,
			"-t, --use-terminal-dimensions");
	puts("OR:");
	printf("\t%-*sSet height of the map (Def: %d)\n", OPT_WIDTH,
			"-H, --height <height>", DEFAULT_W_GAME_HEIGHT);
	printf("\t%-*sSet width of the map (Def: %d)\n", OPT_WIDTH,
			"-W, --width <width>", DEFAULT_W_GAME_WIDTH);
	puts("\nObstacles:");
	printf("\t%-*sSet permill of obstacles in the map (Def: %d)\n", OPT_WIDTH,
			"-o, --obstacles <permill>", DEFAULT_PERMILL_OBSTACLES);
	puts("\nDelay:");
	printf("\t%-*sSet starting delay in milliseconds (Def: %d)\n", OPT_WIDTH,
			"-s, --starting-delay <ms>", DEFAULT_STARTING_DELAY);
	printf("\t%-*sSet minumum delay in milliseconds (Def: %d)\n", OPT_WIDTH,
			"-m, --minimum-delay <ms>", DEFAULT_MINIMUM_DELAY);
	printf("\t%-*sSet reduction of delay in milliseconds when eating food (Def: %d)\n",
			OPT_WIDTH, "-S, --step-delay <ms>", DEFAULT_STEP_DELAY);
	puts("\nTemporal items duration:");
	printf("\t%-*sSet duration of decelerators in seconds (Def: %d)\n",
			OPT_WIDTH, "-d, --duration-decelerator <s>", DEFAULT_DURATION_DECELERATOR);
	printf("\t%-*sSet duration of shorteners in seconds (Def: %d)\n",
			OPT_WIDTH, "-D, --duration-shortener <s>", DEFAULT_DURATION_SHORTENER);
	printf("\t%-*sSet duration of extra points in seconds (Def: %d)\n",
			OPT_WIDTH, "-e, --duration-extra-points <s>", DEFAULT_DURATION_EXTRA_POINTS);
	puts("\nProbability of items (1/X chances of appearing when eating a food):");
	printf("\t%-*sSet probability of decelerators (Def: %d)\n",
			OPT_WIDTH, "-p, --probability-decelerator <prob>",
			DEFAULT_PROBABILITY_DECELERATOR);
	printf("\t%-*sSet probability of shorteners (Def: %d)\n",
			OPT_WIDTH, "-P, --probability-shortener <prob>",
			DEFAULT_PROBABILITY_SHORTENER);
	printf("\t%-*sSet probability of extra points (Def: %d)\n",
			OPT_WIDTH, "-E, --probability-extra-points <prob>",
			DEFAULT_PROBABILITY_EXTRA_POINTS);
	puts("\nMap change:");
	printf("\t%-*sSet the step of score between map changes (Def: %d)\n", OPT_WIDTH,
			"-c, --score-step-map-change <score>", DEFAULT_SCORE_STEP_MAP_CHANGE);
	printf("\t%-*sDisable map changing\n", OPT_WIDTH,	"-C, --disable-map-change");
	printf("\n\t%-*sDisplay this help\n", OPT_WIDTH, "-h, --help");
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
		{"starting-delay", required_argument, NULL, 's'},
		{"minimum-delay", required_argument, NULL, 'm'},
		{"step-delay", required_argument, NULL, 'S'},
		{"two-players", no_argument, NULL, '2'},
		{"duration-decelerator", required_argument, NULL, 'd'},
		{"duration-shortener", required_argument, NULL, 'D'},
		{"duration-extra-points", required_argument, NULL, 'e'},
		{"probability-decelerator", required_argument, NULL, 'p'},
		{"probability-shortener", required_argument, NULL, 'P'},
		{"probability-extra-points", required_argument, NULL, 'E'},
		{"score-step-map-change", required_argument, NULL, 'c'},
		{"disable-map-change", no_argument, NULL, 'C'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};
	while ((op = getopt_long(argc, argv, ":tH:W:o:s:m:S:2d:D:e:p:P:E:c:Ch",
					long_options, NULL)) != -1)
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
			case 's':
				args->starting_delay = atoi(optarg);
				break;
			case 'm':
				args->minimum_delay = atoi(optarg);
				break;
			case 'S':
				args->step_delay = atoi(optarg);
				break;
			case '2':
				args->two_players = 1;
				break;
			case 'D':
				args->duration_shortener = atoi(optarg);
				break;
			case 'd':
				args->duration_decelerator = atoi(optarg);
				break;
			case 'e':
				args->duration_extra_points = atoi(optarg);
				break;
			case 'p':
				args->probability_decelerator = atoi(optarg);
				break;
			case 'P':
				args->probability_shortener = atoi(optarg);
				break;
			case 'E':
				args->probability_extra_points = atoi(optarg);
				break;
			case 'c':
				args->score_step_map_change = atoi(optarg);
				break;
			case 'C':
				args->disable_map_change = 1;
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
		fputs("--width and --height arguments\n", stderr);
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
