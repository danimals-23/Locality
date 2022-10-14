/*
 * uarray2.h
 * Bill Liu (zliu26) + Daniel Peng (dpeng01)
 * CS40: iii
 * 
 * Interface of the uarray2 data structure, which represents a two dimensional
 * array that stores anything. This file contains all of the relevant functions 
 * that uarray2 can use.
 *
 */
#ifndef __UARRAY_H__
#define __UARRAY_H__

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <uarray.h>

# define T UArray2_T
typedef struct T *T;

/*
 * purpose: Uarray2_new creates a new 2D Uarray with size width * height on
 *          heap and returns the pointer to it
 *
 * arguments: 
 *      width: integer that is large or equal to 0
 *      height: integer that is large or equal to 0
 *      size: size of type of element stored in the array
 *      
 * returns: returns a pointer to the new 2D Uarray
 */
T UArray2_new(int width, int height, int size);

/*
 * purpose: Uarray2_free deallocates memory used by 2D uarray struct
 *
 * arguments: 
 *      uarray2: double pointer to a valid uarray2 struct
 *      
 * returns: void return type
 */
void UArray2_free(T *uarray2);

/*
 * purpose: Uarray2_width returns the width of the 2D uarray
 *
 * arguments: 
 *      uarray2: double pointer to a valid uarray2 struct
 *      
 * returns: returns width as a integer
 */
int UArray2_width(T uarray2);

/*
 * purpose: Uarray2_height returns the height of the 2D uarray
 *
 * arguments: 
 *      uarray2: double pointer to a valid uarray2 struct
 *      
 * returns: returns height as a integer
 */
int UArray2_height(T uarray2);

/*
 * purpose: Uarray2_size returns the size of element type stored
 *
 * arguments: 
 *      uarray2: a double pointer to a valid uarray2 struct
 *      
 * returns: returns size as a size_t
 */
int UArray2_size(T uarray2);

/*
 * purpose: Uarray2_at search for the value stored at the specific position
 *
 * arguments: 
 *      uarray2: a pointer to a valid uarray2
 *      width: integer that falls between 0 and width of the 2D uarray
 *      height: integer that falls between 0 and width of the 2D uarray
 *      
 * returns: returns the a pointer to value stored at the required position
 */
void *UArray2_at(T uarray2, int width, int height);

/*
 * purpose: UArray2_map_row_major maps over the 2D uarray and execute apply
 *          function to every single values stored in row major order
 *
 * arguments: 
 *      uarray2: a pointer to a valid uarray2
 *      apply: an apply function given outside
 *      cl: application-specific pointer passed by client
 *      
 * returns: void return types
 */
void UArray2_map_row_major(T uarray2, void apply(int width, int height,
                            T uarray2, void* value, void *cl), void *cl);

/*
 * purpose: UArray2_map_col_major maps over the 2D uarray and execute apply
 *          function to every single values stored in col major order
 *
 * arguments: 
 *      uarray2: a pointer to a valid uarray2
 *      apply: an apply function given outside
 *      cl: application-specific pointer passed by client
 *      
 * returns: void return types
 */
void UArray2_map_col_major(T uarray2, void apply(int width, int height,
                            T uarray2, void* value, void *cl), void *cl);
# undef T
# endif