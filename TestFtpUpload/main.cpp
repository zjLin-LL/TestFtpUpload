#include <windows.h>
#include <vector>
#include<io.h>
#include <wininet.h>
#include <iostream>
using namespace std;


 //获取文件和文件大小
  void GetFiles( string path, vector<string>& files );


 /** 
  *  当上传程序运行在Windows环境，上传文件到Linux环境
  *  为防止目的路径和文件名中文乱码
  *  需要将目的路径和文件名从GB2312转码为UTF-8
  *  反之，从Linux上传文件刀Windows，需要从UTF-8转码为GB2312
  */

 //GB2312到UTF-8的转换 
  char* G2U(const char* gb2312);

 //UTF-8到GB2312的转换 
  char* U2G(const char* utf8);


int main()
{

	//ftp地址
	string ftpSvrIp="192.168.1.126";

	//ftp端口
	int port=21;

	//用户名
	string userName="pi";
    
	//用户密码
	string password="raspberry";

	//上传文件源路径
	string sourceFilePath="C:/Users/LLL/Desktop/1.png";

	//上传文件目的路径
	string desFilePath="/home/pi/pattern";

	vector<string> files; 

	//判断上传的是文件还是文件夹标识       
	int size=0;                                 
	if(sourceFilePath.find(".")==string::npos)
	{
		//获取文件夹下所有文件名
		GetFiles(sourceFilePath, files );  
		char str[30];   
		size = files.size();  
	}
	HINTERNET hint;
	HINTERNET hftp;
	do{
		hint = InternetOpen(0, INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
		if (hint==NULL)  
		{   
			break;  
		} 
		//创建ftp连接
		hftp = InternetConnect(hint, ftpSvrIp.c_str(),port,userName.c_str(),password.c_str(), INTERNET_SERVICE_FTP, 0, 0);
		if (hftp==NULL) 
		{
			break;
		}
		//如果目的路径不存在，一级一级创建
		//目的路径格式为:"./dir/dir1/.../"
		int pos=1;
		string tempPath;
		while(pos>0)
		{
			// 从第二个“/”开始依次找到目的路径中的“/”位置
			pos=desFilePath.find_first_of('/',pos+1);
			if(pos==-1)
				break;
			tempPath=desFilePath.substr(0,pos+1);
			if(_access(tempPath.c_str(),0)==-1)
			{
				FtpCreateDirectory(hftp,tempPath.c_str());
			}
		}

        //如果FtpSetCurrentDirectory一直报错，看一下desFilePath路径是否创建成功，用资源管理器看能否直接访问ftp
		if (FtpSetCurrentDirectory(hftp,desFilePath.c_str()))
		{
			//上传文件源为一个文件
			if(0==size)
			{ 
				//获取上传路径中的文件名
				int pos=sourceFilePath.find_last_of('/');
				string desfilename=sourceFilePath.substr(pos+1);
				int i=FtpPutFile(hftp,sourceFilePath.c_str(),desfilename.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
			}
			//上传文件源为文件夹
			else
			{
				for (int i = 0;i < size;i++)
				{
					std::string tempFilePath=files[i].c_str();
					//获取上传路径中的文件名
					int pos=tempFilePath.find_last_of('/');
					string desfilename=tempFilePath.substr(pos+1);
					FtpPutFile(hftp,tempFilePath.c_str(),desfilename.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);		
				}
			}
		}
	}while(0);

	InternetCloseHandle(hftp);
	InternetCloseHandle(hint);

	cout<<"上传文件完成"<<endl;
	system("pause");

	return 0;
}


void GetFiles( string path, vector<string>& files )  
{  
	//文件句柄  
	long hFile = 0;  
	//文件信息  
	struct _finddata_t fileinfo;  
	std::string p;  
	if((hFile = _findfirst(p.assign(path).append("/*").c_str(),&fileinfo))!= -1)  
	{
		do  
		{  
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if((fileinfo.attrib &  _A_SUBDIR))  
			{  
				if(strcmp(fileinfo.name,".") != 0 && strcmp(fileinfo.name,"..")!= 0)  
					GetFiles( p.assign(path).append("/").append(fileinfo.name), files );  
			}  
			else  
			{  
				files.push_back(p.assign(path).append("/").append(fileinfo.name));  
			}  
		}while(_findnext(hFile, &fileinfo)==0);  
		_findclose(hFile);  
	}  
}

//UTF-8到GB2312的转换 
char* U2G(const char* utf8) 
{ 
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0); 
	wchar_t* wstr = new wchar_t[len+1]; 
	memset(wstr, 0, len+1); 
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len); 
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL); 
	char* str = new char[len+1]; 
	memset(str, 0, len+1); 
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL); 
	if(wstr) delete[] wstr; 
	return str; 
} 

//GB2312到UTF-8的转换 
char* G2U(const char* gb2312) 
{ 
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0); 
	wchar_t* wstr = new wchar_t[len+1]; 
	memset(wstr, 0, len+1); 
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len); 
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL); 
	char* str = new char[len+1]; 
	memset(str, 0, len+1); 
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL); 
	if(wstr) delete[] wstr; 
	return str; 
} 