#ifndef PICTURE_H
#define PICTURE_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv.hpp>   
#include <opencv2/imgproc/imgproc.hpp>
#include"FileHelper.h"
#define NORM_WIDTH 19 //��һ����Ŀ�͸�
#define NORM_HEIGHT 19
using namespace cv; 

class Picture{

	//�ò��������жϴ�ͼƬ�Ƿ�Ϊ���� 1��Ϊ���� 0��Ϊ������
public:int label = 1; 

	   //ͼƬ�Ļ���ͼ��
public:int *integral; 

//haar����������   19*19:78460    
	   int haarNum = 67096;// 63960;


int rows;
int cols;

Mat mat;
Mat camerFrame;
 
public:Picture(String filePath){ 

		   camerFrame = imread(filePath, 1); 
		   Pic2Gray(camerFrame, mat);//��ͼ��ת��Ϊ�Ҷ�ͼ��   
		   
}

public:Picture(Mat camerFrame){ 
		   mat = camerFrame;
		   initIntegral(camerFrame);  
}

public:int getHaar(int haar_id,int y,int x,int len,int wid){
		   if (haar_id == 1){
			  return computerSumColor(x, y, wid / 2, len)
				   - computerSumColor(x + wid / 2, y, wid / 2, len);
		   }
		   else if (haar_id == 2){
			   return computerSumColor(x, y, wid, len / 2)
				   - computerSumColor(x, y + len / 2, wid, len / 2);
		   }
		   else if (haar_id == 3){
			   return   computerSumColor(x, y, wid / 3, len)
				   + computerSumColor(x + wid / 3 * 2, y, wid / 3, len)
				   - computerSumColor(x + wid / 3, y, wid / 3, len);
		   }
		   else if (haar_id == 4){
			   return  computerSumColor(x, y, wid, len / 3)
				   + computerSumColor(x, y + len / 3 * 2, wid, len / 3)
				   - computerSumColor(x, y + len / 3, wid, len / 3);
		   }
		   else if (haar_id == 5){
			   return   computerSumColor(x + wid / 2, y, wid / 2, len / 2)
				   + computerSumColor(x, y + len / 2, wid / 2, len / 2)
				   - computerSumColor(x, y, wid / 2, len / 2)
				   - computerSumColor(x + wid / 2, y + len / 2, wid / 2, len / 2);
		   }
		   else{
			   return   computerSumColor(x , y, wid , len ) 
				   -2* computerSumColor(x + wid / 3, y + len / 3, wid / 3, len / 3);
		   }
}

//��ȡָ�������ͼ����Σ������й�һ������
public:Mat getMat(int x, int y, int width){

		Mat srcMatMat(width, width, CV_8UC1, Scalar(0));//��ȡͼ��
	    for (int i = 0; i < width;i++){
			for (int ri = 0; ri < width; ri++){ 
				srcMatMat.at<uchar>(ri, i) = mat.at<uchar>(ri+y, i+x);
			}
		}
		Mat dst_img_rsize(NORM_WIDTH, NORM_HEIGHT, srcMatMat.type());//���й�һ��
		resize(srcMatMat, dst_img_rsize, dst_img_rsize.size(), 0, 0, INTER_LINEAR);
		return dst_img_rsize;
		    
}
	   
public:void freePicture(){ 
		   free(integral);
}


	   //����ɫ�ؾ����ʼ������ͼ��
public:void initIntegral(Mat mat){
		    
		   rows = mat.rows;
		   cols = mat.cols;
		   integral = new int[rows*cols];
		   //���·���������Ϊ���㷨�Ŀ��٣������ж�  
		   integral[0] = mat.at<uchar>(0, 0);
		   for (int i = 1; i < rows; i++){
			   //ucharΪ��ͨ����ȡ����
			   integral[i*cols] = mat.at<uchar>(i, 0) + integral[cols*(i - 1)];
		   }
		   for (int i = 1; i < mat.cols; i++){
			   integral[i] = mat.at<uchar>(0, i) + integral[(i - 1)];
		   }
		   for (int i = 1; i < rows; i++){
			   for (int ri = 1; ri < cols; ri++){
				   integral[i*cols + ri] = integral[i*cols + ri - 1]
					   + integral[(i - 1)*cols + ri] - integral[(i - 1)*cols + ri - 1]
					   + mat.at<uchar>(i, ri);
			   }
		   }
} 

	   //���������������غ�
	   //@x,y �������Ͻǵ�����
	   //@wid,len���εĿ���볤��
public:int computerSumColor(int x, int y, int wid, int len){

		   if (x == 0 && y == 0)
			   return integral[(len - 1)*cols + wid - 1];
		   else if (x == 0){
			   return integral[(y + len - 1)*cols + wid - 1] - integral[(y - 1)*cols + wid - 1];
		   }
		   else if (y == 0){

			   return integral[(len - 1)*cols + x + wid - 1] - integral[(len - 1)*cols + x - 1];
		   }
		   else{
			   return integral[(y + len - 1)*cols + x + wid - 1] - integral[(y - 1)*cols + x + wid - 1]
				   - integral[(y + len - 1)*cols + x - 1] + integral[(y - 1)*cols + x - 1];
		   }
}

	   //��ͼ��ת��Ϊ�Ҷ�ͼ��
	   void Pic2Gray(Mat camerFrame, Mat &gray)
	   {
		   //��̨ͨʽ��3ͨ��BGR,�ƶ��豸Ϊ4ͨ��
		   if (camerFrame.channels() == 3)
		   {
			   cvtColor(camerFrame, gray, CV_BGR2GRAY);
		   }
		   else if (camerFrame.channels() == 4)
		   {
			   cvtColor(camerFrame, gray, CV_BGRA2GRAY);
		   }
		   else
			   gray = camerFrame;
	   }


	   //���㷨���������ض���С��ͼ�����haar����������
	   //һ��ʹ�õ�ͼ��Ϊ24*24,����һ��ʹ��Ĭ�ϵ�162336
		public :int static  computerHaarNum(int rows,int cols){
				   int count = 0, count1 = 0;
				   for (int i = 0; i < rows; i++){//��
					   for (int rri = 0; rri < cols - 1; rri = rri++){  //��
						   for (int ri = 1; ri <= cols - i; ri++){//��
							   for (int y = 2; y <= rows - rri; y = y + 2){//��
								   count1++;

							   }
						   }
					   }
				   }
				   count = count1 * 2;
				   count1 = 0;
				   for (int i = 0; i < rows; i++){//��
					   for (int rri = 0; rri < cols - 2; rri = rri++){  //��
						   for (int ri = 1; ri <= cols - i; ri++){//��
							   for (int y = 3; y <= rows - rri; y = y + 3){//��
								   count1++;

							   }
						   }
					   }
				   }
				   count = count + count1 * 2;
				   count1 = 0;
				   for (int i = 0; i <cols - 1; i++){//��
					   for (int rri = 0; rri < cols - 1; rri = rri++){  //��
						   for (int ri = 2; ri <= rows - i; ri = ri + 2){//��
							   for (int y = 2; y <= cols - rri; y = y + 2){//��
								   count++;

							   }
						   }
					   }
				   }
				   return count;
		}

};
#endif