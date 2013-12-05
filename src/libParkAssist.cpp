/*
 * libParkAssist.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: Matteo Ragni
 */


#include "libParkAssist.hpp"

void printCapProperties(VideoCapture cap){

	cout << "┌─────────────────┐ " << endl;
	cout << "│ FILE PROPERTIES │" << endl;
	cout << "└──┬──────────────┘ " << endl;
	cout << "   │            " << endl;
	cout << "   ├─> Dimensions:  " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << "x" << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
	cout << "   ├─> Tot Frames:  " << cap.get(CV_CAP_PROP_FRAME_COUNT) << endl;
	cout << "   ├─> FPS:         " << cap.get(CV_CAP_PROP_FPS) << endl;
	cout << "   ├─> Times:       " << cap.get(CV_CAP_PROP_POS_MSEC) << endl;
	cout << "   ├─> Rel. pos:    " << cap.get(CV_CAP_PROP_POS_AVI_RATIO) << endl;
	cout << "   ├─> Convert RGB: " << cap.get(CV_CAP_PROP_CONVERT_RGB) << endl;
	cout << "   ├─> Format:      " << cap.get(CV_CAP_PROP_FORMAT) << endl;
	cout << "   ├─> Mode:        " << cap.get(CV_CAP_PROP_MODE) << endl;
	cout << "   └─> Codec Code:  " << cap.get(CV_CAP_PROP_FOURCC) << endl;
	cout << endl;
}

bool readConfFile(char* filePath){

	// Apertura del file di configurazione:
	INFO("[LIBHW] Opening: " << filePath);
	FileStorage fs(filePath, FileStorage::READ);

	if (!fs.isOpened()) {
		cout << "[LIBHW] Cannot open file:" << filePath << endl;
		return FALSE;
	}

	fs["parking_spot"] >> nParkingSpot;
	INFO("[LIBHW] Identified: " << nParkingSpot << " parking spots");

	FileNode spots = fs["spots"];
		FileNodeIterator node       = spots.begin();
		FileNodeIterator node_end   = spots.end();

	for (int idx = 0; node != node_end; ++node, idx++) {
		parkingSpot[idx] = readParkSpot(*node);
		if (parkingSpot[idx].nspot == ERRORCODE) {
			cout << "[LIBHW] Cannot read spot:" << idx << endl;
			return FALSE;
		}
	}

	// Chiusura del file di configurazione
	fs.release();
	return TRUE;
}

parkSpot readParkSpot(FileNode spot) {

	parkSpot tmp;
	std::vector<int> rect;
	std::vector<int> xpoly;
	std::vector<int> ypoly;
	std::vector<int> param;

	// Syntax check
	if(!checkIntSyntax(spot,"nspot")) { tmp.nspot = ERRORCODE; return tmp;}
	tmp.nspot = (int)spot["nspot"] - 1;

	spot["rect"] >> rect;
	spot["polyx"] >> xpoly;
	spot["polyy"] >> ypoly;
	spot["param"] >> param;

	// Assigning polyline points
	for (int i = 0; i < (int)xpoly.size(); i++) {
		tmp.PolyPoint[i] = Point((int)xpoly[i],(int)ypoly[i]);
	}
	// Assigning parameters
	for (int i = 0; i < (int)param.size(); i++) {
		tmp.parameters[i] = param[i];
	}

	// Assigning rectangular values
	tmp.rectPoint = Point((int)rect[0],(int)rect[1]);
	tmp.rectDim[0] = (int)rect[2];
	tmp.rectDim[1] = (int)rect[3];

	return tmp;
}

bool checkIntSyntax(FileNode fn, const char* node) {
	if (fn[node].type() != FileNode::INT) {
		cout << "[LIBHW] Spot syntax error: " << node << endl;
		return FALSE;
	} else {
		return TRUE;
	}
}

