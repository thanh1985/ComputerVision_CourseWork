
// cvHw2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "cvHw2.h"
#include "cvHw2Dlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <algorithm>
#include <vector>

// using opencv 2.4.7
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CcvHw2Dlg dialog



CcvHw2Dlg::CcvHw2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CcvHw2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcvHw2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcvHw2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BgSub, &CcvHw2Dlg::OnBnClickedBtnBgsub)
	ON_BN_CLICKED(IDC_BTN_PreProc, &CcvHw2Dlg::OnBnClickedBtnPreproc)
	ON_BN_CLICKED(IDC_BTN_Tracking, &CcvHw2Dlg::OnBnClickedBtnTracking)
	ON_BN_CLICKED(IDC_BTN_Eigs, &CcvHw2Dlg::OnBnClickedBtnEigs)
	ON_BN_CLICKED(IDC_BTN_FR, &CcvHw2Dlg::OnBnClickedBtnFr)
	ON_BN_CLICKED(IDC_BTN_FD, &CcvHw2Dlg::OnBnClickedBtnFd)
	ON_BN_CLICKED(IDC_BTN_DR, &CcvHw2Dlg::OnBnClickedBtnDr)
END_MESSAGE_MAP()


// CcvHw2Dlg message handlers

BOOL CcvHw2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	//////////
	// connect to the console
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	cout.clear();
	//////////

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CcvHw2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CcvHw2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CcvHw2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*------------------------------------------------------------------------------------*/
void CcvHw2Dlg::OnBnClickedBtnBgsub()
{
	// TODO: Add your control notification handler code here
	string fileName = "../Database/bgSub_video.mp4";
	VideoCapture cap(fileName);

	Mat frame; //current frame
	Mat fgMaskMOG; //fg mask generated by MOG method
	Ptr<BackgroundSubtractor> pMOG; //MOG Background subtractor
	int key = 0;
	
	namedWindow("Frame", WINDOW_AUTOSIZE);

	pMOG = new BackgroundSubtractorMOG(); //MOG approach

	string sNotice =  "Please press Esc or q to exit";
	while ((char)key != 'q' && (char)key != 27)
	{
		//read the current frame
		cap.read(frame);
		// if the video is finished, you get empty frames from the capture
		// if (frame.empty()) break;
		if (!cap.read(frame)) { 
			break;
		}
		//update the background model
		pMOG->operator()(frame, fgMaskMOG);
		//get the frame number and write it on the current frame
		stringstream ss;
		rectangle(frame, cv::Point(10, 2), cv::Point(100, 20),
			cv::Scalar(255, 255, 255), -1);
		ss << cap.get(CV_CAP_PROP_POS_FRAMES);
		string frameNumberString = ss.str();
		putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
			FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));

		putText(frame, sNotice, cv::Point(15, 50),
			FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
		//show the current frame and the fg masks
		imshow("Frame", frame);
		imshow("FG Mask MOG", fgMaskMOG);
		
		//get the input from the keyboard
		key = waitKey(60);
	}

	waitKey(0);
	destroyAllWindows();
	cap.release();
}

// global vars
vector<KeyPoint> keypoints, keypointsSorted;
bool bPre;
int winSize = 10;

void CcvHw2Dlg::OnBnClickedBtnPreproc()
{
	// TODO: Add your control notification handler code here

	string fileName = "../Database/tracking_video.mp4";
	VideoCapture cap(fileName);
	Mat frame;
	// read the 1st frame
	// Note: 1st readed frame is sometimes black/gray, try to display next frame
	cap.read(frame);
	//cap.read(frame);
	
	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 75;
	params.maxThreshold = 120;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 25;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.7;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.5;

	Ptr<SimpleBlobDetector> pSBD;
	// Setup SimpleBlobDetector parameters.
	pSBD = new SimpleBlobDetector(params);
	
	pSBD->detect(frame, keypoints);
	
	Point pt1(0, 0), pt2(10, 10);

	// TODO: improve the following to for loop
	keypointsSorted.push_back(keypoints[6]);
	keypointsSorted.push_back(keypoints[4]);
	keypointsSorted.push_back(keypoints[1]);
	keypointsSorted.push_back(keypoints[2]);
	keypointsSorted.push_back(keypoints[3]);
	keypointsSorted.push_back(keypoints[0]);
	keypointsSorted.push_back(keypoints[5]);

	// save keypoints to a txt file
	ofstream fout("hw2_1.txt");
	string points;

	for (int i = 0; i < keypointsSorted.size(); ++i){
		points = "Point" + std::to_string(i+1) + ":";
		fout << points << "(" << roundf(keypointsSorted[i].pt.x) << "," << roundf(keypointsSorted[i].pt.y) << ")" << endl;
	}

	fout << "Window size: " << winSize << endl;
	fout.close();

	for (int i = 0; i < keypoints.size(); ++i)
	{
		pt1.x = keypoints[i].pt.x - winSize / 2;
		pt1.y = keypoints[i].pt.y - winSize / 2;
		pt2.x = keypoints[i].pt.x + winSize / 2;
		pt2.y = keypoints[i].pt.y + winSize / 2;

		rectangle(frame, pt1, pt2, cv::Scalar(0, 0, 255));

		for (int j = round(pt1.y); j < round(pt2.y); ++j){
			for (int k = round(pt1.x); k < round(pt2.x); ++k) {
				frame.at<Vec3b>(j, k)[2] = 255;
			}
		}
			
	}
		
	imshow("Blue ellipse detection", frame);
	waitKey(0);
	destroyAllWindows();
	cap.release();
	bPre = true;
}


