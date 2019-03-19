#ifndef COLORTYPE_H
#define COLORTYPE_H

#include <QGLWidget>


/*********  Definition of structures  *********/

// RGB
typedef struct {
        int r;          // [0 255]
        int g;          // [0 255]
        int b;          // [0 255]
} rgb_int;

typedef struct {
        double r;       // [0 1]
        double g;       // [0 1]
        double b;       // [0 1]
} rgb_double;

typedef struct {
        float r;        // [0 1]
        float g;        // [0 1]
        float b;        // [0 1]
} rgb_float;

// HSV
typedef struct {
        double h;       // [0 360] (angles)
        double s;       // [0 1]
        double v;       // [0 1]
} hsv;

// YUV
typedef struct {
        double y;       // [0 1]
        double u;       // [-0.5 0.5]
        double v;       // [-0.5 0.5]
} yuv;

/*********  Colorspace transfer functions  *********/

hsv rgb2hsv(rgb_double in);
rgb_double hsv2rgb(hsv in);
yuv rgb2yuv(rgb_int in);
rgb_float yuv2rgb(yuv in);
yuv checkyuv(yuv in);
rgb_int checkrgb(rgb_int in);

#endif // COLORTYPE_H
