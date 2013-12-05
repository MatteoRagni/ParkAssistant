/*
 * main.hpp
 *
 *  Created on: Nov 4, 2013
 *      Author: Matteo Ragni
 */

#ifndef MAIN_HPP_
#define MAIN_HPP_

#include "libParkAssist.hpp"

/* DEFINITIONS */
#define FDHISTORY 50
#define FDNGAUS   3
#define FDTVALUE  0.1
#define ALPHA 	  0.01

/* GLOBAL VARIABLES */
parkSpot parkingSpot[MAXPARKSPOT];
int nParkingSpot;



Mat ICbuffer[FDFRAME];
Mat ICbufferGr[FDFRAME];

vector<ParkSpotObj> parkingSpotObj;

Mat HistogramsWall(HST_WALL_ROWS*HST_PLOT_W, HST_WALL_ROWS*HST_PLOT_H, CV_8UC3, SCLBLACK);

Mat FD;

void HistogramCycle(int);
void FDCycle(int);

BackgroundSubtractorMOG mog(FDHISTORY, FDNGAUS, FDTVALUE);

#endif /* MAIN_HPP_ */