void CcvHw2Dlg::OnBnClickedBtnTracking()
{
	// TODO: Add your control notification handler code here
	if (bPre)
	{
		string fileName = "../Database/tracking_video.mp4";
		VideoCapture cap(fileName);
		Mat frame_prev, frame_next;
		std::vector<cv::Point2f> features_prev, features_next;
		vector<vector<Point2f>> vKeypoints;
		// read the 1st frame
		cap.read(frame_next);

		for (int i = 0; i < keypointsSorted.size(); ++i){
			features_next.push_back(keypointsSorted[i].pt);
		}

		// define parameters for func. calcOpticalFlowPyrLK(params)
		vector<uchar> status;
		vector<float> err;
		TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);

		for (;;)
		{
			vKeypoints.push_back(features_next);
			frame_prev = frame_next.clone();
			features_prev = features_next;
			cap.read(frame_next);				// get next frame
			if (!cap.read(frame_next)) {
				break;
			}
			calcOpticalFlowPyrLK(frame_prev, frame_next, features_prev, features_next, status, err, Size(winSize * 2 + 1, winSize * 2 + 1), 3, termcrit, 0, 0.001);
		}
		cap.release();

		// save keypoints to a txt file
		ofstream fout("hw2_2.txt");
		string sframes;
		for (int i = 0; i < vKeypoints.size(); ++i) {
			sframes = "frame " + std::to_string(i + 1) + ":";
			fout << sframes;
			for (int j = 0; j < vKeypoints[i].size(); ++j) {
				if (j < vKeypoints[i].size() - 1) {
					fout << "(" << roundf(vKeypoints[i][j].x) << "," << roundf(vKeypoints[i][j].y) << "), ";
				}
				else {
					fout << "(" << roundf(vKeypoints[i][j].x) << "," << roundf(vKeypoints[i][j].y) << ")";
				}
			}
			fout << endl;
		}
		fout.close();

		// read the saved txt file, i.e hw2_2.txt 
		ifstream fin("hw2_2.txt");
		string line;
		vector<Point2f> vPoints;
		vector<vector<Point2f>>  savedFeatures;

		while (getline(fin, line)) { // get a whole line
			stringstream ss(line);
			string stmp;
			int itmp;
			char ctmp;
			int sx1, sy1, sx2, sy2, sx3, sy3, sx4, sy4, sx5, sy5, sx6, sy6, sx7, sy7;

			ss >> stmp >> itmp >> ctmp >> ctmp >> sx1 >> ctmp >> sy1 
				>> ctmp >> ctmp >> ctmp >> sx2 >> ctmp >> sy2
				>> ctmp >> ctmp >> ctmp >> sx3 >> ctmp >> sy3
				>> ctmp >> ctmp >> ctmp >> sx4 >> ctmp >> sy4
				>> ctmp >> ctmp >> ctmp >> sx5 >> ctmp >> sy5
				>> ctmp >> ctmp >> ctmp >> sx6 >> ctmp >> sy6
				>> ctmp >> ctmp >> ctmp >> sx7 >> ctmp >> sy7;
		
			/*cout << sx1 << sy1 << "\t" << sx2 << sy2 << "\t";
			cout << sx3 << sy3 << "\t" << sx4 << sy4 << "\t";
			cout << sx5 << sy5 << "\t" << sx6 << sy6 << "\t";
			cout << sx7 << sy7 << endl;*/
			vPoints.push_back(Point2f(sx1, sy1));
			vPoints.push_back(Point2f(sx2, sy2));
			vPoints.push_back(Point2f(sx3, sy3));
			vPoints.push_back(Point2f(sx4, sy4));
			vPoints.push_back(Point2f(sx5, sy5));
			vPoints.push_back(Point2f(sx6, sy6));
			vPoints.push_back(Point2f(sx7, sy7));

			savedFeatures.push_back(vPoints);
			//vPoints.clear(); // reset the vector, avoiding accumulation
			// but here, we want to accumulate all the points
		}

		fin.close();
	
		// draw points
		Mat frame;
		VideoCapture cap1(fileName);
		Point pt1(0, 0), pt2(10, 10);

		for (int i = 0; i < savedFeatures.size()-28; ++i)
		{
			cap1.read(frame);				// get 1 frame
			if (!cap1.read(frame)) {
				break;
			}

			// get only the last 7 elements
			for (int jj = savedFeatures[i].size()-1; jj > savedFeatures[i].size() - 8; --jj){
				pt1.x = savedFeatures[i][jj].x - winSize / 2;
				pt1.y = savedFeatures[i][jj].y - winSize / 2;
				pt2.x = savedFeatures[i][jj].x + winSize / 2;
				pt2.y = savedFeatures[i][jj].y + winSize / 2;

				rectangle(frame, pt1, pt2, cv::Scalar(0, 0, 255));

				for (int jj = round(pt1.y); jj < round(pt2.y); ++jj){
					for (int k = round(pt1.x); k < round(pt2.x); ++k) {
						frame.at<Vec3b>(jj, k)[2] = 255;
					}
				}

			}

			for (int j = 0; j < savedFeatures[i].size(); ++j){
				frame.at<Vec3b>(savedFeatures[i][j].y, savedFeatures[i][j].x)[2] = 255;
				//cout << savedFeatures[i][j].x << ", " << savedFeatures[i][j].y << " ";
			}

			//cout << endl;
			imshow("Optical Flow", frame);
			waitKey(120);
		}

		cap1.release();
		destroyAllWindows();
	}
	else
		cout << "Please run the preprocessing first" << endl;
}

