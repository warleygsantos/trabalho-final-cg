#include "ColorSystem.h"

HSV rgb2hsv(RGB rgb)
{
    HSV hsv;
    double minimo, maximo, delta;

    minimo = (rgb.r < rgb.g) ? rgb.r : rgb.g;
    minimo = (minimo  < rgb.b) ? minimo  : rgb.b;

    maximo = (rgb.r > rgb.g) ? rgb.r : rgb.g;
    maximo = (maximo  > rgb.b) ? maximo  : rgb.b;

    hsv.v = maximo;
    delta = maximo - minimo;
    if(delta < 0.00001)
    {
        hsv.s = 0;
        hsv.h = 0;
        return hsv;
    }
    if(maximo > 0.0)
    {
        hsv.s = (delta / maximo);
    }
    else
    {
        hsv.s = 0.0;
        hsv.h = 0;
        return hsv;
    }
    if(rgb.r >= maximo)
    {
        hsv.h = (rgb.g - rgb.b) / delta;
    }
    else
    {
        if(rgb.g >= maximo)
        {
            hsv.h = 2.0 + (rgb.b - rgb.r) / delta;
        }
        else
        {
            hsv.h = 4.0 + (rgb.r - rgb.g) / delta;
        }
    }
    hsv.h *= 60.0;

    if(hsv.h < 0.0)
    {
        hsv.h += 360.0;
    }
    return hsv;
}

RGB hsv2rgb(HSV hsv)
{
    int i;
    RGB rgb;
    double hh, p, q, t, ff;

    if(hsv.s <= 0.0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    hh = hsv.h;
    if(hh >= 360.0)
    {
        hh = 0.0;
    }
    hh /= 60.0;
    i = hh;
    ff = hh - i;
    p = hsv.v * (1.0 - hsv.s);
    q = hsv.v * (1.0 - (hsv.s * ff));
    t = hsv.v * (1.0 - (hsv.s * (1.0 - ff)));

    switch(i)
    {
    case 0:
        rgb.r = hsv.v;
        rgb.g = t;
        rgb.b = p;
        break;
    case 1:
        rgb.r = q;
        rgb.g = hsv.v;
        rgb.b = p;
        break;
    case 2:
        rgb.r = p;
        rgb.g = hsv.v;
        rgb.b = t;
        break;

    case 3:
        rgb.r = p;
        rgb.g = q;
        rgb.b = hsv.v;
        break;
    case 4:
        rgb.r = t;
        rgb.g = p;
        rgb.b = hsv.v;
        break;
    case 5:
    default:
        rgb.r = hsv.v;
        rgb.g = p;
        rgb.b = q;
        break;
    }
    return rgb;
}
