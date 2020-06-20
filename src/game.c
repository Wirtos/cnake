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
	init_pair(PAIR_HEAD2, COLOR_BLACK, COLOR_CYAN);
	init_pair(PAIR_PLAYER, COLOR_GREEN, -1);
	init_pair(PAIR_PLAYER2, COLOR_CYAN, -1);
	init_pair(PAIR_FOOD, COLOR_CYAN, -1);
	init_pair(PAIR_SHORTENER, COLOR_BLUE, -1);
	init_pair(PAIR_DECELERATOR, COLOR_GREEN, -1);
	init_pair(PAIR_EXTRA_POINTS, COLOR_YELLOW, -1);
	init_pair(PAIR_TITLE, COLOR_GREEN, COLOR_RED);

	attrset(COLOR_PAIR(PAIR_DEFAULT));
}

/*
 * Updates score marker. Display in two players mode if score2 is not NULL
 */
static void
redraw_score(WINDOW *w_score, unsigned int score, unsigned int *score2)
{
	if (score2)
	{
		mvwaddstr(w_score, 0, 0, "Scores | ");
		wattron(w_score, COLOR_PAIR(PAIR_PLAYER));
		waddstr(w_score, "Player 1: ");
		wattroff(w_score, COLOR_PAIR(PAIR_PLAYER));

		wclrtoeol(w_score);
		wattron(w_score, COLOR_PAIR(PAIR_SCORE));
		wprintw(w_score, "%u", score);
		wattroff(w_score, COLOR_PAIR(PAIR_SCORE));

		wattron(w_score, COLOR_PAIR(PAIR_PLAYER2));
		waddstr(w_score, " Player 2: ");
		wattroff(w_score, COLOR_PAIR(PAIR_PLAYER2));
		wattron(w_score, COLOR_PAIR(PAIR_SCORE));
		wprintw(w_score, "%u", *score2);
		wattroff(w_score, COLOR_PAIR(PAIR_SCORE));
	}
	else
	{
		mvwaddstr(w_score, 0, 0, "Score: ");

		wclrtoeol(w_score);
		wattron(w_score, COLOR_PAIR(PAIR_SCORE));
		wprintw(w_score, "%u", score);
		wattroff(w_score, COLOR_PAIR(PAIR_SCORE));
	}
	wnoutrefresh(w_score);
}

/*
 * Draws the keys window
 */
static void
draw_keys(WINDOW *w_keys, int two_players)
{
	wborder(w_keys, 0, 0, 0, 0, 0, 0, 0, 0);

	wattron(w_keys, A_BOLD);
	mvwaddstr(w_keys, 1, WIDTH_W_KEYS/2 - 2, "Keys");
	wattroff(w_keys, A_BOLD);

	if (two_players)
	{
		wattron(w_keys, COLOR_PAIR(PAIR_PLAYER));
		mvwaddstr(w_keys, 3, 2, "Player 1");
		wattroff(w_keys, COLOR_PAIR(PAIR_PLAYER));
		mvwaddstr(w_keys, 4, 2, "Up:    w, k");
		mvwaddstr(w_keys, 5, 2, "Down:  s, j");
		mvwaddstr(w_keys, 6, 2, "Left:  a, h");
		mvwaddstr(w_keys, 7, 2, "Right: d, l");
		wattron(w_keys, COLOR_PAIR(PAIR_PLAYER2));
		mvwaddstr(w_keys, 9, 2, "Player 2");
		wattroff(w_keys, COLOR_PAIR(PAIR_PLAYER2));
		mvwaddstr(w_keys, 10, 2, "Up:    up arrow");
		mvwaddstr(w_keys, 11, 2, "Down:  down arrow");
		mvwaddstr(w_keys, 12, 2, "Left:  left arrow");
		mvwaddstr(w_keys, 13, 2, "Right: right arrow");
		mvwaddstr(w_keys, 15, 2, "Pause: p");
		mvwaddstr(w_keys, 16, 2, "Quit:  q");
	}
	else
	{
		mvwaddstr(w_keys, 3, 2, "Up:    w, k, up arrow");
		mvwaddstr(w_keys, 4, 2, "Down:  s, j, down arrow");
		mvwaddstr(w_keys, 5, 2, "Left:  a, h, left arrow");
		mvwaddstr(w_keys, 6, 2, "Right: d, l, right arrow");
		mvwaddstr(w_keys, 8, 2, "Pause: p");
		mvwaddstr(w_keys, 9, 2, "Quit:  q");
	}

	wnoutrefresh(w_keys);
}

/*
 * Updates game window acording to the field's map
 */
