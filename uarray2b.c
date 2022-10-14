/******************************************************************************
 *
 *     uarray2b.c
 *
 *     Assignment: HW 3 - Locality
 *     Authors:    Daniel Peng (dpeng01), Ranvir Malik (rmalik04)
 *     Date:       10/13/2022
 *
 *     Implements the functions for the UArray2b_T data structure      
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <uarray.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <math.h>


#define T UArray2b_T

/* struct that stores the dimensions, blocksize, element size, and UArray2 */
struct T
{
        int height;
        int width;
        int size;
        int blocksize;
        UArray2_T main_arr;
};



static void map_block(int width, int height, T array2b, UArray_T block,
        void apply(int col, int row, T array2b, void *elem, void *cl),
                                void *cl);


/*
 * Name: UArray2b_new
 * Purpose: create a new UArray2b with dimensions, size and blocksize that
 *          the client inputs
 * Parameters: integers representing the width and height of the array, the
 *             size of the elements stored in the array, and the blocksize
 * Returns: a UArray2b_T struct
 * Effects: allocates memory for the UArray2b_T struct
 * Error Cases: if the width and height are less than or equal to 0, or if the
 *              blocksize is less than 1, the program will terminate with a
 *              allocate memory for the struct, the program will terminate with
 *              a checked runtime error.
 */
extern T UArray2b_new (int width, int height, int size, int blocksize)
{
        assert(width >= 0 && height >= 0 && blocksize >= 1);
        T new_2b = malloc(sizeof(*new_2b));
        assert(new_2b != NULL);

        /* Set width, height, size, blocksize */
        new_2b->width     = width;
        new_2b->height    = height;
        new_2b->size      = size;
        new_2b->blocksize = blocksize;
        
        /* Calculating the number of blocks needed */
        int block_h = height / blocksize + 1;
        int block_w = width / blocksize + 1;
        if(width % blocksize == 0){
                block_w = block_w - 1;
        }
        if(height % blocksize == 0){
                block_h = block_h - 1;
        }

        /* Initializing the 2d UArray */
        new_2b->main_arr = UArray2_new(block_w, block_h, new_2b->size);

        /* Populating the UArray2 with UArrays */
        for(int i = 0; i < block_h; i++){
                for(int j = 0; j < block_w; j++){
                        UArray_T *new_array = UArray2_at(new_2b->main_arr, 
                                                                j, i);
                        *new_array = UArray_new(blocksize * blocksize, size);
                }
        }

        /* Return the UArray2b */
        return new_2b;
}


/*
 * Name: UArray2b_new_64K_block
 * Purpose: create a new UArray2b with dimensions and size inputted by the
 *          client, but defaults the block size to 64KB, which is the biggest
 *          it can be.
 * Parameters: integers representing the width and height of the array, and the
 *             size of the elements stored in the array.
 * Returns: a UArray2b_T struct
 * Effects: allocates memory for the UArray2b_T struct through calling the
 *          function "UArray2b_new()"
 * Error Cases: If the blocksize is larger than 64KB, then the blocksize is set
 *              to 1. Other error cases are handled by the function
 *              "UArray2b_new()".
 */
extern T UArray2b_new_64K_block (int width, int height, int size)
{
        int blocksize;
        /* If the given size is larger than 64K, set blocksize to 1 */
        if(size > 65536){
                blocksize = 1;
        }
        else{
                blocksize = (int) sqrt((65536 / size));
        }
        return UArray2b_new(width, height, size, blocksize);
}


/*
 * Name: UArray2b_free
 * Purpose: free the memeory allocated by the UArray2b_T struct
 * Parameters: a pointer to the UArray2b_T struct
 * Returns: nothing
 * Effects: frees and recycles memory allocated for the UArray2b_T struct
 * Error Cases: If the struct passed into the function is NULL, the program
 *              will terminate with a checked runtime error
 */
extern void UArray2b_free (T *array2b)
{
        assert(array2b != NULL);
        for(int i = 0; i < UArray2_height((*array2b)->main_arr); i++){
                for(int j = 0; j < UArray2_width((*array2b)->main_arr); j++){
                        UArray_T *curr = UArray2_at((*array2b)->main_arr, 
                                j, i);
                        UArray_free(curr);
                }
        }
        UArray2_free(&(*array2b)->main_arr);
        free(*array2b);
}


/*
 * Name: UArray2b_width
 * Purpose: returns the width of the UArray2b array
 * Parameters: the UArray2b array the client wants to get the width of
 * Returns: an integer representing the width of the array
 * Effects: none
 * Error Cases: If the struct passed into the function is NULL, the program
 *              will terminate with a checked runtime error
 */
