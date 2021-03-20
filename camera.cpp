#include "camera.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

/*#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>*
#include <opencv2/objdetect/objdetect.hpp>*/

const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

string IntToString (int a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}

RNG rng(12345);

using namespace std;

int treshold1;
int treshold2;
double tresh1max;
double tresh2max;
int y,p;
int ymax;

void on_trackbar(int, void*)
{
 printf("Seuils de la binarisation pour la detection des vehicules : \n");
 printf("Seuil 1 = %d\n", treshold1);
 printf("Seuil 2 = %d\n", treshold2);
 printf("=======================================\n");
}


Camera::Camera()
{
	m_fps = 30;
	Mat cdst,dst,gray;
	m_image =  imread("img.png", IMREAD_UNCHANGED);
	cvtColor(m_image, gray, COLOR_BGR2GRAY);  // image en niveau de gris
  	Canny(m_image, dst, 150, 150, 3);
	HoughLines(dst, m_lines, 1, CV_PI/110, 150, 0, 0 );
	m_ref = imread("def.png", IMREAD_UNCHANGED);

	cvtColor(m_ref, m_ref, COLOR_BGR2GRAY);  // image en niveau de gris

}

bool Camera::open(std::string filename)
{
	m_fileName = filename;
	
	// Convert filename to number if you want to
	// open webcam stream
	std::istringstream iss(filename.c_str());
	int devid;
	bool isOpen;
	if(!(iss >> devid))
	{
		isOpen = m_cap.open(filename.c_str());
	}
	else 
	{
		isOpen = m_cap.open(devid);
	}
	
	if(!isOpen)
	{
		std::cerr << "Unable to open video file." << std::endl;
		return false;
	}
	
	// set framerate, if unable to read framerate, set it to 30
	m_fps = m_cap.get(CV_CAP_PROP_FPS);
	if(m_fps == 0)
		m_fps = 30;
}

