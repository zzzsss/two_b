#include"cv.h"
#include"the_image.h"
//this file is the reference
//这段代码是引用的

int aoiGravityCenter(IplImage *src, CvPoint &center)
{
	//if(!src)
	// return GRAVITYCENTER__SRC_IS_NULL;
	double m00, m10, m01;
	CvMoments moment;
	cvMoments( src, &moment, 1);
	m00 = cvGetSpatialMoment( &moment, 0, 0 );
	if( m00 == 0)
		return 1;
	m10 = cvGetSpatialMoment( &moment, 1, 0 );
	m01 = cvGetSpatialMoment( &moment, 0, 1 );
	center.x = (int) (m10/m00);
	center.y = (int) (m01/m00);
	return 0;
}

void cvThin (IplImage* src, IplImage* dst, int iterations)
{
	cvCopyImage(src, dst);
	BwImage dstdat(dst);
	IplImage* t_image = cvCloneImage(src);
	BwImage t_dat(t_image);
	for (int n = 0; n < iterations; n++)
		for (int s = 0; s <= 1; s++) {
			cvCopyImage(dst, t_image);
			for (int i = 0; i < src->height; i++)
				for (int j = 0; j < src->width; j++)
					if (t_dat[i][j]) {
						int a = 0, b = 0;
						int d[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1},
						{1, 0}, {1, -1}, {0, -1}, {-1, -1}};
						int p[8];
						p[0] = (i == 0) ? 0 : t_dat[i-1][j];
						for (int k = 1; k <= 8; k++) {
							if (i+d[k%8][0] < 0 || i+d[k%8][0] >= src->height ||
								j+d[k%8][1] < 0 || j+d[k%8][1] >= src->width)
								p[k%8] = 0;
							else 
								p[k%8] = t_dat[ i+d[k%8][0] ][ j+d[k%8][1] ];
							if (p[k%8]) {
								b++;
								if (!p[k-1]) a++;
							}
						}
						if (b >= 2 && b <= 6 && a == 1)
							if (!s && !(p[2] && p[4] && (p[0] || p[6])))
								dstdat[i][j] = 0;
						else if (s && !(p[0] && p[6] && (p[2] || p[4])))
							dstdat[i][j] = 0;
					}
			}
	cvReleaseImage(&t_image);
}