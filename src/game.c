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

#include <config.h>
#include <field.h>
#include <snake.h>
#include <arguments_parser.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Prepares colors
 */
static void
set_curses_properties()
{
	start_color();
	use_default_colors();

	/* Color pairs definitions */
	init_pair(PAIR_DEFAULT, -1, -1);
	init_pair(PAIR_SCORE, COLOR_YELLOW, -1);
	init_pair(PAIR_BORDER, COLOR_MAGENTA, -1);
	init_pair(PAIR_SNAKE, -1, COLOR_RED);
	init_pair(PAIR_HEAD, COLOR_BLUE, COLOR_GREEN);
	init_pair(PAIR_FOOD, COLOR_CYAN, -1);
	init_pair(PAIR_SHORTENER, COLOR_BLUE, -1);
	init_pair(PAIR_DECELERATOR, COLOR_GREEN, -1);
	init_pair(PAIR_TITLE, COLOR_GREEN, COLOR_RED);

	attrset(COLOR_PAIR(PAIR_DEFAULT));
}

/*
 * Updates score marker
 */
static void
redraw_score(WINDOW *w_score, unsigned int score)
{
	mvwaddstr(w_score, 0, 0, "Score: ");

	wclrtoeol(w_score);
	wattron(w_score, COLOR_PAIR(PAIR_SCORE));
	wprintw(w_score, "%u", score);
	wattroff(w_score, COLOR_PAIR(PAIR_SCORE));

	wnoutrefresh(w_score);
}

/*
 * Draws the keys window
 */
static void
draw_keys(WINDOW *w_keys)
{
	wborder(w_keys, 0, 0, 0, 0, 0, 0, 0, 0);

	wattron(w_keys, A_BOLD);
	mvwaddstr(w_keys, 1, WIDTH_W_KEYS/2 - 2, "Keys");
	wattroff(w_keys, A_BOLD);

	mvwaddstr(w_keys, 3, 2, "Up:    w, k, up arrow");
	mvwaddstr(w_keys, 4, 2, "Down:  s, j, down arrow");
	mvwaddstr(w_keys, 5, 2, "Left:  a, h, left arrow");
	mvwaddstr(w_keys, 6, 2, "Right: d, l, right arrow");
	mvwaddstr(w_keys, 8, 2, "Pause: p");
	mvwaddstr(w_keys, 9, 2, "Quit:  q");

	wnoutrefresh(w_keys);
}

/*
 * Updates game window acording to the field's map
 */
static void
redraw_game(WINDOW *w_game, field_t *field, direction_t direction)
{
	int i, j;

	werase(w_game);

	for (i = 0; i < field->height; i++)
	{
		for (j = 0; j < field->width; j++)
		{
			switch (field->matrix[i][j])
			{
				case EMPTY:
					break;
				case SNAKE:
					mvwaddch(w_game, i, j, '#' | COLOR_PAIR(PAIR_SNAKE));
					break;
				case HEAD:
					wattron(w_game, COLOR_PAIR(PAIR_HEAD));
					switch (direction)
					{
						case NORTH:
							mvwaddch(w_game, i, j, '^');
							break;
						case EAST:
							mvwaddch(w_game, i, j, '>');
							break;
						case WEST:
							mvwaddch(w_game, i, j, '<');
							break;
						case SOUTH:
							mvwaddch(w_game, i, j, 'v');
					}
					wattroff(w_game, COLOR_PAIR(PAIR_HEAD));
					break;
				case FOOD:
					mvwaddch(w_game, i, j, 'f' | COLOR_PAIR(PAIR_FOOD));
					break;
				case BORDER:
					mvwaddch(w_game, i, j, '*' | COLOR_PAIR(PAIR_BORDER));
					break;
				case OBSTACLE:
					mvwaddch(w_game, i, j, 'x' | COLOR_PAIR(PAIR_BORDER));
					break;
				case SHORTENER:
					mvwaddch(w_game, i, j, 's' | COLOR_PAIR(PAIR_SHORTENER));
					break;
				case DECELERATOR:
					mvwaddch(w_game, i, j, 'd' | COLOR_PAIR(PAIR_DECELERATOR));
					break;
			}
		}
	}

	wnoutrefresh(w_game);
}

/*
 * Pause game and display PAUSED banner in w_game
 */
static void
pause(WINDOW *w_game, time_t delay)
{
	int max_y, max_x;

	getmaxyx(w_game, max_y, max_x);
	wattron(w_game, A_REVERSE);
	mvwaddstr(w_game, max_y / 2, max_x / 2 - 3, "PAUSED");
	wattroff(w_game, A_REVERSE);
	wrefresh(w_game);

	timeout(-1);  /* Take out timeout */
	getch();
	timeout(delay);  /* Restore timeout */
}

/*
 * Initialize data structures and run game mainloop
 */
