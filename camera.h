#include <opencv2/opencv.hpp>
#include <string.h>

using namespace cv;
using namespace std;




class Camera
{
public:
	Camera();
	
	bool open(std::string name);
	void play();
	bool close();
	
private:
	std::string m_fileName;
	VideoCapture m_cap;
	int m_fps;
	Mat m_image;  // image que l'on ouvre
	vector<Vec2f> m_lines; // vecteur lignes de detection des voies
	Mat m_ref;
		
	Mat m_frame;	
};
