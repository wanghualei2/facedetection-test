#ifndef FILEHELPER_H
#define FILEHELPER_H
#include <iostream>
#include <fstream> 
#include <cstdlib> 
#include <string> 
using namespace std;
using  std::string;

 
class FileHelper{

	string fileName; 

	public:ifstream input; 
	 

	   //@filename文件名 
		public:FileHelper(string  filename){

				this->fileName = filename; 
				cout << filename << endl;;
				input.open(fileName, ios::in);
				if (!input)
				{
					cerr << "Open input file error!" << endl;
					exit(-1);
				} 

		}

	   //读取一行数据
		public:string readline(){
				   string str;
				   char s;
				   while (((s = input.get()) != '\n') && !input.eof()){

					   str = str + s;
				   }
				   return str;
		}

		 

		public:void close(){

				   input.close();
		}
};

#endif