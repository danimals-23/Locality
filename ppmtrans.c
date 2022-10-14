/******************************************************************************
 *
 *     ppmtrans.c
 *
 *     Assignment: HW 3 - Locality
 *     Authors:    Daniel Peng (dpeng01), Ranvir Malik (rmalik04)
 *     Date:       10/13/2022
 *
 *     Implementation of a program which transforms images, similarly to 
 *     jpegtran. The client can use this program to flip or rotate images.
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"


#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)


/*
 *                     HELPER FUNCTIONS BELOW
 */

/*
 * name:      new_a2_array
 * purpose:   Creates a new A2Methods UArray from a given image and
 *            rotation degree
 *
 * arguments: A Pnm_ppm image which is the original image that the client wants
 *            to flip
 *            
 *            An integer representing the degrees of rotation for the image
 *
 * returns:   Returns a new A2Methods_UArray to store the rotated image
 *
 * effects:   Depending on the rotation, width and height will be adjusted
 */
static A2Methods_UArray2 new_a2_array(Pnm_ppm image, int degrees)
{
        assert(image != NULL);
        /* Flag to check if we need to adjust width and height */
        int flag = degrees % 180;

        const struct A2Methods_T *method_suite = image->methods;
        int height = method_suite->height(image->pixels);
        int width  =  method_suite->width(image->pixels);
        int size   =   method_suite->size(image->pixels);

        A2Methods_UArray2 result;

        if(flag != 0){ /* If 90 or 270 degrees, swap height and width */
                result = method_suite->new(height, width, size);
        }
        else{
                result = method_suite->new(width, height, size);
        }
        return result;
}


/*
 * name:      transpose
 * purpose:   Sets the current element to its transposed element from the
 *            original image (swapping x and y coordinates)
 *
 * arguments: Two ints representing the index of the current element
 *            An A2Methods_UArray2 object
 *            The current element
 *            A void closure pointer
 *
 * returns:   None
 *
 * effects:   Gets the transposed pixel from the original array and sets it
 *            to the current pixel
 */
static void transpose(int col, int row, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = cl;
        const struct A2Methods_T *method_suite = image->methods;
        *(Pnm_rgb) elem = *(Pnm_rgb) method_suite->at(image->pixels,
                                        row, col);
        (void) array2;
}


/*
 * name:      transpose_image
 * purpose:   Transposes an image; flips the x and y coordinates
 *
 * arguments: A Pnm_ppm image representing the original image
 *            The mapping method from A2Methods
 *            A char* of the time file name
 *
 * returns:   None
 *
 * effects:   Flips the x and y coordinates of the original image,
 *            transposes the image.
 */
static void transpose_image(Pnm_ppm image, A2Methods_mapfun *map,
                        char *time_file_name)
{
        /* Timing start */
        CPUTime_T timer;
        double time;
        timer = CPUTime_New();
        CPUTime_Start(timer);

        /* Initialize new array of flipped dimensions for transposing */
        int rotate_degree = 90;
        A2Methods_UArray2 result = new_a2_array(image, rotate_degree);

        map(result, transpose, image);

        time = CPUTime_Stop(timer);

        /* Frees original ppm picture and replaces with the flipped one */
        const struct A2Methods_T *method_suite = image->methods;
        int height = method_suite->height(result);
        int width  =  method_suite->width(result);
        method_suite->free(&image->pixels);
        image->height = height;
        image->width  = width;
        image->pixels = result;

        double ptime = time / (height * width);

        /* Prints timing information */
        if(time_file_name != NULL){
                FILE* time_file = fopen(time_file_name, "w");
                if(time_file == NULL){
                        fprintf(stderr, "Time file could not be opened \n");
                }
                fprintf(time_file, 
                        "Time it took to transpose : %.0f nanoseconds\n",
                                time);
                fprintf(time_file, "Time per pixel: %.0f nanoseconds\n", 
                        ptime);
                fclose(time_file);
        }
        CPUTime_Free(&timer);
}


/*
 * name:      flip_h
 * purpose:   Sets the current element to its flipped element from the original
 *            image (horizontal flip)
 *
 * arguments: Two ints representing the index of the current element
 *            An A2Methods_UArray2 object
 *            The current element
 *            A void closure pointer
 *
 * returns:   None
 *
 * effects:   Gets the flipped element from the original array and sets it
 *            to the current element
 */
static void flip_h(int col, int row, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = cl;
        const struct A2Methods_T *method_suite = image->methods;
        int width = method_suite->width(image->pixels);
        *(Pnm_rgb) elem = *(Pnm_rgb) method_suite->at(image->pixels,
                                        width - col - 1, row);
        (void) array2;
}

/*
 * name:      flip_v
 * purpose:   Sets the current element to its flipped element from the original
 *            image (vertical flip)
 *
 * arguments: Two ints representing the index of the current element
 *            An A2Methods_UArray2 object
 *            The current element
 *            A void closure pointer
 *
 * returns:   None
 *
 * effects:   Gets the flipped element from the original array and sets it
 *            to the current element
 */