void printSpot(parkSpot spot) {

	cout << endl;
	cout << "PARKING SPOT: " << spot.nspot << endl;
	cout << endl;
	cout << "Parameters:" << endl;
	for(int i = 0; i < PARAMETERS; i++) {
		cout << "[" << i << "]: " << spot.parameters[i] << "  ";
	} cout << endl;
	cout << endl;
	cout << "Rectangle: P = (" << spot.rectPoint.x << "," << spot.rectPoint.y << ") ";
	cout <<            "S = (" << spot.rectDim[0] << "," << spot.rectDim[1] << ")" << endl;
	cout << "Polyline: ";
	for (int i = 0; i < MAXPOLY; i++ ) {
		cout << "P" << i << " = (";
		cout << spot.PolyPoint[i].x << "," << spot.PolyPoint[i].y << "), ";
	}
	cout << endl;

}

float round2(float num, int dec) {
	float num_c = num;
	float mult = 1;
	for(int i = 0; i < dec; i++ ) { mult *= 10; }

	return cvRound(num_c*mult)/mult;
}

void histogramsWallClear() {
	Mat(HST_PLOT_W * HST_WALL_ROWS, HST_PLOT_H * HST_WALL_COLS, CV_8UC3, SCLBLACK).copyTo(HistogramsWall);
}

void histogramsWall(int pos, Histogram input) {

	int rows = cvRound(pos / HST_WALL_ROWS);
	int cols = pos - (rows * HST_WALL_COLS);

	Mat tmpInput = Mat();
	Mat tmpRect = Mat(HST_PLOT_W, HST_PLOT_H, CV_8UC3, SCLBLACK);
	input.histSum.copyTo(tmpInput);
	// Creating the position of interest
	Rect ROI = Rect(rows * HST_PLOT_H, cols * HST_PLOT_W, HST_PLOT_W, HST_PLOT_H);

	// Aggiunta delle scritte: medie e deviazioni standard
	char position[5];
	char means[25];
	char stddevs[25];

	sprintf(position, "%d", pos);
	sprintf(means, "M: %.2f %.2f %.2f", input.means[0], input.means[1], input.means[2]);
	sprintf(stddevs, "S: %.2f %.2f %.2f", input.stddev[0], input.stddev[1], input.stddev[2]);

	putText(tmpInput, position, Point(10,20), FONT_HERSHEY_COMPLEX_SMALL, 0.5, SCLWHITE, 1, CV_AA);
	putText(tmpInput, means, Point(10,40), FONT_HERSHEY_COMPLEX_SMALL, 0.5, SCLWHITE, 1, CV_AA);
	putText(tmpInput, stddevs, Point(10,60), FONT_HERSHEY_COMPLEX_SMALL, 0.5, SCLWHITE, 1, CV_AA);

	tmpInput.copyTo(HistogramsWall(ROI));

}

/**************************/
// IMPLEMENTAZIONE CLASSE //
/**************************/

/*******************/
// HISTOGRAM CLASS //
/*******************/
Histogram::Histogram(Mat Frame) {

	this->histSum = Mat(HST_PLOT_H, HST_PLOT_W, CV_8UC3, SCLBLACK);
	this->refresh(Frame);
}

Histogram::Histogram() {
	for (int i = 0; i < HISTCOMP; i++) {
		this->hist[i] = Mat();
		this->plane.push_back(Mat::ones(1,1,CV_8UC3));
	}
}

void Histogram::refresh(Mat Frame){
	this->SourceImage = Frame.clone();
	this->refresh();
}

void Histogram::refresh(){

	int histSize = HST_SIZE; // numero di Bins = 32
	float hist_range[] = { 0, HST_RANGE };
	const float* histRange = { hist_range };

	Mat TmpPlane;
	Mat TmpHist;

	split(this->SourceImage, this->plane);

	for(int i = 0; i < HISTCOMP; i++) {
		this->plane[i].copyTo(TmpPlane);
		calcHist( &(TmpPlane), 1, 0, Mat(), TmpHist, 1, &histSize, &histRange, HST_UNIFORM, HST_ACC);
		TmpHist.copyTo(this->hist[i]);
	}
	this->refreshHistSum();
	this->evalValues();

	//this->print();

}

