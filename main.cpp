#include "camera.h"

int main(void)
{
	while(1){
	Camera myCam;
		
	myCam.open("cctv.avi");
	
	myCam.play();
	
	myCam.close();
	}
	return 1;
	
}