static void flip_v(int col, int row, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = cl;
        const struct A2Methods_T *method_suite = image->methods;
        int height = method_suite->height(image->pixels);
        *(Pnm_rgb) elem = *(Pnm_rgb) method_suite->at(image->pixels,
                                        col, height - row - 1);
        (void) array2;
}

/*
 * name:      flip_image
 * purpose:   Flips an image horizontally or vertically
 *
 * arguments: A Pnm_ppm image representing the original image
 *            The mapping method from A2Methods
 *            A char* representing the direction of flip
 *            A char* of the time file name
 *
 * returns:   None
 *
 * effects:   Flips the image horizontally or vertically depending on what
 *            client wants
 */
static void flip_image(Pnm_ppm image, A2Methods_mapfun *map,
                        char* dir, char *time_file_name)
{
        /* Timing start */
        CPUTime_T timer;
        double time;
        timer = CPUTime_New();
        CPUTime_Start(timer);

        /* Initialize new array of same dimensions */
        A2Methods_UArray2 result = new_a2_array(image, 0);
        if(strcmp(dir, "vertical") == 0){ /* Vertical flip */
                map(result, flip_v, image);
        }
        else{ /* horizontal flip */
                map(result, flip_h, image);
        }
        time = CPUTime_Stop(timer);

        /* Frees original ppm picture and replaces with the flipped one */
        const struct A2Methods_T *method_suite = image->methods;
        int height = method_suite->height(result);
        int width  =  method_suite->width(result);
        method_suite->free(&image->pixels);
        image->height = height;
        image->width  = width;
        image->pixels = result;

        double ptime = time / (height * width);

        /* Prints timing information */
        if(time_file_name != NULL){
                FILE* time_file = fopen(time_file_name, "w");
                if(time_file == NULL){
                        fprintf(stderr, "Time file could not be opened \n");
                }
                fprintf(time_file, 
                        "Time it took to do a %s flip : %.0f nanoseconds\n",
                                dir, time);
                fprintf(time_file, "Time per pixel: %.0f nanoseconds\n", 
                        ptime);
                fclose(time_file);
        }
        CPUTime_Free(&timer);
}

/*
 * name:      rotate_180
 * purpose:   Sets the current element to its rotated element from the original
 *            image (180 degree rotation)
 *
 * arguments: Two ints representing the index of the current element
 *            An A2Methods_UArray2 object
 *            The current element
 *            A void closure pointer
 *
 * returns:   None
 *
 * effects:   Gets the rotated element from the original array and sets it
 *            to the current element
 */
static void rotate_180(int col, int row, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = cl;
        const struct A2Methods_T *method_suite = image->methods;
        int height = method_suite->height(image->pixels);
        int width = method_suite->width(image->pixels);
        *(Pnm_rgb) elem = *(Pnm_rgb) method_suite->at(image->pixels,
                                        width - col - 1, height - row - 1);
        (void) array2;
}


/*
 * name:      rotate_90
 * purpose:   Sets the current element to its rotated element from the original
 *            image (90 degree rotation)
 *
 * arguments: Two ints representing the index of the current element
 *            An A2Methods_UArray2 object
 *            The current element
 *            A void closure pointer
 *
 * returns:   None
 *
 * effects:   Gets the rotated element from the original array and sets it
 *            to the current element
 */
static void rotate_90(int col, int row, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = cl;
        const struct A2Methods_T *method_suite = image->methods;
        int height = method_suite->height(image->pixels);
        *(Pnm_rgb) elem = *(Pnm_rgb) method_suite->at(image->pixels,
                                                row, height - col - 1);
        (void) array2;
}


/*
 * name:      rotate_270
 * purpose:   Sets the current element to its rotated element from the original
 *            image (270 degree rotation)
 *
 * arguments: Two ints representing the index of the current element
 *            An A2Methods_UArray2 object
 *            The current element
 *            A void closure pointer
 *
 * returns:   None
 *
 * effects:   Gets the rotated element from the original array and sets it
 *            to the current element
 */
static void rotate_270(int col, int row, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = cl;
        const struct A2Methods_T *method_suite = image->methods;
        int width = method_suite->width(image->pixels);
        *(Pnm_rgb) elem = *(Pnm_rgb) method_suite->at(image->pixels,
                                                width - row - 1, col);
        (void) array2;
}


/*
 * name:      rotate_image
 * purpose:   rotates an image by a specific given amount of degrees
 *
 * arguments: A Pnm_ppm image representing the original image
 *            The mapping method from A2Methods
 *            A char* representing the direction of flip
 *            A char* of the time file name
 *
 * returns:   None
 *
 * effects:   Rotates the image by 0, 90, 180, or 270 degrees, depending on
 *            how the client feels
 */
