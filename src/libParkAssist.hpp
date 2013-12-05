/*
 * libParkAssist.hpp
 *
 *  Created on: Nov 10, 2013
 *      Author: Matteo Ragni
 */

#ifndef LIBPARKASSIST_HPP_
#define LIBPARKASSIST_HPP_

/* INCLUSIONS */

#include <iostream>
#include <opencv.hpp>
#include <time.h>
//#include <string>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

/* DEBUG DEFINITIONS */

#define __DEBUG

#ifdef __DEBUG
	#define 		ERROR(message) 	cout << "[ERROR] " << message << endl;
	#define 		INFO(message) 	cout << "[INFO] " << message << endl;
	#define			DEBUG(message)	cout << "[DEBUG] " << message << endl;
#endif
#ifndef __DEBUG
	#define 		ERROR(message)
	#define 		INFO(message)
	#define			DEBUG(message)
#endif

/* DEFINITIONS */

#define MAXPOLY		4
#define TRUE 		1
#define FALSE 		0
#define MAXPARKSPOT	25
#define ERRORCODE	9999
#define PARAMETERS  5

#define PARKFREE	1
#define PARKBUSY   -1
#define PARKWAIT	0
#define PARKTS		5

#define FDFRAME		3

#define CV_CONVERSION	CV_BGR2HSV

// Hist. Calc options
#define HISTCOMP	3
#define HST_SIZE	32
#define HST_RANGE	256
#define HST_UNIFORM	true	// bin size costante
#define HST_ACC		false	// istogramma ripulito prima di calcolare
#define HST_PLOT_W	150
#define HST_PLOT_H  HST_PLOT_W

#define HST_WALL_ROWS 5
#define HST_WALL_COLS 5

// Colors
#define SCLBLACK	Scalar(0,0,0)
#define SCLWHITE	Scalar(255,255,255)
#define SCLBLUE		Scalar(255,0,0)
#define SCLGREEN	Scalar(0,255,0)
#define SCLRED		Scalar(0,0,255)
#define SCLYELLOW	Scalar(0,255,255)

/* TYPEDEF */

typedef struct {

	int nspot;

	Point rectPoint;
	int rectDim[2];

	Point PolyPoint[MAXPOLY];

	int parameters[PARAMETERS];

} parkSpot;

/* Global Variables */
extern parkSpot parkingSpot[MAXPARKSPOT];
extern int nParkingSpot;
extern Mat ICbuffer[FDFRAME];
extern Mat HistogramsWall;

/* FUNCTION DEFINITION */

// Proprietà del video
void printCapProperties(VideoCapture);

// Funzione che si occupa della lettura del file di configurazione YML
// Ritorna vero se la lettura non ha dato problemi
// Ritorna falso se la lettura ha dato problemi (sintassi, etc.)
bool readConfFile(char*);

//Funzione che si occupa della lettura specifica di un elemento del file
// di configurazione
// Ritorna una struttura di tipo parkSpot
parkSpot readParkSpot(FileNode);

// Controlla che un filenode di tipo int. Ritorna true se vero,
// ritorna false se false. Se non lo è stampa un messaggio in console
bool checkIntSyntax(FileNode, const char*);

// Stampa tutte le variabili in parking spot
void printSpot(parkSpot);

// Funzione che approssima alla cifra specificata un float
float round2(float, int);

/* CLASS */
// Classe che si occupa del caclolo e della gestione dell'istogramma:
class Histogram {

	public:
		Mat SourceImage;     	// Immagine originale
		vector<Mat> plane; 		// Componenti della immagine:
		Mat hist[HISTCOMP];		// Componenti dell'istogramma;
		Mat histSum;         	// Immagine dell'istogramma

		float means[HISTCOMP];
		float stddev[HISTCOMP];

		// costruttore della classe
		Histogram(Mat);
		Histogram();

		// Funzione che si occupa dello split e del calcolo dei singoli elementi dell'istogramma
		void evalHist();
		// Funzione che si occupa dell'aggiornamento e del ricalcolo degli istogrammi
		void refresh();
		void refresh(Mat);
		// Funzione che si occupa della generazione della nuova immagine normalizzata contenente
		// tutti i piani dell'istogramma
		void refreshHistSum();
		// Funzione che calcola le medie di un istogramma e la sua deviazione standard
		void evalValues();

		// Funzione che si occupa della stampa a schermo della immagine
		// con la stringa che punta alla window che si occuperà della apertura
		void plot(char*);
		// Funzione che plotta a schermo lo status degli istogram
		void print();

};

// classe che rappresenta un parking spot. Punto cardinale dell'intero
// oggetto risulta essere la variabile parkRect che rappresenta la porzione
// della immagine da analizzare.
class ParkSpotObj {
	public:
		//parkSpot inputStruct;
		Mat buffer[FDFRAME];

		Rect ParkRect;

		Point warpedDim;
		Mat warpingMat;
		vector<Point2f> PolyPoints2f;
		vector<Point2f> warpingPts;

		Mat fdbuffer;			// Immagine utilizzata per il frame buffering
		Mat edgebuffer;

		vector<Point> PolyPoints;

		vector<int> parameters;

		int status, nspot, fd, edge;
		int counter;
		Histogram hist;
		Histogram hist_old;

		// Costruttore di ParkSpotCl. Si becce in ingresso due puntatori alle struct
		// del posto di parcheggio e tre matrici di immagini per poter formare il buffer iniziale
		ParkSpotObj(parkSpot);
		// Distruttore della classe, credo che mi vada bene uno standard
		// per adesso, poi vedo se ho tempo di ottimizzare.

		// Funzione che calcola la condizione iniziale del posto auto. Ritorna
		// uno dei codici specifici!
		int initialStatus();
		// Funzione che estra la Mat del posto di parcheggio a partire
		// dall'ultimo frame;
		void refreshImage();
		// Funzione che si occupa di traslare i buffer di uno indietro
		// per poter fare il frame differencing tra tre frame
		void shiftBuffers();
		// Funzione che pensa alla rappresentazione a schermo del poligono chiuso
		// colorato che rappresenta lo status del parcheggio. Si prende come
		// valore in ingresso il nome della finestra nella quale è rappresentata l'immagine
		void plotPolyFill(Mat);
		// Funzione che plotta il rettangolo di interesse del parcheggio
		void plotRectangle(Mat);

		// Funzione che si occupa del refresh delle classi Histograms
		void refreshHist();

		// Funzione che si occupa del sorting dei corners della polyline in modo tale da averli
		// ordinati dal top-left al bottom-right
		void sortCorners();
		// Ritorna un punto che rappresenta le dimensioni della immagine che deve essere scalata
		void getWarpedDimension();

		// Funzione che si occupa del refresh della immagine di frame difference
		void refreshImagefd(Mat);
		// Funzione che si occupa del refresh della immagine di edge detection
		void refreshImageEdge(Mat);

		// Funzione che si occupa del plot del numero del parcheggio nella immagine
		// passata come input
		void plotParkNum(Mat);

		// Funziona che mostra tutto lo stato interno della variabile.
		void show();


};

// funzione molto particolare che si occupa del print di un wall di histograms
void histogramsWallClear();
void histogramsWall(int, Histogram);

#endif /* LIBPARKASSIST_HPP_ */