void Histogram::refreshHistSum(){

	Scalar Color[HISTCOMP] = { SCLBLUE, SCLGREEN, SCLRED };

	int hist_w = HST_PLOT_W;
	int hist_h = HST_PLOT_H;
	int bin_w = cvRound( (double) hist_w/HST_SIZE );

	Mat TmpMat( HST_PLOT_H, HST_PLOT_W, CV_8UC3, SCLBLACK );
	Mat TmpHist;

	for (int j = 0; j < HISTCOMP; j++) {
		(this->hist[j]).copyTo(TmpHist);
		normalize(TmpHist, TmpHist, 0, HST_PLOT_H, NORM_MINMAX, -1, Mat() );
		for (int i = 0; i < HST_SIZE; i++) {
			line( TmpMat, Point( bin_w*(i-1), hist_h - cvRound(TmpHist.at<float>(i-1)) ) , \
					      Point( bin_w*(i), hist_h - cvRound(TmpHist.at<float>(i)) ),      \
		                  Color[j], 2, 8, 0  );
		}
	}
	TmpMat.copyTo(this->histSum);
}

void Histogram::plot(char* window) {
	namedWindow(window,CV_WINDOW_KEEPRATIO | CV_WINDOW_NORMAL);
	resizeWindow(window, HST_PLOT_W, HST_PLOT_H);
	imshow(window, this->histSum);
}

void Histogram::print(){

	cout << "┌───────────┐         " << endl;
	cout << "│ HISTOGRAM │         " << endl;
	cout << "└──┬────────┘         " << endl;
	cout << "   │          	       " << endl;
	cout << "   ├─> Dimensions:    ";
	for (int i= 0; i < HISTCOMP; i++) { cout << " [" << i << "] " << this->plane[i].rows << "x" << this->plane[i].cols;} cout << endl;
	cout << "   ├─> Dimensions:    ";
	for (int i= 0; i < HISTCOMP; i++) { cout << " [" << i << "] " << this->hist[i].rows << "x" << this->hist[i].cols;} cout << endl;
	cout << "   ├─> Dimensions:    " << " [SUM] " << this->histSum.rows << "x" << this->histSum.cols << endl;
	cout << "   │          		   " << endl;
	cout << "   ├─> Tot Comps:     ";
	for (int i= 0; i < HISTCOMP; i++) { cout << " [" << i << "] " << this->hist[i].total(); } cout << endl;
	cout << "   ├─> Sum Tot Comps: " << " [SUM] " << this->histSum.total() << endl;
	cout << "   │          		   " << endl;
	cout << "   ├─> Means:         ";
	for (int i= 0; i < HISTCOMP; i++) { cout << " [" << i << "] " << this->means[i]; } cout << endl;
	cout << "   └─> Std Dev²:      ";
	for (int i= 0; i < HISTCOMP; i++) { cout << " [" << i << "] " << this->stddev[i]; } cout << endl;
}

void Histogram::evalValues(){

	Mat tmpHist;
	Mat tmpMean;
	Mat tmpSD;

	merge(this->hist, HISTCOMP, tmpHist);

	meanStdDev(tmpHist, tmpMean, tmpSD, Mat());
	for (int i = 0; i < HISTCOMP; i++) {
		this->means[i] = round2(saturate_cast<float>(tmpMean.at<double>(i)),5);
		this->stddev[i] = round2(saturate_cast<float>(tmpSD.at<double>(i)),2);
	}
}

