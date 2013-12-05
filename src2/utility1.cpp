/*
 * utility1.cpp
 *
 *  Created on: Nov 3, 2013
 *      Author: Matteo Ragni
 */

#include "libutility1.hpp"

// ### MAIN ###

// Global variables
int clickNum = 0;
int clickNumR = 0;
int polyCount = 0;
int rectCount = 0;
Point curve[MAX_POLYLINE][NPOINTS];
Point RectPoint[MAXRECT][2];
Rect vectRect[MAXRECT];
Mat Frame;

int main(int argc, char** argv) {

	char* videoPath = argv[1];
	int nFrame = 0;
	//Mat Frame;
	istringstream(argv[2]) >> nFrame;

	VideoCapture cap;
	cap.open(videoPath);

	// Check sull'ingresso.
	if (argc != 3 || !cap.isOpened()) {
		ERROR("Cannot open video stream!");
		INFO("Use:" << endl << "utility1 [video path/video.avi] [nFrame] [option]");
		#ifdef __DEBUG
			if (argc > 1) {
				ERROR(argv[1] << "@" << argv[2]);
				ERROR(argc);
			} else {
				ERROR(argc);
			}
		#endif
		return 1;
	}

	printCapProperties(cap);

	// Setta il numero di frame specificato
//	for (int i=0; i < nFrame; i++) { cap.grab(); }
//	cap >> Frame;
	cap.set(CV_CAP_PROP_POS_FRAMES, nFrame);
	cap >> Frame;

	namedWindow(WINDOW,1); // Genera una nuova finestra per la visualizzazione
	cvSetMouseCallback(WINDOW, mouseHandler, NULL);
	imshow(WINDOW, Frame);
	waitKey(0);
}