extern int UArray2b_width (T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}


/*
 * Name: UArray2b_height
 * Purpose: returns the height of the UArray2b array
 * Parameters: the UArray2b array the client wants to get the height of
 * Returns: an integer representing the height of the array
 * Effects: none
 * Error Cases: If the struct passed into the function is NULL, the program
 *              will terminate with a checked runtime error
 */
extern int UArray2b_height (T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}


/*
 * Name: UArray2b_size
 * Purpose: returns the element size of the UArray2b array
 * Parameters: the UArray2b array the client wants to get the element size of
 * Returns: an integer representing the size of the elements in the array
 * Effects: none
 * Error Cases: If the struct passed into the function is NULL, the program
 *              will terminate with a checked runtime error
 */
extern int UArray2b_size(T array2b)
{
        assert(array2b != NULL);
        int size = array2b->size;
        return size;
}


/*
 * Name: UArray2b_blocksize
 * Purpose: returns the block size of the UArray2b array
 * Parameters: the UArray2b array the client wants to get the block size of
 * Returns: an integer representing the size of the blocks in the array
 * Effects: none
 * Error Cases: If the struct passed into the function is NULL, the program
 *              will terminate with a checked runtime error
 */
extern int UArray2b_blocksize(T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}


/*
 * Name: UArray2b_at
 * Purpose: gets the element in the array at the inputted coordinates
 * Parameters: a UArray2b_T array, and two intergers, column and row, that
 *             represent the coordinates of the element
 * Returns: returns a pointer to the element in the cell
 * Effects: none
 * Error Cases: If the struct passed into the function is NULL, the program
 *              will terminate with a checked runtime error. Additionally,
 *              if the inputted row or column are greater than the width or
 *              height of the array, the program will terminate with a checked
 *              runtime error
 */
extern void *UArray2b_at(T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert(column < UArray2b_width(array2b) && 
                        row < UArray2b_height(array2b));
        int blksize = array2b->blocksize;
        UArray_T *block = UArray2_at(array2b->main_arr,
                            column / blksize, row / blksize);
        return UArray_at(*block, blksize * 
                        (row % blksize) + (column % blksize));
}


/*
 * Name: map_block
 * Purpose: iterates through the UArray2b by iterating block by block. It will
 *          iterate through the blocks in row major order.
 * Parameters: two integers representing the width and height of the UArray2b,
 *             a UArray2b_T array, a UArray_T array that represents a block,
 *             an apply function, and a closure variable that can be casted to
 *             whatever the client needs
 * Returns: nothing
 * Effects: none
 * Error Cases: If the inputted struct is NULL, the program will terminate with
 *              a checked runtime error
 */
extern void UArray2b_map(T array2b, void apply(int col, int row, T array2b,
                        void *elem, void *cl), void *cl)
{
        assert(array2b != NULL);
        assert(array2b->main_arr != NULL);
        int block_w = UArray2_width(array2b->main_arr);
        int block_h = UArray2_height(array2b->main_arr);
        for(int i = 0; i < block_h; i++){
                for(int j = 0; j < block_w; j++){
                        UArray_T block = 
                               *(UArray_T*)UArray2_at(array2b->main_arr, j, i);
                        map_block(j, i, array2b, block, apply, cl);
                }
        }

}


/*
 * Name: map_block
 * Purpose: iterates through one block. It will iterate through the blocks in
            row major order.
 * Parameters: two integers representing the width and height of the UArray2b,
               a UArray2b_T array, a UArray_T array that represents a block,
               an apply function, and a closure variable that can be casted to
               whatever the client needs
 * Returns: nothing
 * Effects: none
 * Error Cases: Error cases are handled by the "UArray2b_map()" function
 */
void map_block(int width, int height, T array2b, UArray_T block,
        void apply(int col, int row, T array2b, void *elem, void *cl),
                                void *cl)
{
        assert(block);
        int blksize = array2b->blocksize;
        int col = width * blksize;
        int row = height * blksize;

        /* Loops through row and column of block */
        for(int i = row; i < row + blksize; i++){
                for(int j = col; j < col + blksize; j++){
                        /* Checks if it is in bounds */
                        if((i < array2b->height) && (j < array2b->width)){
                                int index = blksize * (i % blksize) 
                                                        + j % blksize;
                                apply(j, i, array2b, UArray_at(block, index),
                                        cl);
                        }
                }
        }
}