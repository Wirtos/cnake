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

#ifndef CONFIG_H
#define CONFIG_H

/*
 * Constants starting by DEFAULT_ can be changed through
 * command line arguments
 */

/* Sizes */
#define WIDTH_W_KEYS 28
#define DEFAULT_W_GAME_HEIGHT 26
#define DEFAULT_W_GAME_WIDTH 66

/* Obstacles */
#define DEFAULT_PERMILL_OBSTACLES 10

/* Delays */
/* milliseconds */
#define DEFAULT_STARTING_DELAY 300
#define DEFAULT_MINIMUM_DELAY 120
#define DEFAULT_STEP_DELAY 10
/* seconds */
#define DEFAULT_DURATION_SHORTENER 5
#define DEFAULT_DURATION_DECELERATOR 7
#define DEFAULT_DURATION_EXTRA_POINTS 5

/* Points */
#define POINTS_FOOD 10
#define POINTS_SHORTENER 15
#define POINTS_DECELERATOR 5
#define POINTS_EXTRA_POINTS 50

/*
 * Probabilities: 1/X chances of item appearing when eating a FOOD
 */
#define DEFAULT_PROBABILITY_SHORTENER 15
#define DEFAULT_PROBABILITY_DECELERATOR 10
#define DEFAULT_PROBABILITY_EXTRA_POINTS 10

/* Map change */
#define DEFAULT_SCORE_STEP_MAP_CHANGE 200

#endif /* CONFIG_H */
