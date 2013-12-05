/*
 * libutility1.hpp
 *
 *  Created on: Nov 3, 2013
 *      Author: Matteo Ragni
 */

#ifndef LIBUTILITY1_HPP_
#define LIBUTILITY1_HPP_

#include <iostream>
#include <opencv.hpp>

#define __DEBUG
//#define __DEBUGLIB

/* Namespaces */
using namespace cv;
using namespace std;

/* Define Macros */
#define ERROR(message) cout << "[Error]: " << message << endl;
#define INFO(message) cout << "[Info]: " << message << endl;

/* Window definitions */
#define WINDOW			"Frame"

/* Polylines definitions */
#define MAX_POLYLINE  	50
#define NPOINTS			4
#define POLYCOLOR 		Scalar(255,255,255)
#define POLYCLOSED		1
#define POLYSTYLE		1

/* Rect Definitions */
#define MAXRECT			50

/* Functions Definitions */
// Function for mouse handlind
void mouseHandler(int, int, int, int, void*);

// Functions polyline handling
void mouseClick(int, int, int);
void printLastPoly(int);
void drawLastPoly(int);

// Functions rectangle hadling
void mouseRect(int, int, int);
void printLastRect(int, int, int, int);
void drawLastRect(int);
Rect Rectangle(int, Point, Point);

// Video Function
void printCapProperties(VideoCapture);

extern int clickNum;
extern int clickNumR;
extern int polyCount;
extern int rectCount;
extern Point curve[MAX_POLYLINE][NPOINTS];
extern Point RectPoint[MAXRECT][2];
extern Rect vectRect[MAXRECT];
extern Mat Frame;


#endif /* LIBUTILITY1_HPP_ */
