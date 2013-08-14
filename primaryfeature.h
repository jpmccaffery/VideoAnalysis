#ifndef PRIMARYFEATURE_H
#define PRIMARYFEATURE_H

//#include "opencv/cv.h"
#include "opencv2/core/core.hpp"

class PrimaryFeature
{
public:
    PrimaryFeature();
    void draw(IplImage *image);
    void setBGR();
    void setHSV();
    void setRect();

    int height;
    int width;

    double hue, saturation, value;
    double blue, green, red;

    int x;
    int y;

    CvRect box;

};

#endif // PRIMARYFEATURE_H
