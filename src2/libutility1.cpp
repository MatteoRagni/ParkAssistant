/*
 * libutility1.cpp
 *
 *  Created on: Nov 3, 2013
 *      Author: Matteo Ragni
 */

#include "libutility1.hpp"


/* La funzione mouse handler ha due funzioni:
 * - nel caso di click con il tasto sisnistro permette di disegnare un poligono
 *   di lati definiti in NPOINTS, e generare il codice che definirà quei poligoni.
 * - nel caso di click con il tasto destro la funzione permette di disegnare un rettangolo
 *   nella area di parcheggio.
 */

void mouseHandler(int event, int x, int y, int flags, void* ptr) {

	// Click del tasto sinistro
	if( event == CV_EVENT_LBUTTONDOWN) {

		#ifdef __DEBUGLIB
			INFO("Lclick:" << x << "x" << y << " - " << clickNum);
		#endif

		if (clickNum == (NPOINTS-1)) {
			curve[polyCount][clickNum] = Point(x,y);
			printLastPoly(polyCount);
			drawLastPoly(polyCount);
			polyCount++;
			clickNum = 0;
		}
		else {
			curve[polyCount][clickNum] = Point(x,y);
			clickNum++;
		}
	}

	// Click del tasto destro
	if( event == CV_EVENT_RBUTTONDOWN ) {

		#ifdef __DEBUGLIB
			INFO("Lclick:" << x << "x" << y << " - " << clickNum);
		#endif

		if (clickNumR == 1) {
			RectPoint[rectCount][clickNumR] = Point(x,y);
			vectRect[rectCount] = Rectangle(rectCount, RectPoint[rectCount][0], RectPoint[rectCount][1]);
			drawLastRect(rectCount);
			rectCount++;
			clickNumR = 0;
		} else {
			RectPoint[rectCount][clickNumR] = Point(x,y);
			clickNumR++;
		}
	}
}

/* Funzioni per il disegno delle polylines
 * ci sono anche le funzioni che provvedono alla stampa a schermo del
 * poligono
 */
void printLastPoly(int npoly) {
	cout << "% Polyline no:" << polyCount << endl;
	cout << "polyx:[";
	for (int i=0; i<NPOINTS; i++) {
		cout << curve[npoly][i].x;
		if (i<NPOINTS-1) { cout << ", "; }
	}
	cout << "], ";
	cout << "polyy:[";
		for (int i=0; i<NPOINTS; i++) {
			cout << curve[npoly][i].y;
			if (i<NPOINTS-1) { cout << ", "; }
		}
	cout << "]" << endl;
	cout << endl;
}

void drawLastPoly(int npoly) {
	const Point* curves[1] = {curve[npoly]};
	int curvesPts[] = {NPOINTS};
	polylines(Frame, curves, curvesPts, 1, (bool)POLYCLOSED, Scalar(255,255,255), POLYSTYLE );
	imshow(WINDOW,Frame);
}

/* Routines per la generazione e il print a schermo di rettangoli.
 * Si implementa anche una nuova funzione che genera i point a partire
 * da due
 */
void mouseRect(int nR, int x, int y) {

}

void printLastRect(int nR, int x, int y, int w, int h) {
	cout << "% Rectangle: " << nR <<endl;
	cout << "rect:[" << x << ", " << y << ", " << w << ", " << h << "]" << endl;
}

void drawLastRect(int nR) {
	rectangle(Frame,vectRect[nR],Scalar(255,0,0),POLYSTYLE);
	imshow(WINDOW,Frame);
}

Rect Rectangle(int nR, Point p1, Point p2) {
	int x, y, w, h;
	if (p1.x >= p2.x) {
		 x = p2.x;
		 w = p1.x - p2.x;
	} else {
		x = p1.x;
		w = p2.x - p1.x;
	}
	if (p1.y >= p2.y) {
		y = p2.y;
		h = p1.y - p2.y;
	} else {
		y = p1.y;
		h = p2.y - p1.y;
	}
	printLastRect(nR, x, y, w, h);

	return Rect(x,y,w,h);
}

/* Video capture: print property
 *
 */
void printCapProperties(VideoCapture cap){

	cout << "FILE PROPERTIES" << endl << endl;
	cout << "Dimensions: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << "x" << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
	cout << "Tot Frames: " << cap.get(CV_CAP_PROP_FRAME_COUNT) << endl;
	cout << "FPS:        " << cap.get(CV_CAP_PROP_FPS) << endl;
	cout << "Times:      " << cap.get(CV_CAP_PROP_POS_MSEC) << endl;
	cout << "Rel. pos:   " << cap.get(CV_CAP_PROP_POS_AVI_RATIO) << endl;
	cout << endl;
}