static void
start(arguments_t *args)
{
	WINDOW *w_score, *w_game, *w_keys;
	field_t *field;
	snake_t *snake;
	unsigned int w_game_y, score, keep_mainloop;
	time_t delay;

	set_curses_properties();

	/* Title */
	attron(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
	mvaddstr(1, COLS/2 - 4, "S N A K E");
	attroff(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
	wnoutrefresh(stdscr);

	w_game_y = (LINES+3)/2 - args->height/2;  /* Starting line of w_game */
	w_score = newwin(1, COLS - WIDTH_W_KEYS - 4, w_game_y - 1, 1);
	w_game = newwin(args->height, args->width, w_game_y, 1);
	w_keys = newwin(11, WIDTH_W_KEYS, LINES/2 - 5, COLS - WIDTH_W_KEYS - 1);

	field = init_field(args->height, args->width, args->permill_obstacles);
	snake = init_snake(field);
	add_food(field);

	draw_keys(w_keys);

	/* Mainloop */
	score = 0;
	keep_mainloop = 1;
	delay = args->starting_delay;
	timeout(delay);
	while (keep_mainloop)
	{
		remove_expired_items(field);
		redraw_score(w_score, score);
		redraw_game(w_game, field, snake->direction);
		doupdate();

		/* Get user input */
		switch (getch())
		{
			case 'w':
			case 'k':
			case KEY_UP:
				snake->direction = NORTH;
				break;
			case 'a':
			case 'h':
			case KEY_LEFT:
				snake->direction = WEST;
				break;
			case 's':
			case 'j':
			case KEY_DOWN:
				snake->direction = SOUTH;
				break;
			case 'd':
			case 'l':
			case KEY_RIGHT:
				snake->direction = EAST;
				break;
			case 'p':
				pause(w_game, delay);
				break;
			case 'q':
				keep_mainloop = 0;
		}

		/* Move the snake */
		switch (advance(field, snake))
		{
			case EMPTY:
				break;
			case SNAKE:
			case HEAD:
			case BORDER:
			case OBSTACLE:
				keep_mainloop = 0;
				break;
			case FOOD:
				add_food(field);
				score += POINTS_FOOD;

				/* Delay reduction */
				if (delay > args->minimum_delay)
					delay -= args->step_delay;
				else
					delay = args->minimum_delay;
				timeout(delay);

				/* Items generation */
				if (rand() % PROBABILITY_SHORTENER == 0)
					add_temp_item(field, SHORTENER, DURATION_SHORTENER);
				if (rand() % PROBABILITY_DECELERATOR == 0)
					add_temp_item(field, DECELERATOR, DURATION_DECELERATOR);
				break;
			case SHORTENER:
				score += POINTS_SHORTENER;
				break;
			case DECELERATOR:
				score += POINTS_DECELERATOR;
				delay = args->starting_delay;
				timeout(delay);
				break;
		}
	}

	/* Free the memory */
	delete_snake(snake);
	delete_field(field);
	delwin(w_score);
	delwin(w_game);
	delwin(w_keys);
	endwin();

	printf("Your score: %u\n", score);
}

/*
 * Set default values in unspecified options. Also checks terminal size.
 * NEEDS INITIALIZED NCURSES
 */
static void
set_default_options(arguments_t *args)
{
	/* Size settings */
	if (args->use_terminal_dimensions)
	{
		args->height = LINES - 4;
		args->width = COLS - WIDTH_W_KEYS - 3;
	}
	else
	{
		if (args->height == -1)
			args->height = DEFAULT_W_GAME_HEIGHT;
		if (args->width == -1)
			args->width = DEFAULT_W_GAME_WIDTH;
	}

	/* Check terminal size */
	if (args->height + 3 > LINES)
	{
		endwin();
		delete_arguments(args);
		fputs("Terminal height too small\n", stderr);
		exit(1);
	}
	if (args->width + WIDTH_W_KEYS + 3 > COLS)
	{
		endwin();
		delete_arguments(args);
		fputs("Terminal width too small\n", stderr);
		exit(1);
	}

	/* Obstacles settings */
	if (args->permill_obstacles == -1)
		args->permill_obstacles = DEFAULT_PERMILL_OBSTACLES;

	/* Delay settings */
	if (args->starting_delay == -1)
		args->starting_delay = DEFAULT_STARTING_DELAY;

	if (args->minimum_delay == -1)
		args->minimum_delay = DEFAULT_MINIMUM_DELAY;

	if (args->step_delay == -1)
		args->step_delay = DEFAULT_STEP_DELAY;
}

int
main(int argc, char *argv[])
{
	arguments_t *args = parse_arguments(argc, argv);

	srand(time(NULL));
	initscr();

	cbreak();             /* Do not buffer keypresses */
	noecho();             /* Do not show keypresses */
	keypad(stdscr, TRUE); /* Enable special keys */
	curs_set(0);          /* Hide cursor */

	set_default_options(args);
	start(args);
	delete_arguments(args);

	return (0);
}
