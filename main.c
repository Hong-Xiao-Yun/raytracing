#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "thread.h"

#include "primitives.h"
#include "raytracing.h"
#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512

pthread_t tid[4];
pthread_mutex_t lock;



static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}
void setStruct(thread_args **threadS,int n,uint8_t *pixels, color background_color,
                rectangular_node rectangulars, sphere_node spheres,
                light_node lights, const viewpoint *view,int  width,int height);

int main()
{
    uint8_t *pixels;
    light_node lights = NULL;
    rectangular_node rectangulars = NULL;
    sphere_node spheres = NULL;
    color background = { 0.0, 0.1, 0.1 };
    struct timespec start, end;

    thread_args *threadS[4];



#include "use-models.h"

    /* allocate by the given resolution */
    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

    printf("# Rendering scene\n");
    /* do the ray tracing with the given geometry */
    clock_gettime(CLOCK_REALTIME, &start);

    /*thread*/
    setStruct(threadS,4,pixels, background,rectangulars,spheres,lights, &view,ROWS,COLS);
   // printf(" getchar()\n");
    // getchar();
    
    int err;
    int k=0;
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    // while(k < 1)
    // {
         // pthread_create(&(tid[0]), NULL, &raytracing, threadS[0]);
         pthread_create(&(tid[1]), NULL, &raytracing, threadS[1]);
          // err = pthread_create(&(tid[2]), NULL, &raytracing, threadS[2]);
           // err = pthread_create(&(tid[3]), NULL, &raytracing, threadS[3]);
        // if (err != 0)
            // printf("\ncan't create thread :[%s]", strerror(err));
        // k++;
    // }

    // pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    // pthread_join(tid[2], NULL);
    // pthread_join(tid[3], NULL);
    pthread_mutex_destroy(&lock);
    

    /*raytracing(pixels, background, rectangulars, spheres, lights, &view, 0, ROWS/2,0,COLS/2,ROWS,COLS);
    raytracing(pixels, background, rectangulars, spheres, lights, &view, 0, ROWS/2,COLS/2,COLS,ROWS,COLS);
    raytracing(pixels, background, rectangulars, spheres, lights, &view, ROWS/2, ROWS,0,COLS/2,ROWS,COLS);
    raytracing(pixels, background, rectangulars, spheres, lights, &view, ROWS/2, ROWS,COLS/2,COLS,ROWS,COLS);
    */

   /* raytracing(pixels, background,
               rectangulars, spheres, lights, &view, ROWS, COLS);*/
    clock_gettime(CLOCK_REALTIME, &end);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);
    }

    delete_rectangular_list(&rectangulars);
    delete_sphere_list(&spheres);
    delete_light_list(&lights);
    free(pixels);
    printf("Done!\n");
    printf("Execution time of raytracing() : %lf sec\n", diff_in_second(start, end));
    return 0;
}
void setStruct(thread_args **threadS,int n,uint8_t *pixels, color background_color,
                rectangular_node rectangulars, sphere_node spheres,
                light_node lights, const viewpoint *view,int width,int height){
    for (int i = 0; i < n; i++)
    {
         threadS[i] = (thread_args *)malloc(sizeof (thread_args));
        if (threadS[i] !=NULL)
        {
               if (i==0){
                    threadS[i] ->  startwidth = 0;
                    threadS[i] ->  endwidth = ROWS/2;
                    threadS[i] ->  startheight = 0;
                    threadS[i] -> endheight = COLS/2;
                }
                if (i==1){
                    threadS[i] ->  startwidth = 0;
                    threadS[i] ->  endwidth = ROWS/2;
                    threadS[i] ->  startheight = COLS/2;
                    threadS[i] -> endheight = COLS;
                }
                if (i==2){
                    threadS[i] ->  startwidth = ROWS/2;
                    threadS[i] ->  endwidth = ROWS;
                    threadS[i] ->  startheight =0;
                    threadS[i] -> endheight = COLS/2;
                }
                if (i==3){
                   threadS[i] ->  startwidth = ROWS/2;
                    threadS[i] ->  endwidth = ROWS;
                    threadS[i] ->  startheight =COLS/2;
                    threadS[i] -> endheight = COLS;
                }
        } 
          threadS[i] -> pixels =pixels;
          memcpy( threadS[i] -> background_color,background_color,sizeof(color));
  
    threadS[i] -> rectangulars = rectangulars;
    threadS[i] -> spheres =spheres;
    threadS[i] -> lights =lights;
    threadS[i] -> view = view;
    
    threadS[i]  ->  width = ROWS;
    threadS[i] ->  height = COLS;       
    }
  
}