// PARKSPOTOBJ
ParkSpotObj::ParkSpotObj(parkSpot inputStruct){

	this->counter=0;
	this->nspot = inputStruct.nspot;

	// empty vectors
	this->parameters.clear();
	this->PolyPoints.clear();
	this->PolyPoints2f.clear();
	this->warpingPts.clear();

	for (int i = 0; i < PARAMETERS; i++){  this->parameters.push_back(inputStruct.parameters[i]); }
	for (int i = 0; i < MAXPOLY; i++){  this->PolyPoints.push_back(inputStruct.PolyPoint[i]); }
	// Align the polypoints vector and get the warped transformation matrix;
	this->sortCorners();
	this->getWarpedDimension();

	// Assign warpingPoints and PolyPoints2f;
	for (int i = 0; i < (int)this->PolyPoints.size(); i++) {
		this->PolyPoints2f.push_back(Point2f(saturate_cast<float>(this->PolyPoints[i].x), saturate_cast<float>(this->PolyPoints[i].y)));
	};

	this->warpingMat = getPerspectiveTransform(this->PolyPoints2f, this->warpingPts);

	// Generates the rectangle
	this->ParkRect = Rect(inputStruct.rectPoint.x, \
						  inputStruct.rectPoint.y, \
						  inputStruct.rectDim[0],  \
						  inputStruct.rectDim[1]);

	Mat tmpWarpSrc;
	Mat tmpWarpDst = Mat::zeros(this->warpedDim.x, this->warpedDim.y, CV_8UC3);
	tmpWarpDst.copyTo(this->fdbuffer);
	for (int i = 0; i < FDFRAME; i++) {
		ICbuffer[i].copyTo(tmpWarpSrc);
		warpPerspective(tmpWarpSrc, tmpWarpDst, this->warpingMat, tmpWarpDst.size());
		tmpWarpDst.copyTo(this->buffer[i]);
	}
	tmpWarpSrc.release();
	tmpWarpDst.release();

	this->status = PARKFREE;
	this->fd = 0;

	Mat tmpHstImgHSV;
	cvtColor(this->buffer[0], tmpHstImgHSV, CV_CONVERSION);
	this->hist = Histogram(tmpHstImgHSV);
	cvtColor(this->buffer[1], tmpHstImgHSV, CV_CONVERSION);
	this->hist_old = Histogram(tmpHstImgHSV);

	this->status = ParkSpotObj::initialStatus();

	this->show();
}


void ParkSpotObj::refreshImage(){
	this->shiftBuffers();
	//this->buffer[FDFRAME-1] = (*LastFrame)(this->ParkRect);
	Mat tmpWarpSrc;
	Mat tmpWarpDst = Mat::zeros(this->warpedDim.x, this->warpedDim.y, CV_8UC3);
	ICbuffer[0].copyTo(tmpWarpSrc);
	warpPerspective(tmpWarpSrc, tmpWarpDst, this->warpingMat, tmpWarpDst.size());
	tmpWarpDst.copyTo(this->buffer[0]);
	tmpWarpSrc.release();
	tmpWarpDst.release();
}

void ParkSpotObj::refreshImagefd(Mat input){

	Mat tmpWarpSrc;
	Mat tmpWarpDst = Mat::zeros(this->warpedDim.x, this->warpedDim.y, CV_8UC3);
	input.copyTo(tmpWarpSrc);
	warpPerspective(tmpWarpSrc, tmpWarpDst, this->warpingMat, tmpWarpDst.size());
	tmpWarpDst.copyTo(this->fdbuffer);
	tmpWarpSrc.release();
	tmpWarpDst.release();

	// Calcola la somma dell'fd e controlla che non superi la soglia param[0]
	this->fd = sum(this->fdbuffer)[0];
	if(this->fd > this->parameters[0]) { this->status = PARKWAIT; }
}

void ParkSpotObj::refreshImageEdge(Mat input){
	Mat tmpWarpSrc;
	Mat tmpWarpDst = Mat::zeros(this->warpedDim.x, this->warpedDim.y, CV_8UC3);
	input.copyTo(tmpWarpSrc);
	warpPerspective(tmpWarpSrc, tmpWarpDst, this->warpingMat, tmpWarpDst.size());
	tmpWarpDst.copyTo(this->edgebuffer);
	tmpWarpSrc.release();
	tmpWarpDst.release();

	// Calcola la somma dell'fd e controlla che non superi la soglia param[0]
	this->edge = sum(this->edgebuffer)[0];

}

