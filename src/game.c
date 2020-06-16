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

#include "field.h"
#include "snake.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

/* Some constants */

#define MINIMUM_LINES 10
#define MINIMUM_COLS 70
#define WIDTH_W_KEYS 28

#define DELAY 300 /* milliseconds */

#define PAIR_DEFAULT 1
#define PAIR_SCORE 2
#define PAIR_BORDER 3
#define PAIR_SNAKE 4
#define PAIR_HEAD 5
#define PAIR_FOOD 6
#define PAIR_TITLE 7

/*
 * Checks terminal size and prepares colors
 */
void
set_curses_properties()
{
	if (LINES < MINIMUM_LINES || COLS < MINIMUM_COLS)
	{
		endwin();
		fprintf(stderr, "Terminal too small\n");
		exit(1);
	}

	/* Color pairs definitions */
	start_color();
	use_default_colors();

	init_pair(PAIR_DEFAULT, -1, -1);
	init_pair(PAIR_SCORE, COLOR_YELLOW, -1);
	init_pair(PAIR_BORDER, COLOR_MAGENTA, -1);
	init_pair(PAIR_SNAKE, -1, COLOR_RED);
	init_pair(PAIR_HEAD, COLOR_BLUE, COLOR_GREEN);
	init_pair(PAIR_FOOD, COLOR_CYAN, -1);
	init_pair(PAIR_TITLE, COLOR_GREEN, COLOR_RED);

	attrset(COLOR_PAIR(PAIR_DEFAULT));
}

/*
 * Updates score marker
 */
void
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
void
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
void
redraw_game(WINDOW *w_game, field_t *field, direction_t direction)
{
	size_t i, j;

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
			}
		}
	}

	wnoutrefresh(w_game);
}

void
pause(WINDOW *w_game)
{
	int max_y, max_x;

	getmaxyx(w_game, max_y, max_x);
	wattron(w_game, A_REVERSE);
	mvwaddstr(w_game, max_y / 2, max_x / 2 - 3, "PAUSED");
	wattroff(w_game, A_REVERSE);
	wrefresh(w_game);

	timeout(-1);  /* Take out timeout */
	getch();
	timeout(DELAY);  /* Restore timeout */
}

void
start()
{
	WINDOW *w_score, *w_game, *w_keys;
	field_t *field;
	snake_t *snake;
	unsigned int keep_mainloop = 1, score = 0;
	size_t height_game, width_game;

	set_curses_properties();

	/* Title */
	attron(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
	mvaddstr(1, COLS/2 - 4, "S N A K E");
	attroff(COLOR_PAIR(PAIR_TITLE) | A_BOLD);
	wnoutrefresh(stdscr);

	height_game = LINES - 4;
	width_game = COLS - WIDTH_W_KEYS - 3;
	w_score = newwin(1, COLS - 1, 3, 1);
	w_game = newwin(height_game, width_game, 4, 1);
	w_keys = newwin(11, WIDTH_W_KEYS, LINES/2 - 5, COLS - WIDTH_W_KEYS - 1);

	field = init_field(height_game, width_game);
	snake = init_snake(field);
	add_food(field);

	draw_keys(w_keys);

	/* Mainloop */
	while (keep_mainloop)
	{
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
				pause(w_game);
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
				keep_mainloop = 0;
				break;
			case FOOD:
				score += 10;
				add_food(field);
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

int
main()
{
	initscr();

	timeout(DELAY);       /* Set timeout for keypresses */
	cbreak();             /* Do not buffer keypresses */
	noecho();             /* Do not show keypresses */
	keypad(stdscr, TRUE); /* Enable special keys */
	curs_set(0);          /* Hide cursor */

	start();

	return (0);
}
