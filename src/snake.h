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

#ifndef SNAKE_H
#define SNAKE_H

#include "field.h"
#include <stdlib.h>

typedef enum direction_e {NORTH, EAST, WEST, SOUTH} direction_t;

typedef struct body_s
{
	coord_t y, x;
	struct body_s *next;
} body_t;

typedef struct snake_s
{
	direction_t direction;
	body_t *tail, *head;
} snake_t;


/*
 * Initialize snake and place it in the field
 */
snake_t*
init_snake(field_t *field);

/*
 * Make the snake advance one cell in the field, return type of
 * the cell it advanced over
 */
cell_t
advance(field_t *field, snake_t *snake);

/*
 * Deallocate the snake
 */
void
delete_snake(snake_t *snake);

#endif /* SNAKE_H */
