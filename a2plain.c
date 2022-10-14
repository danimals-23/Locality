/******************************************************************************
 *
 *     a2plain.c
 *
 *     Assignment: HW 3 - Locality
 *     Authors:    Daniel Peng (dpeng01), Ranvir Malik (rmalik04)
 *     Date:       10/13/2022
 *
 *     Implements the functions for the A2Methods_T class that use a UArray2    
 *     data structure.
 *
 *****************************************************************************/

#include <string.h>

#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/



typedef A2Methods_UArray2 A2;

/*
 * Name: new
 * Purpose: create a new UArray2 with dimensions and size that the client
            inputs
 * Parameters: two integers representing the width and height of the UArray2
               and the size of the elements stored in the array
 * Returns: an A2 array
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                height and width are less than 0 the program will terminate
                with a checked runtime error.
*/
static A2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

 
/*
 * Name: new_with_blocksize
 * Purpose: create a new UArray2 with dimensions and size that the client
            inputs. Blocksize is not needed since it is not a blocked array
 * Parameters: integers representing the width and height of the UArray2,
               the size of the elements stored in the array, and the blocksize
 * Returns: an A2 array
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                height and width are less than 0 the program will terminate
                with a checked runtime error.
*/
static A2 new_with_blocksize(int width, int height, int size, int blocksize)
{
        (void) blocksize;
        return new(width, height, size);
}


/*
 * Name: a2free
 * Purpose: frees and recyles the memory allocated by the A2 array
 * Parameters: the A2 array the client wants to free
 * Returns: nothing
 * Effects: recyles the memory allocated by the A2 array
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                A2 array the client wants to free is NULL, the program will
                terminate with a checked runtime error
*/
static void a2free(A2 * array2p)
{
        UArray2_free((UArray2_T *) array2p);
}


/*
 * Name: width
 * Purpose: returns the width of the A2 array
 * Parameters: the A2 array the client wants to get the width of
 * Returns: an integer representing the width of the A2 array
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                A2 array the client wants to get the width of is NULL,
                the program will terminate with a checked runtime error
*/
static int width(A2 array2)
{
        return UArray2_width(array2);
}


/*
 * Name: height
 * Purpose: returns the height of the A2 array
 * Parameters: the A2 array the client wants to get the height of
 * Returns: an integer representing the height of the A2 array
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                A2 array the client wants to get the height of is NULL, the
                program will terminate with a checked runtime error
*/
static int height(A2 array2)
{
        return UArray2_height(array2);
}


/*
 * Name: size
 * Purpose: returns the size of the type of element stored in the A2 array
 * Parameters: the A2 array the client wants to get the element size of
 * Returns: an integer representing the element size of the A2 array
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                A2 array the client wants to get the element isze of is NULL,
                the program will terminate with a checked runtime error
*/
static int size(A2 array2)
{
        return UArray2_size(array2);
}


/*
 * Name: blocksize
 * Purpose: returns the blocksize of the A2 array. This will always be 1 since
            a normal array stores one element in a "block"
 * Parameters: the A2 array the client wants to get the block size of
 * Returns: an integer representing the block size of the A2 array
 * Effects: none
 * Error Cases: If the A2 array the client wants to get the blocksize of
                is NULL, the program will terminate with a checked runtime
                error
*/
static int blocksize(A2 array2)
{
        (void)array2;
        return 1;
}


/*
 * Name: at
 * Purpose: returns the element at the inputted position in the A2 array
 * Parameters: an A2 array, and two integers representing the coordinates
               (width and height) of the elemenet they are trying to access
 * Returns: a pointer to an A2Methods_Object which is the element at the
            inputted coordinates
 * Effects: none
 * Error Cases: If the A2 array the client inputs is NULL, the program will
                terminate with a checked runtime error. Similarly, if the width
                and height are negative or out of bounds of the array, the
                program will terminate with a checked runtime error.
*/
static A2Methods_Object *at(A2 array2, int width, int height)
{
        return UArray2_at(array2, width, height);
}


/*
 * Name: UArray2_applyfun
 * Purpose: defines the apply function that will be used in the mapping
            functions
 * Parameters: a UArray2_T array, two integers representing the coordinates
               (col and row) of the element they are trying to access, and
               two void pointers that can be casted to anything the client
               needs.
 * Returns: nothing
 * Effects: none
 * Error Cases: none
*/
typedef void UArray2_applyfun(int i, int j, UArray2_T array2b, void *elem, 
                        void *cl);


/*
 * Name: map_row_major
 * Purpose: iterates through the A2Methods_Uarray2 row by row. That is,
            starting from (0,0), it will iterate (1, 0), (2, 0), (3, 0)...
 * Parameters: an A2Methods_UArray2 array, an apply function, and a closure
               variable
 * Returns: nothing
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                inputted A2Methods_UArray2 array is NULL, the program will
                terminate with a checked runtime error.
*/
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}


/*
 * Name: map_col_major
 * Purpose: iterates through the A2Methods_Uarray2 by column. That is,
            starting from (0,0), it will iterate (0, 1), (0, 2), (0, 3)...
 * Parameters: an A2Methods_UArray2 array, an apply function, and a closure
               variable
 * Returns: nothing
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                inputted A2Methods_UArray2 array is NULL, the program will
                terminate with a checked runtime error.
*/
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}


struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};


/*
 * Name: apply_small
 * Purpose: defines and implements an apply function that is used for small
            iterations of the array and casts it to the closure variable
 * Parameters: a UArray2_T array, two integers representing the coordinates
               (col and row) of the element they are trying to access, and
               two void pointers that can be casted to anything the client
               needs.
 * Returns: nothing
 * Effects: none
 * Error Cases: none
*/
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}


/*
 * Name: small_map_row_major
 * Purpose: iterates through smaller A2Methods_Uarray2 row by row. That is,
            starting from (0,0), it will iterate (1, 0), (2, 0), (3, 0)...
 * Parameters: an A2Methods_UArray2 array, an apply function, and a closure
               variable
 * Returns: nothing
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                inputted A2Methods_UArray2 array is NULL, the program will
                terminate with a checked runtime error.
*/
static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}


/*
 * Name: small_map_col_major
 * Purpose: iterates through smaller A2Methods_Uarray2 by column. That is,
            starting from (0,0), it will iterate (0, 1), (0, 2), (0, 3)...
 * Parameters: an A2Methods_UArray2 array, an apply function, and a closure
               variable
 * Returns: nothing
 * Effects: none
 * Error Cases: Error cases are handled by the UArray2 implementation. If the
                inputted A2Methods_UArray2 array is NULL, the program will
                terminate with a checked runtime error.
*/
static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}


/* defining all the functions that are a part of the uarray2_methods_plain */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize, 
        at,
        map_row_major,
        map_col_major,
        NULL,
        map_row_major, // Default mapping function
        small_map_row_major,
        small_map_col_major,
        NULL,
        small_map_row_major,
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
