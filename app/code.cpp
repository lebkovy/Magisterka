#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;


// g++ $(pkg-config --cflags --libs opencv4) -std=c++11 projekt.cpp -o projekt

//Funkcja odpowiedzialna za nałożenie na plotno zadanego obrazu
void overlayImage(const cv::Mat &background, const cv::Mat &foreground, 
  cv::Mat &output, cv::Point2i location)
{
  background.copyTo(output);
  for(int y = std::max(location.y , 0); y < background.rows; ++y)
  {
    int fY = y - location.y;
    if(fY >= foreground.rows)
      break;
    for(int x = std::max(location.x, 0); x < background.cols; ++x)
    {
      int fX = x - location.x;
      if(fX >= foreground.cols)
        break;
      double opacity =
        ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

        / 255.;
      for(int c = 0; opacity > 0 && c < output.channels(); ++c)
      {
        unsigned char foregroundPx =
          foreground.data[fY * foreground.step + fX * foreground.channels() + c];
        unsigned char backgroundPx =
          background.data[y * background.step + x * background.channels() + c];
        output.data[y*output.step + output.channels()*x + c] =
          backgroundPx * (1.-opacity) + foregroundPx * opacity;
      }
    }
  }
}







 int main( int argc, char** argv )
 {
    VideoCapture cap(0);

    //sprawdzenie dostepu do kamery urzadzenia
    if ( !cap.isOpened() )
    {
         cout << "Cannot open the web cam" << endl;
         return -1;
    }

    //uruchomienie okna z przechwyconym obrazem z kamery
    namedWindow("Control", WINDOW_AUTOSIZE);

    int iLowH = 102;
    int iHighH = 179;

    int iLowS = 150; 
    int iHighS = 255;

    int iLowV = 60;
    int iHighV = 255;

    createTrackbar("LowH", "Control", &iLowH, 179); 
    createTrackbar("HighH", "Control", &iHighH, 179);

    createTrackbar("LowS", "Control", &iLowS, 255);
    createTrackbar("HighS", "Control", &iHighS, 255);

    createTrackbar("LowV", "Control", &iLowV, 255);
    createTrackbar("HighV", "Control", &iHighV, 255);

    int iLastX = -1; 
    int iLastY = -1;

    Mat imgTmp;
    cap.read(imgTmp); 
    
    Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;  
 
    while (true)
    {
        Mat imgOriginal;

        bool bSuccess = cap.read(imgOriginal);

        if (!bSuccess)
        {
             cout << "Cannot read a frame from video stream" << endl;
             break;
        }

        Mat imgHSV;

        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

        Mat imgThresholded;

        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
      
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
 
        Moments oMoments = moments(imgThresholded);
 
        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;
 
        if (dArea > 10000) {
           int posX = dM10 / dArea;
           int posY = dM01 / dArea;        
         
           if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0) {
			        if (waitKey(60) == 113) {
                line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 15);
				      }
		  	      if (waitKey(60) == 119) {
         	      line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,255,0), 15); 
          	  }
				      else if (waitKey(60) == 101)
       		    {
         	      line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,0), 15); 
          	  }
           }
           iLastX = posX;
           iLastY = posY;
        }
  
		Mat apple = imread("apple.png", IMREAD_UNCHANGED);
		
		Mat result;
		
		overlayImage(imgOriginal, apple, result, cv::Point(0,0));

		namedWindow("Apple", WINDOW_AUTOSIZE);
		
        
        Mat imgOriginalFlipped;
		
        //apple = apple + imgLines;
		    imgOriginal = imgOriginal + imgLines;		
        //flip(apple,appleFlipped,1);
        //imshow("Lines", imgFlipped);
        flip(imgOriginal,imgOriginalFlipped,1);
        //imshow("Original", imgOriginalFlipped);
        
        Mat resultFlipped;
        
        result = result + imgLines;
        flip(result, resultFlipped, 1);
        imshow("result", resultFlipped);
        
        
        if (waitKey(30) == 27)
        {
           cout << "Program ended..." << endl;
           break; 
        }
     }

    return 0;
    
}