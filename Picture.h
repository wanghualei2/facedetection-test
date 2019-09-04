#ifndef PICTURE_H
#define PICTURE_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv.hpp>   
#include <opencv2/imgproc/imgproc.hpp>
#include"FileHelper.h"
#define NORM_WIDTH 19 //归一化后的宽和高
#define NORM_HEIGHT 19
using namespace cv; 

class Picture{

	//该参数用来判断此图片是否为人脸 1：为人脸 0：为非人脸
public:int label = 1; 

	   //图片的积分图像
public:int *integral; 

//haar特征的数量   19*19:78460    
	   int haarNum = 67096;// 63960;


int rows;
int cols;

Mat mat;
Mat camerFrame;
 
public:Picture(String filePath){ 

		   camerFrame = imread(filePath, 1); 
		   Pic2Gray(camerFrame, mat);//将图像转化为灰度图像   
		   
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

//获取指定区域的图像矩形，并进行归一化操作
public:Mat getMat(int x, int y, int width){

		Mat srcMatMat(width, width, CV_8UC1, Scalar(0));//截取图像
	    for (int i = 0; i < width;i++){
			for (int ri = 0; ri < width; ri++){ 
				srcMatMat.at<uchar>(ri, i) = mat.at<uchar>(ri+y, i+x);
			}
		}
		Mat dst_img_rsize(NORM_WIDTH, NORM_HEIGHT, srcMatMat.type());//进行归一化
		resize(srcMatMat, dst_img_rsize, dst_img_rsize.size(), 0, 0, INTER_LINEAR);
		return dst_img_rsize;
		    
}
	   
public:void freePicture(){ 
		   free(integral);
}


	   //根据色素矩阵初始化积分图像
public:void initIntegral(Mat mat){
		    
		   rows = mat.rows;
		   cols = mat.cols;
		   integral = new int[rows*cols];
		   //以下繁琐步骤是为了算法的快速，减少判断  
		   integral[0] = mat.at<uchar>(0, 0);
		   for (int i = 1; i < rows; i++){
			   //uchar为单通道读取方法
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

	   //计算矩形区域的像素和
	   //@x,y 矩形右上角的坐标
	   //@wid,len矩形的宽度与长度
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

	   //将图像转化为灰度图像
	   void Pic2Gray(Mat camerFrame, Mat &gray)
	   {
		   //普通台式机3通道BGR,移动设备为4通道
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


	   //该算法用来计算特定大小的图像产生haar特征的数量
	   //一般使用的图像为24*24,所以一般使用默认的162336
		public :int static  computerHaarNum(int rows,int cols){
				   int count = 0, count1 = 0;
				   for (int i = 0; i < rows; i++){//行
					   for (int rri = 0; rri < cols - 1; rri = rri++){  //列
						   for (int ri = 1; ri <= cols - i; ri++){//长
							   for (int y = 2; y <= rows - rri; y = y + 2){//宽
								   count1++;

							   }
						   }
					   }
				   }
				   count = count1 * 2;
				   count1 = 0;
				   for (int i = 0; i < rows; i++){//行
					   for (int rri = 0; rri < cols - 2; rri = rri++){  //列
						   for (int ri = 1; ri <= cols - i; ri++){//长
							   for (int y = 3; y <= rows - rri; y = y + 3){//宽
								   count1++;

							   }
						   }
					   }
				   }
				   count = count + count1 * 2;
				   count1 = 0;
				   for (int i = 0; i <cols - 1; i++){//行
					   for (int rri = 0; rri < cols - 1; rri = rri++){  //列
						   for (int ri = 2; ri <= rows - i; ri = ri + 2){//长
							   for (int y = 2; y <= cols - rri; y = y + 2){//宽
								   count++;

							   }
						   }
					   }
				   }
				   return count;
		}

};
#endif