static void
redraw_game(WINDOW *w_game, field_t *field,
		direction_t dir, direction_t dir2)
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
				case HEAD2:
					if (field->matrix[i][j] == HEAD)
						wattron(w_game, COLOR_PAIR(PAIR_HEAD));
					else
						wattron(w_game, COLOR_PAIR(PAIR_HEAD2));
					switch (field->matrix[i][j] == HEAD ? dir : dir2)
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
					if (field->matrix[i][j] == HEAD)
						wattroff(w_game, COLOR_PAIR(PAIR_HEAD));
					else
						wattroff(w_game, COLOR_PAIR(PAIR_HEAD2));
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
				case EXTRA_POINTS:
					mvwaddch(w_game, i, j, 'e' | COLOR_PAIR(PAIR_EXTRA_POINTS));
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
pause(WINDOW *w_game, field_t *field, time_t delay)
{
	int max_y, max_x;
	time_t before_pause;

	getmaxyx(w_game, max_y, max_x);
	wattron(w_game, A_REVERSE);
	mvwaddstr(w_game, max_y / 2, max_x / 2 - 3, "PAUSED");
	wattroff(w_game, A_REVERSE);
	wrefresh(w_game);

	timeout(-1);  /* Take out timeout */
	before_pause = time(NULL);
	getch();
	prolong_temp_items(field, time(NULL) - before_pause);
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
	snake_t *snake, *snake2, *snakex;
	unsigned int w_game_y, w_keys_height, score, score2, keep_mainloop;
	unsigned int *scorex;
	time_t delay;

	/* Stores who pressed the key */
	enum {AC_TIMEOUT, AC_PLAYER, AC_PLAYER2} action_who;

	set_curses_properties();

	/* Title */
	attron(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
	mvaddstr(1, COLS/2 - 4, "S N A K E");
	attroff(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
	wnoutrefresh(stdscr);

	w_game_y = (LINES+3)/2 - args->height/2;  /* Starting line of w_game */
	w_score = newwin(1, COLS - WIDTH_W_KEYS - 4, w_game_y - 1, 1);
	w_game = newwin(args->height, args->width, w_game_y, 1);
	w_keys_height = args->two_players ? 18 : 11;
	w_keys = newwin(w_keys_height, WIDTH_W_KEYS, LINES/2 - w_keys_height/2,
			COLS - WIDTH_W_KEYS - 1);

	field = init_field(args->height, args->width, args->permill_obstacles);
	snake = init_snake(field, HEAD);
	if (args->two_players)
		snake2 = init_snake(field, HEAD2);
	add_food(field);

	draw_keys(w_keys, args->two_players);

	/* Mainloop */
	score = 0;
	score2 = 0;
	keep_mainloop = 1;
	delay = args->starting_delay;
	timeout(delay);
	while (keep_mainloop)
	{
		remove_expired_items(field);
		if (args->two_players)
			redraw_score(w_score, score, &score2);
		else
			redraw_score(w_score, score, NULL);
		redraw_game(w_game, field, snake->direction,
				args->two_players ? snake2->direction : 0);
		doupdate();

		/* Get user input */
		action_who = AC_PLAYER;
		switch (getch())
		{
			case ERR:
				action_who = AC_TIMEOUT;
				break;
			case 'w':
			case 'k':
				snake->direction = NORTH;
				break;
			case 'a':
			case 'h':
				snake->direction = WEST;
				break;
			case 's':
			case 'j':
				snake->direction = SOUTH;
				break;
			case 'd':
			case 'l':
				snake->direction = EAST;
				break;

				/* Keys functionality depends on two_players mode */
			case KEY_UP:
				if (args->two_players)
				{
					snake2->direction = NORTH;
					action_who = AC_PLAYER2;
				}
				else
					snake->direction = NORTH;
				break;
			case KEY_LEFT:
				if (args->two_players)
				{
					snake2->direction = WEST;
					action_who = AC_PLAYER2;
				}
				else
					snake->direction = WEST;
				break;
			case KEY_RIGHT:
				if (args->two_players)
				{
					snake2->direction = EAST;
					action_who = AC_PLAYER2;
				}
				else
					snake->direction = EAST;
				break;
			case KEY_DOWN:
				if (args->two_players)
				{
					snake2->direction = SOUTH;
					action_who = AC_PLAYER2;
				}
				else
					snake->direction = SOUTH;
				break;
			case 'p':
				pause(w_game, field, delay);
				break;
			case 'q':
				keep_mainloop = 0;
		}

		/* Move the snake */
		for (int i = 0; i <= 1 && keep_mainloop; i++)
		{
			if (i == 0) /* Player turn */
			{
				/* Skip to player2 if the action was of him */
				if (action_who == AC_PLAYER2)
					continue;
				snakex = snake;
				scorex = &score;
			}
			else /* Player2 turn */
			{
				if (!args->two_players || action_who == AC_PLAYER)
					break;
				snakex = snake2;
				scorex = &score2;
			}

			switch (advance(field, snakex))
			{
				case EMPTY:
					break;
				case SNAKE:
				case HEAD:
				case HEAD2:
				case BORDER:
				case OBSTACLE:
					keep_mainloop = 0;
					break;
				case FOOD:
					add_food(field);
					*scorex += POINTS_FOOD;

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
					if (rand() % PROBABILITY_EXTRA_POINTS == 0)
						add_temp_item(field, EXTRA_POINTS, DURATION_EXTRA_POINTS);
					break;
				case SHORTENER:
					*scorex += POINTS_SHORTENER;
					break;
				case DECELERATOR:
					*scorex += POINTS_DECELERATOR;
					delay = args->starting_delay;
					timeout(delay);
					break;
				case EXTRA_POINTS:
					*scorex += POINTS_EXTRA_POINTS;
					break;
			}
		}
	}

	/* Kill Ncurses */
	delwin(w_score);
	delwin(w_game);
	delwin(w_keys);
	endwin();

	if (args->two_players)
	{
		printf("Player %d died first\n", snakex == snake ? 1 : 2);
		puts("====================");
		printf("Player 1 score: %u\n", score);
		printf("Player 2 score: %u\n", score2);
	}
	else
		printf("Your score: %u\n", score);

	/* Free the memory */
	delete_snake(snake);
	if (args->two_players)
		delete_snake(snake2);
	delete_field(field);

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
