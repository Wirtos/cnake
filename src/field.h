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

typedef enum cell_e {EMPTY, SNAKE, FOOD, BORDER} cell_t;

typedef struct
{
	size_t width, height;
	enum cell_e **matrix;  /* [height][width] */
} field_t;


/*
 * Initialize a field with empty (incl. borders) map
 */
field_t*
init_field(size_t width, size_t height);

/*
 * Deallocate field
 */
void
delete_field(field_t *field);

#endif /* FIELD_H */
