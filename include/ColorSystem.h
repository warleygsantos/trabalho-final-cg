#ifndef COLORSYSTEM_INCLUDED
#define COLORSYSTEM_INCLUDED

typedef struct RGB
{
    int r;
    int g;
    int b;
} RGB;

typedef struct HSV
{
    double h;
    double s;
    double v;
} HSV;


HSV rgb2hsv(RGB);

RGB hsv2rgb(HSV);

#endif // COLORSYSTEM_INCLUDED
