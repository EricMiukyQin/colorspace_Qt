#include "colortype.h"

hsv rgb2hsv(rgb_double in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0;                              // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) {                           // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

rgb_double hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb_double         out;

    if(in.s <= 0.0) {                            // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = long(hh);
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;
    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

yuv rgb2yuv(rgb_int in)
{
    yuv out, temp;
    temp.y = (double(in.r) * 0.299000 + double(in.g) * 0.587000 + double(in.b) * 0.114000) / 255;
    temp.u = (double(in.r) * (-0.168736) + double(in.g) * (-0.331264) + double(in.b) * (0.500000) + 128) / 255 - 0.5;
    temp.v = (double(in.r) * 0.500000 + double(in.g) * (-0.418688) + double(in.b) * (-0.081312) + 128) / 255 - 0.5;
    out = checkyuv(temp);
    return out;
}

rgb_float yuv2rgb(yuv in)
{
    rgb_int temp1, temp2;
    rgb_float out;
    temp1.r = int((in.y * 255) + 1.4075 * ((255 * (in.v + 0.5)) - 128));
    temp1.g = int((in.y * 255) - 0.3455 * ((255 * (in.u + 0.5)) - 128) - (0.7169 * ((255 * (in.v + 0.5)) - 128)));
    temp1.b = int((in.y * 255) + 1.7790 * ((255 * (in.u + 0.5)) - 128));
    temp2 = checkrgb(temp1);
    out.r = float(temp2.r) / 255;
    out.g = float(temp2.g) / 255;
    out.b = float(temp2.b) / 255;
    return out;
}

yuv checkyuv(yuv in){
    yuv out;
    if(in.y < 0){
        out.y = 0;
    }else if(in.y > 1){
        out.y = 1;
    }else{
        out.y = in.y;
    }
    if(in.u < -0.5){
        out.u = -0.5;
    }else if(in.u > 0.5){
        out.u = 0.5;
    }else{
        out.u = in.u;
    }
    if(in.v < -0.5){
        out.v = -0.5;
    }else if(in.v > 0.5){
        out.v = 0.5;
    }else{
        out.v = in.v;
    }
    return out;
}

rgb_int checkrgb(rgb_int in){
    rgb_int out;
    if(in.r < 0)  {out.r = 0;}
    else if(in.r > 255)  {out.r = 255;}
    else  {out.r = in.r;}
    if(in.g < 0)  {out.g = 0;}
    else if(in.g > 255)  {out.g = 255;}
    else  {out.g = in.g;}
    if(in.b < 0)  {out.b = 0;}
    else if(in.b > 255)  {out.b = 255;}
    else  {out.b = in.b;}
    return out;
}
