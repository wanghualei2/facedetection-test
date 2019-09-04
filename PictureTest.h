#ifndef PICTURETEST_H
#define PICTURETEST_H
#include"Picture.h" 
#include<math.h>

class Haar_No_value{

	public:int haar_id;

	public:int rows;
	public:int cols;
	public:int len;
	public:int wid;

	public:Haar_No_value(){

	}

	public:Haar_No_value(int haar_id,int rows, int cols, int len, int wid){
			   this->haar_id = haar_id;
			   this->rows = rows;
			   this->cols = cols;
			   this->len = len;
			   this->wid = wid;
	}
};

class  FaceRect
{ 
	public:int x;
	public:int y;
	public:int width;

	double value;

	public:FaceRect(){

	}

	public:FaceRect(int x, int y, int width,double value){
			   this->x = x;
			   this->y = y;
			   this->width = width;
			   this->value = value;
	}
};

class PictureTest{

			FaceRect face[1000];
			int faceNum=0; 

			int iteration_nums = 300;

			//要检测的图片
			Picture *picture;

			//阙值数组
			int *haar_compares;

			//Sj参数{1，-1}
			int *p_s;

			//无用参数
			int *haarNo;

			//系数数组
			double *coefficient;

			//haar特征的数量----24*24:162336   20*20:78460  19*19:63960
			int haarNum = 67096;// 63960;

			Haar_No_value *haar_No_value;

			//人脸的左上角坐标,这里设置了默认最多10张人脸
			int x_s[10],y_s[10];

			//人脸的宽度和高度
			int width_s[10], height_s[10];

			//人脸的数目
			int face_num = 0;

			String imagefile_path;

			public :PictureTest(String imagefile_path,String model_path){   
						picture = new Picture(imagefile_path);
						this->imagefile_path = imagefile_path;
						p_s = new int[iteration_nums];
						haar_compares = new int[iteration_nums];
						coefficient = new double[iteration_nums];
						haarNo = new int[iteration_nums];
						initModel(model_path);
						pictureTest(); 

			}  
			void initModel(String model_path){ 
				haar_No_value = new Haar_No_value[haarNum];  
				// 读文件
				ifstream inFile(model_path, ios::in);
				string lineStr;
				vector<vector<string>> strArray;
				int i = 0;
				while (getline(inFile, lineStr))
				{ 
					// 存成二维表结构
					stringstream ss(lineStr);
					string str;
					getline(ss, str, ','); 
					haarNo[i] = stoi(str); 
					getline(ss, str, ',');
					p_s[i] = stoi(str);
					getline(ss, str, ',');
					haar_compares[i] = stoi(str);
					getline(ss, str, ',');
					coefficient[i] = stod(str); 
					getline(ss, str, ',');
					int haar_id = stoi(str);
					getline(ss, str, ',');
					int rows = stoi(str);
					getline(ss, str, ',');
					int cols = stoi(str);
					getline(ss, str, ',');
					int len = stoi(str);
					getline(ss, str, ',');
					int wid = stoi(str);
					Haar_No_value *h=new Haar_No_value(haar_id, rows, cols, len, wid);
					haar_No_value[i] = *h; 
					i++; 
				}
			}
			 
			public :void pictureTest(){
						double valueMax = 0;
						int width = picture->mat.cols;
						int height = picture->mat.rows;
						cout << "width:"<<width<<"\theight:"<<height << endl;
						double s1=0,s2=0;
						Picture *p;
						Mat mat = imread(imagefile_path, 1);
						int wid = 19;
						for (; wid < width&&wid < height; wid = wid + (wid + 1) / 10){//第一层循环----1000 
							for (int x = 0; x + wid < width; x = x + (wid+1) / 10){             //第二层循环----1000
								for (int y = 0; (y + wid) < height; y = y + (wid + 1) / 10){        //第三层循环----500 
									p=new Picture(picture->getMat(x, y, wid));  
									//获取截取的图片，开始检测
									for (int i = 0; i < iteration_nums; i++){        //第四层循环----200 
										s1 = s1 + coefficient[i] * weak_classifier(p_s[i], p->getHaar(haar_No_value[i].haar_id,
											haar_No_value[i].rows,
											haar_No_value[i].cols, haar_No_value[i].len, haar_No_value[i].wid), haar_compares[i]); 
										s2 = s2 + coefficient[i];
									} 
									p->freePicture();
									free(p);
									if (s1>(s2/2)){
										//cout << "..............人脸:"<<faceNum<<"\tx:" << x << "\ty:" << y << "\twid:" << wid <<"\tmargin:"<<s1<<".."<<s2/2<<".."<<s1-(s2 / 2) << endl;
										 
										double value = s1 - (s2 / 2);
										FaceRect rect(x, y, wid, value);
										if (rect.value < (valueMax / 4))
											continue;
										if (valueMax <value){
											valueMax = value;
										}
										bool isAdd=true;
										 
										//合并矩形框 
										for (int faceNo = 0; faceNo < faceNum; faceNo++){
											if (isSimliar(rect, face[faceNo])){
												isAdd = false;
												if (face[faceNo].value / face[faceNo].width> rect.value/rect.width){
													break;
												}
												else{
													face[faceNo].x = rect.x;
													face[faceNo].y = rect.y;
													face[faceNo].width = rect.width;
													face[faceNo].value = rect.value;
												}
											}
										} 
										if (isAdd){   
											face[faceNum] = rect;
											faceNum++;

										} 
									}
									s1 = s2 = 0;
								} 
							} 
						} 

						for (int faceNo = 0; faceNo < faceNum; faceNo++){
							if (face[faceNo].value < (valueMax / 4))
								continue;
							cout << face[faceNo].x << "\t" << face[faceNo].y << "\t" << face[faceNo].width << "\tvalue:" << 
								face[faceNo].value << endl;
							Rect r(face[faceNo].x, face[faceNo].y, face[faceNo].width, face[faceNo].width);
							cv::rectangle(mat, r, Scalar(255, 0, 0), 1, 8, 0);
						} 
						imshow("src", mat);
						waitKey(0);
			}
 
			//判断两个矩形是不是一个
			bool isSimliar(FaceRect r1, FaceRect r2){ 
				if ((r1.x <= r2.x) && (r1.y <= r2.y)){
					if ((r2.x - r1.x > r1.width) || (r2.y - r1.y > r1.width))
						return false;
					else
						return true;
				}
				else if ((r1.x <= r2.x) && (r1.y >= r2.y)){
					if ((r2.x - r1.x > r1.width) || (r1.y - r2.y > r2.width))
						return false;
					else
						return true;
				}
				else if ((r1.x >= r2.x) && (r1.y <= r2.y)){
					if ((r1.x - r2.x > r2.width) || (r2.y - r1.y > r1.width))
						return false;
					else
						return true;
				}
				else if ((r1.x >= r2.x) && (r1.y >= r2.y)){
					if ((r1.x - r2.x > r2.width) || (r1.y - r2.y > r2.width))
						return false;
					else
						return true;
				}
				return false;
			}

			public:int weak_classifier(int p, int feature, int feature_compare){
					   if (p*feature < p*feature_compare)
						   return 1;//人脸
					   else
						   return 0;
			}
	
};
#endif