void Camera::play()
{
	// Create main window
	namedWindow("Video Originale", CV_WINDOW_AUTOSIZE);
	bool isReading = true;													// Condition de lecture de la video
	int timeToWait = 1000/m_fps;											// Compute time to wait to obtain wanted framerate
	Point pta, ptb, ptc, ptd, pte, ptf, ptg, pth;							// Points pour tracer les voies manuellement
	int cpt = 0;
	Mat diff;																// Matrice difference entre image courante et celle de reference
	y = 80;																	// Ordonnee de la ligne qui verifie et compte le passage d'un vehicule a gauche
	p = 50;																	// et les vehicules a droite														
    int carNumberL = 0;
	int carNumberR = 0;
	String str2, str3;


	while(isReading)
	{

		isReading = m_cap.read(m_frame);										// Get frame from stream

		
		if(isReading)
		{
			Mat dst, gray;
			
			vector<vector<Point> > contours;									// Matrice contenant les contours des vehicules
			vector<vector<Point> > Oldcontours;
  			vector<Vec4i> hierarchy;
  			
																				// Show frame in main window
			imshow("Video Originale",m_frame);		
			cv::moveWindow("Video Originale", 0, 20);

			cvtColor(m_frame, gray, COLOR_BGR2GRAY);  							// image en niveau de gris

      																			
			for( size_t i = 0; i < m_lines.size(); i++ )						// Boucle de detection des voies
 			{	
 				if ((i!=1) && (i!=5))
 				{
 			 	 	float rho = m_lines[i][0], theta = m_lines[i][1];
    			 	Point pt1, pt2;
     			 	double a = cos(theta), b = sin(theta);
	    			double x0 = a*rho, y0 = b*rho;
	    			pt1.x = cvRound(x0 + 1000*(-b));
	    			pt1.y = cvRound(y0 + 1000*(a));
	    			pt2.x = cvRound(x0 - 1000*(-b));
	    			pt2.y = cvRound(y0 - 1000*(a));
	    			line( m_frame, pt1, pt2, SCALAR_GREEN, 3, CV_AA);		// Trace des voies sur m_frame
	 			}																// EN VERT
   				  
  			}

																				
			pta.x = -350; pta.y = -1000; ptb.x = 900; ptb.y = 550;				// Detection des lignes en reliant des points
			ptc.x = -840; ptc.y = 552; ptd.x = 950; ptd.y = -330;
			pte.x = -495; pte.y = 910; ptf.x = 870; ptf.y = -560;
			ptg.x = -770; ptg.y = -680; pth.x = 997; pth.y = 250;
			
			line(m_frame, pta, ptb, SCALAR_BLACK, 3, CV_AA);					// Trace des lignes avec les points precedents
		  	line(m_frame, ptc, ptd, SCALAR_BLACK, 3, CV_AA);					// EN NOIR
			line(m_frame, pte, ptf, SCALAR_BLACK, 3, CV_AA);
			line(m_frame, ptg, pth, SCALAR_BLACK, 3, CV_AA);

			imshow("Detection de lignes manuellement", m_frame);
			cv::moveWindow("Detection de lignes manuellement", 0, 20);

																				// Binarisation pour isoler les vehicules
			absdiff(m_ref,gray,diff);											// Soustrait la matrice gray a celle de reference
			
			imshow("Absdiff", diff);
			cv::moveWindow("Absdiff", 0, 350);

			threshold(diff, dst, 55, 255, THRESH_BINARY);						// Binarisation de diff d'un coefficient 55 sur 255

			imshow("Video binarisee", dst);
			cv::moveWindow("Video binarisee", 0, 700);


	 		int dil = 6;											// Operation de fermeture pour obtenir des vehicules sans trous 
	      	Mat element = getStructuringElement(MORPH_RECT, Size(2*dil + 1, 2*dil+1),Point(dil, dil) ); // filtre rectangulaire a 8 connexites
  			dilate(dst,dst,element);								// dilatation d'un coefficient dil
			
  			int ero = 10;		
  			element = getStructuringElement(MORPH_RECT, Size(2*ero + 1, 2*ero+1),Point(ero, ero) );// filtre rectangulaire a 8 connexites
  			erode(dst, dst, element);								// erosion d'un coefficient ero

  			imshow("Video binarisee apres fermeture", dst);
			cv::moveWindow("Video binarisee apres fermeture", 1100, 20);
	 			
	 		// Trouve et stocke les contours des pixels blancs trouves precedemment dans contours
		  	findContours(dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));	

		  	vector<vector<Point> > contours_poly( contours.size() ); 		// Matrice de contours approximes
  			vector<Rect> bounding( contours.size() );						// Matrice de rectangles
		  	vector<vector<Point> >hull(contours.size());					// Matrice des convexites

		   	for(int i = 0; i < contours.size(); i++)						// Boucle qui produit des contours convexes et approximes
		    {  
		      	convexHull(Mat(contours[i]), hull[i], false);				// hull contient les contours convexes
		    
		    	double epsilon = 0.1*cv::arcLength(contours[i],true);			// Precision de l'approximation polygonale
				cv::approxPolyDP(Mat(contours[i]),contours_poly[i], epsilon, true);	// Approximation des contours

				bounding[i] = cv::boundingRect( Mat (contours_poly[i]));		// Matrice de rectangles autour des contours
		    }

		    Mat drawing = Mat::zeros(dst.size(), CV_8UC3);					// Matrice qui va contenir les contours


			for(int i = 0; i < contours.size(); i++)						// Boucle de dessin des contours
		    {

				drawContours(drawing, contours_poly, i, SCALAR_WHITE, 1, 8, vector<Vec4i>(), 0, Point()); // dessine les contours approximes
			   	drawContours(drawing, hull, i, SCALAR_WHITE, 1, 8, vector<Vec4i>(), 0, Point());		// dessine les contours convexes
						
			    rectangle(m_frame, bounding[i].tl(), bounding[i].br(), SCALAR_RED, 2, 8, 0);	// trace les rectangles sur m_frame 
			    rectangle(drawing, bounding[i].tl(), bounding[i].br(), SCALAR_RED, 2, 8, 0);	// trace les rectangles sur drawing
			        		
			    Point cercleTest = Point(bounding[i].br().x,bounding[i].br().y);	// Cercle qui permet de compter les voitures
			    circle(drawing,Point(bounding[i].br().x,bounding[i].br().y),5,SCALAR_WHITE,1,8,0);
			    // cercle place en bas a droite des rectangles des contours

				if((cercleTest.x < 400) && (cercleTest.y >= y) && (cercleTest.y<=y+3))	// Si le cercle passe la ligne de gauche
			    {	
			       	carNumberL += 1;												// on incremente 1 voiture a gauche
			    }

			    if((cercleTest.x > 400) && (cercleTest.y >= p) && (cercleTest.y<=p+2))	// Si le cercle passe la ligne de droite
			    {
			       	carNumberR += 1;												// On incremente 1 voiture a droite

			    }
			       		
	       	}

			line(m_frame, Point(0,y), Point(400,y),SCALAR_RED,5,8, 0);				// Affichage des lignes de passage
			line(m_frame, Point(400,p), Point(1000,p),SCALAR_RED,5,8, 0);
			putText(m_frame, "Ligne de test de passage", Point(0,y-7), FONT_HERSHEY_SIMPLEX, 0.5, SCALAR_RED, 1, 8, false);

			line(drawing, Point(0,y), Point(400,y),SCALAR_RED,5,8, 0);
			line(drawing, Point(400,p), Point(1000,p),SCALAR_RED,5,8, 0);
			putText(drawing, "Ligne de test de passage", Point(0,y-7), FONT_HERSHEY_SIMPLEX, 0.5, SCALAR_RED, 1, 8, false);

		  	str2 = "#Left : " + IntToString(carNumberL);
	       	putText(m_frame, str2, Point(10,40), FONT_HERSHEY_SIMPLEX, 1, SCALAR_WHITE, 2, 8, false); // affichage du nb de voitures
	       	putText(drawing, str2, Point(10,40), FONT_HERSHEY_SIMPLEX, 1, SCALAR_WHITE, 2, 8, false);
	       		
	       	str3 = "#Right : " + IntToString(carNumberR);
	       	putText(m_frame, str3, Point(630,40), FONT_HERSHEY_SIMPLEX, 1, SCALAR_WHITE, 2, 8, false);
	       	putText(drawing, str3, Point(630,40), FONT_HERSHEY_SIMPLEX, 1, SCALAR_WHITE, 2, 8, false);

	       	imshow("Video apres detection", drawing);					// Video binaire
		  	cv::moveWindow("Video apres detection", 1100, 350);
	       		
		  	imshow("Le grand final", m_frame);							// Video originale avec contours et detection
		  	cv::moveWindow("Le grand final", 1100, 700);

		  		
		}
		else
		{
			cout << "L" << carNumberL << "\n";
			cout << "R" << carNumberR << "\n";
			std::cerr << "Unable to read device" << std::endl;
		}
		
		// If escape key is pressed, quit the program
		if(waitKey(timeToWait)%256 == 27)
		{
			std::cerr << "Stopped by user" << std::endl;
			isReading = false;
		}	
	}	
}

bool Camera::close()
{
	// Close the stream
	m_cap.release();
	
	// Close all the windows
	destroyAllWindows();
	usleep(100000);
}


























