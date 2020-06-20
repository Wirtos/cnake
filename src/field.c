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
 * Add an item to a temp_item_list_t
 */
static void
_add_temp_item(temp_item_list_t *til, coord_t y, coord_t x, time_t duration)
{
	temp_item_t *new_item = malloc(sizeof(temp_item_t));
	temp_item_t *aux;

	new_item->y = y;
	new_item->x = x;
	new_item->scheduled_destruction = time(NULL) + duration;
	new_item->next = NULL;

	if (*til)
	{
		aux = *til;
		while (aux->next != NULL)
			aux = aux->next;
		aux->next = new_item;
	}
	else
		*til = new_item;
}

/*
 * Get (*y, *x) coordinates of an expired item. Set them to -1 if no
 * expired items are available. It also deallocates the item from the til
 */
static void
get_expired_item(temp_item_list_t *til, coord_t *y, coord_t *x)
{
	temp_item_t *curr, *prev;
	time_t now;

	*y = *x = -1;
	if (*til)
	{
		now = time(NULL);
		prev = NULL;
		curr = *til;
		while (curr)
		{
			if (now >= curr->scheduled_destruction)
			{
				*y = curr->y;
				*x = curr->x;
				if (prev)
					prev->next = curr->next;
				else
					*til = curr->next;
				free(curr);
				break;
			}
			curr = curr->next;
		}
	}
}

/*
 * Deallocates content of a temp_item_list_t
 */
static void
delete_temp_item_list_content(temp_item_list_t til)
{
	if (til)
	{
		if (til->next)
			delete_temp_item_list_content(til->next);
		free(til);
	}
}

/*
 * Stores in (*y, *x) the coordinate of a random empty cell. Returns 0 if
 * no empty cells are found
 */
static int
get_random_empty_cell(field_t *field, coord_t *y, coord_t *x)
{
	coord_t empty_cells[(field->width - 2) * (field->height - 2)][2];
	int i, j, size = 0;

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

	/* Size */
	field->width = width;
	field->height = height;

	/* Matrix (map) */
	field->matrix = malloc(sizeof(cell_t*) * height);
	for (i = 0; i < height; i++)
	{
		field->matrix[i] = malloc(sizeof(cell_t) * width);
		for (j = 0; j < width; j++)
			field->matrix[i][j] = EMPTY;
	}

	/* North border placing */
	for (i = 0; i < width; i++)
		field->matrix[0][i] = BORDER;

	/* South border placing */
	for (i = 0; i < width; i++)
		field->matrix[height - 1][i] = BORDER;

	/* West border placing */
	for (i = 0; i < height; i++)
		field->matrix[i][0] = BORDER;

	/* East border placing */
	for (i = 0; i < height; i++)
		field->matrix[i][width - 1] = BORDER;

	/* Obstacles placing */
	number_obstacles = (height-2) * (width-2) * permill_obstacles / 1000;
	for (i = 0; i < number_obstacles; i++)
		add_obstacle(field);

	/* List of temporal items */
	field->til = NULL;

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

int
add_temp_item(field_t *field, cell_t type, time_t duration)
{
	coord_t y, x;

	if (get_random_empty_cell(field, &y, &x))
	{
		field->matrix[y][x] = type;
		_add_temp_item(&field->til, y, x, duration);
		return (1);
	}
	return (0);
}

void
prolong_temp_items(field_t *field, time_t extra_seconds)
{
	temp_item_t *curr;

	curr = field->til;
	while (curr)
	{
		curr->scheduled_destruction += extra_seconds;
		curr = curr->next;
	}
}

void
remove_expired_items(field_t *field)
{
	coord_t y, x;
	int keep = 1;

	while (keep)
	{
		get_expired_item(&field->til, &y, &x);
		if (y != -1 && x != -1)
			field->matrix[y][x] = EMPTY;
		else
			keep = 0;
	}
}

void
delete_field(field_t *field)
{
	for (int i = 0; i < field->height; i++)
		free(field->matrix[i]);
	free(field->matrix);

	delete_temp_item_list_content(field->til);

	free(field);
}