static void rotate_image(Pnm_ppm image, A2Methods_mapfun* map, 
                        int degrees, char* time_file_name)
{
        assert(image != NULL);
        /* Timing the rotation */
        CPUTime_T timer;
        double time;
        timer = CPUTime_New();
        CPUTime_Start(timer);

        const struct A2Methods_T *method_suite = image->methods;

        if(degrees == 0){
                time = CPUTime_Stop(timer);
                double ptime = time / (method_suite->height(image->pixels) *
                        method_suite->width(image->pixels));
                /* Writes to the timing file if it is given */
                if(time_file_name != NULL){
                        FILE* time_file = fopen(time_file_name, "w");
                        if(time_file == NULL){
                                fprintf(stderr, 
                                "Time file could not be opened \n");
                        }
                        fprintf(time_file,
                "Time it took to rotate %d degrees : %.0f nanoseconds\n",
                                degrees, time);
                        fprintf(time_file, 
                        "Time per pixel: %.0f nanoseconds\n", ptime);
                        fclose(time_file);
                }
                CPUTime_Free(&timer);
                return;
        }

        /* creates result array, degrees used to see if it needs to change
        dimensions */
        A2Methods_UArray2 result = new_a2_array(image, degrees);
        if(degrees == 180) { // no need to change dimensions
                map(result, rotate_180, image);
        }
        else { // change dimensions due to 90 degrees or 270 degrees rotation
                if(degrees == 90){
                        map(result, rotate_90, image);
                }
                if(degrees == 270){
                        map(result, rotate_270, image);
                }
        }

        time = CPUTime_Stop(timer);
        CPUTime_Free(&timer);

        /* Frees the original ppm picture, replaces with the new rotated one */
        int height = method_suite->height(result);
        int width  =  method_suite->width(result);
        method_suite->free(&image->pixels);
        image->height = height;
        image->width  = width;
        image->pixels = result;

        double ptime = time / (height * width);

        /* Writes to the timing file if it is given */
        if(time_file_name != NULL){
                FILE* time_file = fopen(time_file_name, "w");
                if(time_file == NULL){
                        fprintf(stderr, "Time file could not be opened \n");
                }
                fprintf(time_file,
                "Time it took to rotate %d degrees : %.0f nanoseconds\n",
                        degrees, time);
                fprintf(time_file, "Time per pixel: %.0f nanoseconds\n",
                        ptime);
                fclose(time_file);
        }
}


static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}


/*
 * name:      transform
 * purpose:   transforms an image based on what the client asks
 *
 * arguments: A char* of the time file name
 *            A char* of the action the client wants to do to the image
 *            An int representing the degrees of rotation, if given
 *            A char* of the direction of the flip, if given
 *            A FILE* representing the image which will be transformed
 *            An A2Methods_T* representing the method suite being used
 *            An A2Methods_mapfun* representing the mapping function used
 *
 * returns:   None
 *
 * effects:   Transforms the given image based on what the client wants
 * 
 */
static void transform(char* time_file_name, char* action, int degrees,
        char* dir, FILE* img, A2Methods_T* methods, A2Methods_mapfun* map)
{
        /* Creates a Pnm_ppm object using the image and method suite */
        Pnm_ppm original = Pnm_ppmread(img, *methods);
        assert(original != NULL);

        /* Checks to see what the client wanted to do with the image */
        if(strcmp(action, "-rotate") == 0){
                rotate_image(original, map, degrees,
                                time_file_name);
        }
        else if(strcmp(action, "-flip") == 0){
                flip_image(original, map, dir, time_file_name);
        }
        else if(strcmp(action, "-transpose") == 0){
                transpose_image(original, map, time_file_name);
        }

        /* Writes the resulting image to standard output */
        Pnm_ppmwrite(stdout, original);
        Pnm_ppmfree(&original);
}


int main(int argc, char *argv[]) 
{
        /* Initialization of variables which may or may not be used */
        char *time_file_name = NULL;
        int         rotation =    0;
        int                       i;
        char         *action = NULL;
        char            *dir = NULL;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map != NULL);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        action = "-rotate";
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-flip") == 0) {
                        action = "-flip";
                        dir = argv[++i];
                        if(strcmp(dir, "horizontal") == 
                                strcmp(dir, "vertical")){
                                        fprintf(stderr, "Flip must be" \
                                                " horizontal or vertical\n");
                                }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        action = "-transpose";
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }


        }

        /* Read from standard input for a file */
        if(i == argc) {
                if(action != NULL){
                        transform(time_file_name, action, rotation, dir, stdin,
                                        &methods, map);
                } else {
                        rotation = 0;
                        action = "-rotate";
                        transform(time_file_name, action, rotation,
                                dir, stdin, &methods, map);
                }
        }
        else {
        /* Read the file from the last input in the command line */
                FILE *inputfd = fopen(argv[i], "r");
                if(inputfd == NULL){
                        fprintf(stderr, "File could not be opened \n");
                        return EXIT_FAILURE;
                }
        /* Transform the image and close the file */
                if(action != NULL) { 
                        transform(time_file_name, action, 
                                rotation, dir, inputfd, &methods, map);
                } else{
                        rotation = 0;
                        action = "-rotate";
                        transform(time_file_name, action, rotation,
                                dir, inputfd, &methods, map);
                }
                fclose(inputfd);
        }

        return EXIT_SUCCESS;
}