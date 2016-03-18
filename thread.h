 #ifndef __THREAD_H
#define __THREAD_H
#include "primitives.h"
#include "objects.h"
 typedef struct  {
    uint8_t *pixels;
    color background_color;
    rectangular_node rectangulars;
    sphere_node spheres;
    light_node lights;
    const viewpoint *view;
    int startwidth;
    int endwidth;
    int startheight;
    int endheight;
    int  width;
    int height;
} thread_args;
#endif