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

field_t*
init_field(size_t width, size_t height)
{
	field_t *field;
	size_t i;

	field = malloc(sizeof(field_t));
	field->width = width;
	field->height = height;

	field->matrix = malloc(sizeof(cell_t*) * height);
	for (i = 0; i < height; i++)
	{
		field->matrix[i] = malloc(sizeof(cell_t) * width);
		for (size_t j = 0; j < width; j++)
			field->matrix[i][j] = EMPTY;
	}

	/* North border */
	for (i = 0; i < width; i++)
		field->matrix[0][i] = BORDER;

	/* South border */
	for (i = 0; i < width; i++)
		field->matrix[height - 1][i] = BORDER;

	/* West border */
	for (i = 0; i < height; i++)
		field->matrix[i][0] = BORDER;

	/* East border */
	for (i = 0; i < height; i++)
		field->matrix[i][width - 1] = BORDER;

	return (field);
}

void
delete_field(field_t *field)
{
	for (size_t i = 0; i < field->height; i++)
		free(field->matrix[i]);
	free(field->matrix);
	free(field);
}