// Function declarations
void drawAxis(Mat&, Point, Point, Scalar, const float);
void CcvHw2Dlg::OnBnClickedBtnEigs()
{
	// TODO: Add your control notification handler code here
	 
	Mat img_c, img_g, img_bw;
	img_c = imread("../Database/pca.jpg", IMREAD_COLOR);
	cvtColor(img_c, img_g, COLOR_BGR2GRAY);
	threshold(img_g, img_bw, 0, 255, THRESH_BINARY);

	vector<Point> xy;
	
	for (int i = 0; i < img_bw.rows; ++i){
		for (int j = 0; j < img_bw.cols; ++j){
			if (img_bw.at<uchar>(i, j) > 0){
				xy.push_back(Point(i, j));
			}
		}
	}
	//cout << xy.size() << endl;
	Mat dataPoints = Mat(xy.size(), 2, CV_64FC1);
	for (int i = 0; i < dataPoints.rows; ++i)
	{
		dataPoints.at<double>(i, 0) = xy[i].y;
		dataPoints.at<double>(i, 1) = xy[i].x;
	}
	//Perform PCA analysis
	PCA pca1(dataPoints, Mat(), CV_PCA_DATA_AS_ROW);
	//Store the center of the object
	Point cPoint = Point(static_cast<int>(pca1.mean.at<double>(0, 0)), static_cast<int>(pca1.mean.at<double>(0, 1)));
	//Store the eigenvalues and eigenvectors
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	cout << pca1.eigenvalues.size() << ", " << pca1.eigenvectors.size() << endl;

	for (int i = 0; i < 2; ++i){
		eigen_vecs[i] = Point2d(pca1.eigenvectors.at<double>(i, 0), pca1.eigenvectors.at<double>(i, 1));
		eigen_val[i] = pca1.eigenvalues.at<double>(i, 0);
	}
	
	// Draw the principal components
	circle(img_c, cPoint, 3, Scalar(255, 0, 255), CV_FILLED);
	Point p1 = cPoint + 0.02 * Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
	Point p2 = cPoint - 0.02 * Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
	drawAxis(img_c, cPoint, p1, Scalar(0, 255, 0), 1.5);
	drawAxis(img_c, cPoint, p2, Scalar(255, 255, 0), 5.0);

	imshow("output", img_c);
	waitKey(0);
	destroyAllWindows();
}