void ParkSpotObj::shiftBuffers() {
	for (int i = FDFRAME - 2; i > -1; i--){
		this->buffer[i + 1] = this->buffer[i];
	}
}

void ParkSpotObj::refreshHist() {
	Mat tmpImage;
	cvtColor(this->buffer[0], tmpImage, CV_CONVERSION);
	this->hist.refresh(tmpImage);
	tmpImage.release();
}

int ParkSpotObj::initialStatus() {

	double alpha = (double)this->parameters[2] * 0.1;
	double trans = (double)this->parameters[3];

	/* Rotational Matrix
	 *
	 * R = [  cos(a)   sin(a)]
	 *     [ -sin(a)   cos(a)]
	 *
	 * R*V = [  V1*cos(a) + V2*sin(a) ]
	 *       [ -V1*sin(a) + V2*cos(a) ]
	 *
	 * where
	 * V2 = mu2 (mean of second chrominance component)
	 * V1 = sigma1 ( stdev of first chrominance component)
	 *
	 * Extract y V(2) -> -V1*sin(a) + V2*cos(a)
	 * Translate V(2) -> V(2) - Q
	 *
	 * Check sign of this last element
	 */

	double ts = (cos(alpha)*(double)this->hist.means[2] - sin(alpha)*(double)this->hist.stddev[1]) - (double)trans;
	return (ts >= 0 ? PARKBUSY : PARKFREE);

}

void ParkSpotObj::plotPolyFill(Mat Frame){

	Point tmpPoly[(int)this->PolyPoints.size()];

	for(int i = 0; i < (int)this->PolyPoints.size(); i++) {
		tmpPoly[i].x = this->PolyPoints[i].x;
		tmpPoly[i].y = this->PolyPoints[i].y;
	}

	const Point* curves[1] = {tmpPoly};
	int curvesPts[] = {MAXPOLY};
	Scalar color;
	switch(this->status) {
	case PARKFREE:
		color = SCLGREEN; break;
	case PARKBUSY:
		color = SCLRED; break;
	case PARKWAIT:
		color = SCLBLUE; break;
	}

	polylines(Frame, curves, curvesPts, 1, true, color, 1);
}

void ParkSpotObj::plotRectangle(Mat Frame){
	rectangle(Frame,this->ParkRect,SCLBLACK,2);
}

void ParkSpotObj::sortCorners() {

	vector<Point> top, bot;
	Point center;

	for (int i = 0; i < (int)this->PolyPoints.size(); i++) { center += this->PolyPoints[i]; }

	center *= (1. / this->PolyPoints.size());

	for (int i = 0; i < (int)this->PolyPoints.size(); i++)
	{
		if (this->PolyPoints[i].y < center.y)
			top.push_back(this->PolyPoints[i]);
		else
			bot.push_back(this->PolyPoints[i]);
	}

	Point tl = (top[0].x > top[1].x ? top[1] : top[0]);
	Point tr = (top[0].x > top[1].x ? top[0] : top[1]);
	Point bl = (bot[0].x > bot[1].x ? bot[1] : bot[0]);
	Point br = (bot[0].x > bot[1].x ? bot[0] : bot[1]);

	this->PolyPoints.clear();
	this->PolyPoints.push_back(tl);
	this->PolyPoints.push_back(tr);
	this->PolyPoints.push_back(br);
	this->PolyPoints.push_back(bl);
}

