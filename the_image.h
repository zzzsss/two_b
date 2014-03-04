#ifndef THE_IMAGE_
#define THE_IMAGE_

template<class T> 
class Image{
private:
	IplImage* imgp;
public:
	Image(IplImage* img=0) {imgp=img;}
	~Image(){imgp=0;}
	inline T* operator[](const int rowIndx) 
	{
	return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));
	}
};

typedef struct{
	unsigned char b,g,r;
} RgbPixel;

typedef Image<RgbPixel> RgbImage;
typedef Image<unsigned char> BwImage;


#endif