// refer to 
// https://docs.opencv.org/3.1.0/d1/dee/tutorial_introduction_to_pca.html
void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale = 0.2)
{
	double angle;
	double hypotenuse;
	angle = atan2((double)p.y - q.y, (double)p.x - q.x); // angle in radians
	hypotenuse = sqrt((double)(p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
	//    double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
	//    cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range
	// Here we lengthen the arrow by a factor of scale
	q.x = (int)(p.x - scale * hypotenuse * cos(angle));
	q.y = (int)(p.y - scale * hypotenuse * sin(angle));
	line(img, p, q, colour, 1, CV_AA);
	// create the arrow hooks
	p.x = (int)(q.x + 9 * cos(angle + CV_PI / 4));
	p.y = (int)(q.y + 9 * sin(angle + CV_PI / 4));
	line(img, p, q, colour, 1, CV_AA);
	p.x = (int)(q.x + 9 * cos(angle - CV_PI / 4));
	p.y = (int)(q.y + 9 * sin(angle - CV_PI / 4));
	line(img, p, q, colour, 1, CV_AA);
}

// normalize images
static Mat norm_0_255(InputArray _src) {
	Mat src = _src.getMat();
	// Create and return normalized image:
	Mat dst;
	switch (src.channels()) {
	case 1:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}


// read training data
static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty()) {
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}

// Find name of a face, given the label
string FindName(int nLabel)
{
	switch (nLabel){
	case 0:
		return "Harry Potter";
		break;
	case 1:
		return "Hermione Granger";
		break;
	case 2:
		return "Ron Weasley";
		break;
	default:
		return "undefined";
		break;
	}
}

// global vars
Ptr<FaceRecognizer> model;
bool b3p2;
void CcvHw2Dlg::OnBnClickedBtnFr()
{
	// TODO: Add your control notification handler code here
	string fn_csv = "../Database/trainingData.csv";
	// These vectors hold the images and corresponding labels.
	vector<Mat> images;
	vector<int> labels;

	// Read in the data.
	try {
		read_csv(fn_csv, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		// nothing more we can do
		OnCancel();
	}

	Mat imgMean = (images[0] + images[1] + images[2])/3;
	namedWindow("The mean face", WINDOW_AUTOSIZE);
	moveWindow("The mean face", 300, 300);
	imshow("The mean face", imgMean);

	int height = images[0].rows;
	Mat testSample = images[images.size() - 1];
	int testLabel = labels[labels.size() - 1];
	images.pop_back();
	labels.pop_back();

	model = createEigenFaceRecognizer();
	model->train(images, labels);

	// predict the label of a given image
	int predictedLabel = model->predict(testSample);
	/*string result_message = format("Predicted class = %d / Actual class = %d.", predictedLabel, testLabel);
	cout << result_message << endl;*/
	
	cout << "The input (test, or actual) image is: " << FindName(testLabel) << endl;
	cout << "The output (predicted) image is: " << FindName(predictedLabel) << endl;

	namedWindow("Test image");
	moveWindow("Test image", 500, 300);
	imshow("Test image", testSample);

	waitKey(0);
	destroyAllWindows();
	b3p2 = true;
}

// global vars
vector<Mat> vFaces;
vector<Rect> faces;
bool b4p1 = false;
void CcvHw2Dlg::OnBnClickedBtnFd()
{
	// TODO: Add your control notification handler code here

	Mat image, gray, testImg;
	image = imread("../Database/face.jpg", IMREAD_COLOR);
	cvtColor(image, gray, CV_BGR2GRAY);
	testImg = imread("../Database/test.jpg", IMREAD_GRAYSCALE);
	// Load Face cascade(.xml file)
	CascadeClassifier face_cascade;
	face_cascade.load("../Database/haarcascade_frontalface_alt2.xml");
	// Detect faces
	face_cascade.detectMultiScale(image, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(60, 60));

	// Draw rectangles on the detected faces
	for (int i = 0; i < faces.size(); i++)
	{
		rectangle(image, faces[i], cv::Scalar(0, 0, 255), 2);
	}

	imshow("Detected Face", image);

	// prepare faces for part 4.2
	if (!b4p1) {
		Mat roiI1, roiI2, roiI3;

		Mat roi1 = Mat(gray, faces[0]);
		resize(roi1, roiI1, testImg.size());
		vFaces.push_back(roiI1);

		Mat roi2 = Mat(gray, faces[1]);
		resize(roi2, roiI2, testImg.size());
		vFaces.push_back(roiI2);

		Mat roi3 = Mat(gray, faces[2]);
		resize(roi3, roiI3, testImg.size());
		vFaces.push_back(roiI3);
	}

	waitKey(1000);
	destroyAllWindows();

	b4p1 = true;
}


void CcvHw2Dlg::OnBnClickedBtnDr()
{
	// TODO: Add your control notification handler code here

	// run this only, 3.2 and 4.1 were called already
	if ((b3p2) && (b4p1))
	{
		Mat image;
		image = imread("../Database/face.jpg", IMREAD_COLOR);

		for (int i = 0; i < vFaces.size(); ++i)
		{
			int predictedLabel = model->predict(vFaces[i]);
			cout << predictedLabel << ": " << FindName(predictedLabel) << endl;

			rectangle(image, faces[i], cv::Scalar(0, 0, 255), 2);
			Point ptText(faces[i].x - 20, faces[i].y - 10);
			putText(image, FindName(predictedLabel), ptText,
				FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
		}

		imshow("Face Detection and Recognition", image);

		waitKey(0);
		destroyAllWindows();
	}
	else
		cout << "Please run 3.2 and 4.1 first" << endl;
}
