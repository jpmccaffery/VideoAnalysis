#include "primaryfeature.h"
//#include "opencv/cv.h"
//#include "opencv/highgui.h"
//#include "opencv/cvaux.h"
//#include "opencv/cxcore.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"


PrimaryFeature::PrimaryFeature()
{
    height = 0;
    width = 0;

    hue = 0;
    saturation = 0;
    value = 0;

    blue = 0;
    green = 0;
    red = 0;

    x = 0;
    y = 0;

    box = cvRect(0,0,0,0);
}

void PrimaryFeature::draw(IplImage *image){


}

void PrimaryFeature::setBGR(){

    unsigned char *rowptr;
    IplImage *color, *colorout;
    CvSize *size = new CvSize();

    size->height = 1;
    size->width = 1;

    color = cvCreateImage(*size, IPL_DEPTH_8U, 3);
    colorout = cvCreateImage(*size, IPL_DEPTH_8U, 3);


    rowptr = ((unsigned char *) color->imageData);
    rowptr[0] = (unsigned char) hue;
    rowptr[1] = (unsigned char) saturation;
    rowptr[2] = (unsigned char) value;


    cvCvtColor(color, colorout, CV_HSV2BGR);
    rowptr = ((unsigned char *) colorout->imageData);

    blue = (double) rowptr[0];
    green = (double) rowptr[1];
    red = (double) rowptr[2];


    delete size;
    cvReleaseImage(&color);
    cvReleaseImage(&colorout);
}

void PrimaryFeature::setHSV(){

    unsigned char *rowptr;
    IplImage *color, *colorout;
    CvSize *size = new CvSize();

    size->height = 1;
    size->width = 1;

    color = cvCreateImage(*size, IPL_DEPTH_8U, 3);
    colorout = cvCreateImage(*size, IPL_DEPTH_8U, 3);


    rowptr = ((unsigned char *) color->imageData);
    rowptr[0] = (unsigned char) blue;
    rowptr[1] = (unsigned char) green;
    rowptr[2] = (unsigned char) red;


    cvCvtColor(color, colorout, CV_BGR2HSV);
    rowptr = ((unsigned char *) colorout->imageData);

    hue = (double) rowptr[0];
    saturation = (double) rowptr[1];
    value = (double) rowptr[2];


    delete size;
    cvReleaseImage(&color);
    cvReleaseImage(&colorout);
}

void PrimaryFeature::setRect(){


    box = cvRect(x, y, width, height);
}
