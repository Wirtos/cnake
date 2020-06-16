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

#ifndef FIELD_H
#define FIELD_H

#include <stdlib.h>

typedef size_t coord_t;
typedef enum {EMPTY, SNAKE, HEAD, FOOD, BORDER} cell_t;

typedef struct
{
	size_t width, height;
	cell_t **matrix;  /* [height][width] */
} field_t;


/*
 * Initialize a field with empty (incl. borders) matrix
 */
field_t*
init_field(size_t height, size_t width);

/*
 * Add a random cell with food into the matrix. Return 0 if there wasn't
 * space for it. Return 1 in success
 */
int
add_food(field_t *field);

/*
 * Deallocate field
 */
void
delete_field(field_t *field);

#endif /* FIELD_H */