void ParkSpotObj::getWarpedDimension() {

	vector<int> xvec, yvec;
	vector<int> xdiff, ydiff;

	xvec.clear();
	yvec.clear();
	xdiff.clear();
	ydiff.clear();

	for (int i = 0; i < (int)this->PolyPoints.size(); i++) {
		xvec.push_back(this->PolyPoints[i].x);
		yvec.push_back(this->PolyPoints[i].y);
	}

	xdiff.push_back(xvec[1] - xvec[0]);
	xdiff.push_back(xvec[1] - xvec[3]);
	xdiff.push_back(xvec[2] - xvec[0]);
	xdiff.push_back(xvec[2] - xvec[3]);

	ydiff.push_back(yvec[3] - yvec[0]);
	ydiff.push_back(yvec[3] - yvec[1]);
	ydiff.push_back(yvec[2] - yvec[0]);
	ydiff.push_back(yvec[2] - yvec[1]);

	for (int i = 0; i < (int)xdiff.size(); i++) {
		xdiff[0] = (xdiff[0] >= xdiff[i] ? xdiff[0] : xdiff[i]);
		ydiff[0] = (ydiff[0] >= ydiff[i] ? ydiff[0] : ydiff[i]);
	}
	this->warpedDim = Point(xdiff[0], ydiff[0]);
	this->warpingPts.clear();
	this->warpingPts.push_back(Point2f(0, 0));
	this->warpingPts.push_back(Point2f(saturate_cast<float>(xdiff[0]), 0));
	this->warpingPts.push_back(Point2f(saturate_cast<float>(xdiff[0]), saturate_cast<float>(ydiff[0])));
	this->warpingPts.push_back(Point2f(0, saturate_cast<float>(ydiff[0])));
}

void ParkSpotObj::plotParkNum(Mat input){

	Point centralP(0,0);
	char P[5];

	Scalar color;
	switch(this->status) {
	case PARKFREE:
		color = SCLGREEN; break;
	case PARKBUSY:
		color = SCLRED; break;
	case PARKWAIT:
		color = SCLBLUE; break;
	}

	for(int i = 0; i < (int)this->PolyPoints.size(); i++) {
		centralP += this->PolyPoints[i];
	}
	centralP *= (1. / this->PolyPoints.size());

	sprintf(P, "%d", this->nspot);
	putText(input, P, centralP, FONT_HERSHEY_COMPLEX_SMALL, 0.5, color, 1, CV_AA);

}

void ParkSpotObj::show(){

	cout << "┌───────────┐ " << endl;
	cout << "│ PARK SPOT │ " << endl;
	cout << "└──┬────────┘ " << endl;
	cout << "   │          " << endl;
	cout << "   ├─> spot No:    " << this->nspot << endl;
	cout << "   │ " << endl;
	cout << "   ├─> Posizione:  " << this->ParkRect.x << ", " << this->ParkRect.y << endl;
	cout << "   ├─> Dimensione: " << this->ParkRect.width << "x" << this->ParkRect.height << endl;
	cout << "   │ " << endl;
	cout << "   ├─> Status:     " << this->status << endl;
	cout << "   │ " << endl;
	cout << "   ├─> IC buffer:  ";
	for (int i = 0; i < FDFRAME; i++) { if(!(this->buffer[i].empty())) { cout << "[" << i << "]:EXISTS! "; } else { cout << "[" << i << "]:VOID! "; } } cout << endl;
	cout << "   ├─> Hist New:   ";
	for (int i = 0; i < HISTCOMP; i++) {
		if(!((this->hist.hist[i]).empty())) { cout << "["<< i <<"]:EXISTS! "; } else { cout << "[" << i << "]:VOID! "; }
	}
	cout << endl;
	cout << "   ├─> Hist Old:   ";
	for (int i = 0; i < HISTCOMP; i++) {
		if(!((this->hist_old.hist[i]).empty())) { cout << "[" << i << "]:EXISTS! "; } else { cout << "[" << i << "]:VOID! "; }
	} cout << endl;
	cout << "   │ " << endl;
	cout << "   ├─> Polyline:   ";
	for (int i = 0; i < (int)this->PolyPoints.size(); i++) { cout << "P" << i << " = [" << this->PolyPoints[i].x << ", " << this->PolyPoints[i].y << "] "; } cout << endl;
	cout << "   │ " << endl;
	cout << "   └─> Parameters: ";
	for (int i = 0; i < (int)this->parameters.size(); i++) { cout << "[" << i << "]:[ " << this->parameters[i] << " ] "; } cout << endl;
}








