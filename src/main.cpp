/*
 * main.cpp
 *
 *  Created on: Nov 2, 2013
 *      Author: Matteo Ragni
 */

#include "main.hpp"
#include <fstream>

int main(int argc, char** argv){

	/* Acquisizione dei valori dalla riga di comando */
	if (argc < 2) {
		cout << "ParkAssistant configuration_file.yml video.avi [fast_forward_scale - mandatory]" << endl;
		return 1;
	}
	char* confPath = argv[1];
	char* videoPath = argv[2];
	int ffwd;
	if (argc == 3) {
		ffwd = 1;
	} else {
		istringstream(argv[3]) >> ffwd;
		if (!(ffwd > 1)) ffwd = 1;
 	}

	// Lettura del file di configurazione:
	readConfFile(confPath);


	VideoCapture cap;
	cap.open(videoPath);

	// Impostazioni del video e stampa del sistema di dati:
	printCapProperties(cap);

	for (int i = 0; i < FDFRAME; i++) {
		cap >> ICbuffer[FDFRAME-(i+1)];
		cvtColor(ICbuffer[FDFRAME-(i+1)], ICbufferGr[FDFRAME-(i+1)], CV_BGR2GRAY);
	}

	FD = Mat(ICbuffer[0].rows, ICbuffer[0].cols, CV_8UC1, cv::Scalar(0));

	namedWindow("Window",1);

	// CANNY EDGE DETECTOR
	Mat detectedge;
	blur(ICbufferGr[0], detectedge, Size(3,3));
	Canny( detectedge, detectedge, 25, 110, 3 );

	/* Generazione degli oggetti parking spot */
	for (int i = 0; i < nParkingSpot; i++) {
		// Generazione dei parking spot
		ParkSpotObj tmpParkSpotObj = ParkSpotObj(parkingSpot[i]);
		parkingSpotObj.push_back(tmpParkSpotObj);

		// Disegno sulla immagine i parking spot
		parkingSpotObj[i].plotPolyFill(ICbuffer[0]);

		// Printo il numero del parcheggio per comodità
		parkingSpotObj[i].plotParkNum(ICbuffer[0]);

		histogramsWall(i, parkingSpotObj[i].hist);
	}

	imshow("EDGE", detectedge);


	for (int i = 0; i < nParkingSpot; i++) {
		// Eseguo l'edge detection per ogni posto auto
		parkingSpotObj[i].refreshImageEdge(detectedge);
	}

	imshow("Window", ICbuffer[0]);
	imshow("Hist", HistogramsWall);
	histogramsWallClear();

	waitKey(10000);

	/* Ciclo video */

	int parkfree = 0;
	int parkbusy = 0;
	int parkmove = 0;
	char message[60];

	cout << endl << "FRAME = {";
	// Traslazione di un frame dell ICbuffer
	for (int i = FDFRAME; i < cap.get(CV_CAP_PROP_FRAME_COUNT); i = i + ffwd) {

		parkfree = 0;
		parkbusy = 0;
		parkmove = 0;

		// Frame Shifter
		for (int j = 1; j < (FDFRAME-1); j++) {
			ICbuffer[FDFRAME-j] = ICbuffer[FDFRAME-(j+1)];
		}

		cap.set(CV_CAP_PROP_POS_FRAMES, i);
		cap >> ICbuffer[0];
		cvtColor(ICbuffer[0], ICbufferGr[0], CV_BGR2GRAY);

		// EDGE DETECTION
		blur(ICbufferGr[0], detectedge, Size(3,3));
		Canny( detectedge, detectedge, 25, 110, 3 );


		for (int j = 0; j < nParkingSpot; j++) {

			HistogramCycle(j);

			// Eseguo l'ege detection per ogni posto auto
			if( parkingSpotObj[j].status == PARKWAIT) {

				parkingSpotObj[j].refreshImageEdge(detectedge);
				if( parkingSpotObj[j].edge > parkingSpotObj[j].parameters[1]) {
					parkingSpotObj[j].status = PARKBUSY;
				} else  {
					parkingSpotObj[j].status = PARKFREE;
				}
			}

			FDCycle(j);

			parkingSpotObj[j].plotPolyFill(ICbuffer[0]);
			parkingSpotObj[j].plotParkNum(ICbuffer[0]);

			switch(parkingSpotObj[j].status) {
			case PARKFREE:
				parkfree++; break;
			case PARKBUSY:
				parkbusy++; break;
			case PARKWAIT:
				parkmove++; break;
			}

		}

		sprintf(message, "FREE: %2d - BUSY: %2d - WAIT: %2d", parkfree, parkbusy, parkmove);
		putText(ICbuffer[0], message, Point(10,40), FONT_HERSHEY_COMPLEX_SMALL, 0.8, SCLBLACK, 1, CV_AA);

		// Display delle immagini
		imshow("Hist", HistogramsWall);
		imshow("Window", ICbuffer[0]);
		imshow("EDGE", detectedge);
		waitKey(10);

		histogramsWallClear();
		//imwrite("ICbuffer.png",ICbuffer[0]);


	}
	cout << "};";
	return 0;
}

void HistogramCycle(int j) {
	parkingSpotObj[j].refreshImage();
	parkingSpotObj[j].hist_old = parkingSpotObj[j].hist;
	parkingSpotObj[j].refreshHist();
	histogramsWall(j, parkingSpotObj[j].hist);
}

void FDCycle(int h) {
	if (h == 0) {
		mog(ICbuffer[0], FD, ALPHA);
	}
	parkingSpotObj[h].refreshImagefd(FD);
}



