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

#include <snake.h>
#include <time.h>

snake_t*
init_snake(field_t *field, cell_t head_type)
{
	snake_t *snake;

	snake = malloc(sizeof(snake_t));

	/* Random initial direction */
	snake->direction = rand() % 4;

	snake->tail = malloc(sizeof(body_t));
	/* Choose random place without direct contact with the borders */
	snake->tail->y = (rand() % (field->height - 4)) + 2;
	snake->tail->x = (rand() % (field->width - 4)) + 2;
	snake->tail->next = NULL;
	snake->neck = snake->tail;
	snake->head = snake->tail;

	/* Head */
	snake->head_type = head_type;
	field->matrix[snake->head->y][snake->head->x] = head_type;

	return (snake);
}

/*
 * Add a node in the specified (y,x) coords, reflecting change in both
 * field and snake
 */
static void
append_head(field_t *field, snake_t *snake, coord_t y, coord_t x)
{
	/* In the field */
	field->matrix[snake->head->y][snake->head->x] = SNAKE;
	field->matrix[y][x] = snake->head_type;

	/* In the snake */
	snake->head->next = malloc(sizeof(body_t));
	snake->head->next->y = y;
	snake->head->next->x = x;
	snake->head->next->next = NULL;
	snake->neck = snake->head;
	snake->head = snake->head->next;
}

/*
 * Delete tail of the snake from field and snake
 */
static void
delete_tail(field_t *field, snake_t *snake)
{
	body_t *aux;

	/* In the field */
	field->matrix[snake->tail->y][snake->tail->x] = EMPTY;

	/* In the snake */
	aux = snake->tail->next;
	free(snake->tail);
	snake->tail = aux;
}

/*
 * Deletes the first half of snake body
 */
static void
delete_half_snake(field_t *field, snake_t *snake)
{
	int snake_length = 0;
	body_t *aux = snake->tail;

	/* Measure snake lenght */
	while (aux != snake->head)
	{
		snake_length++;
		aux = aux->next;
	}

	for (int i = 0; i < snake_length/2; i++)
	{
		field->matrix[snake->tail->y][snake->tail->x] = EMPTY;

		aux = snake->tail->next;
		free(snake->tail);
		snake->tail = aux;
	}
}

/*
 * Sets the snake's direction to the opposite of the current one
 */
static void
reverse_direction(snake_t *snake)
{
	switch (snake->direction)
	{
		case NORTH:
			snake->direction = SOUTH;
			break;
		case EAST:
			snake->direction = WEST;
			break;
		case WEST:
			snake->direction = EAST;
			break;
		case SOUTH:
			snake->direction = NORTH;
	}
}

cell_t
advance(field_t *field, snake_t *snake)
{
	coord_t next_y = 0, next_x = 0;
	cell_t old_type;

	switch (snake->direction)
	{
		case NORTH:
			next_y = snake->head->y - 1;
			next_x = snake->head->x;
			break;
		case EAST:
			next_y = snake->head->y;
			next_x = snake->head->x + 1;
			break;
		case WEST:
			next_y = snake->head->y;
			next_x = snake->head->x - 1;
			break;
		case SOUTH:
			next_y = snake->head->y + 1;
			next_x = snake->head->x;
	}

	switch (old_type = field->matrix[next_y][next_x])
	{
		case SHORTENER:
			delete_half_snake(field, snake);
			#ifndef __clang__  /* Hint for GCC only */
			__attribute__((fallthrough));
			#endif
		case DECELERATOR:
		case EXTRA_POINTS:
		case EMPTY:
			append_head(field, snake, next_y, next_x);
			delete_tail(field, snake);
			break;
		case FOOD:
			append_head(field, snake, next_y, next_x);
			break;
		case SNAKE:
			/*
			 * Reverse direction and advance again if hitting the neck so
			 * the snake doesn't die if it tries to go against it
			 */
			if (next_y == snake->neck->y && next_x == snake->neck->x)
			{
				reverse_direction(snake);
				old_type = advance(field, snake);
			}
			break;
		case BORDER:
		case HEAD:
		case HEAD2:
		case OBSTACLE:
			break;  /* Is ded so nothing to do */
	}

	return (old_type);
}

/*
 * Recursively free body from the tail
 */
static void
delete_body(body_t *node)
{
	if (node->next)
		delete_body(node->next);
	free(node);
}

void
delete_snake(snake_t *snake)
{
	delete_body(snake->tail);
	free(snake);
}
