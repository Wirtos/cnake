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

#include <field.h>

/*
 * Stores in (*y, *x) the coordinate of a random empty cell. Returns 0 if
 * no empty cells are found
 */
static int
get_random_empty_cell(field_t *field, coord_t *y, coord_t *x)
{
	coord_t empty_cells[(field->width - 2) * (field->height - 2)][2]; int i, j, size = 0;

	/* Find all the EMPTY cells and store their coordinates in empty_cells */
	for (i = 1; i <= field->height - 2; i++)
	{
		for (j = 1; j <= field->width - 2; j++)
		{
			if (field->matrix[i][j] == EMPTY)
			{
				empty_cells[size][0] = i;
				empty_cells[size][1] = j;
				size++;
			}
		}
	}

	if (size > 0)
	{
		i = rand() % size;  /* Choose a random item of empty_cells */
		*y = empty_cells[i][0];
		*x = empty_cells[i][1];
		return (1);
	}
	return (0);
}

/*
 * Add a random cell with food into the matrix. Return 0 if there wasn't
 * space for it. Return 1 in success
 */
static int
add_obstacle(field_t *field)
{
	coord_t y, x;

	if (get_random_empty_cell(field, &y, &x))
	{
		field->matrix[y][x] = OBSTACLE;
		return (1);
	}
	return (0);
}

field_t*
init_field(int height, int width, int permill_obstacles)
{
	field_t *field;
	int i, j, number_obstacles;

	field = malloc(sizeof(field_t));
	field->width = width;
	field->height = height;

	field->matrix = malloc(sizeof(cell_t*) * height);
	for (i = 0; i < height; i++)
	{
		field->matrix[i] = malloc(sizeof(cell_t) * width);
		for (j = 0; j < width; j++)
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

	/* Obstacles */
	number_obstacles = (height-2) * (width-2) * permill_obstacles / 1000;
	for (i = 0; i < number_obstacles; i++)
		add_obstacle(field);

	return (field);
}

int
add_food(field_t *field)
{
	coord_t y, x;

	if (get_random_empty_cell(field, &y, &x))
	{
		field->matrix[y][x] = FOOD;
		return (1);
	}
	return (0);
}

void
delete_field(field_t *field)
{
	for (int i = 0; i < field->height; i++)
		free(field->matrix[i]);
	free(field->matrix);
	free(field);
}
