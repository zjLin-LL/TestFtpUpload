#include <windows.h>
#include <vector>
#include<io.h>
#include <wininet.h>
#include <iostream>
using namespace std;


 //��ȡ�ļ����ļ���С
  void GetFiles( string path, vector<string>& files );


 /** 
  *  ���ϴ�����������Windows�������ϴ��ļ���Linux����
  *  Ϊ��ֹĿ��·�����ļ�����������
  *  ��Ҫ��Ŀ��·�����ļ�����GB2312ת��ΪUTF-8
  *  ��֮����Linux�ϴ��ļ���Windows����Ҫ��UTF-8ת��ΪGB2312
  */

 //GB2312��UTF-8��ת�� 
  char* G2U(const char* gb2312);

 //UTF-8��GB2312��ת�� 
  char* U2G(const char* utf8);


int main()
{

	//ftp��ַ
	string ftpSvrIp="192.168.1.126";

	//ftp�˿�
	int port=21;

	//�û���
	string userName="pi";
    
	//�û�����
	string password="raspberry";

	//�ϴ��ļ�Դ·��
	string sourceFilePath="C:/Users/LLL/Desktop/1.png";

	//�ϴ��ļ�Ŀ��·��
	string desFilePath="/home/pi/pattern";

	vector<string> files; 

	//�ж��ϴ������ļ������ļ��б�ʶ       
	int size=0;                                 
	if(sourceFilePath.find(".")==string::npos)
	{
		//��ȡ�ļ����������ļ���
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
		//����ftp����
		hftp = InternetConnect(hint, ftpSvrIp.c_str(),port,userName.c_str(),password.c_str(), INTERNET_SERVICE_FTP, 0, 0);
		if (hftp==NULL) 
		{
			break;
		}
		//���Ŀ��·�������ڣ�һ��һ������
		//Ŀ��·����ʽΪ:"./dir/dir1/.../"
		int pos=1;
		string tempPath;
		while(pos>0)
		{
			// �ӵڶ�����/����ʼ�����ҵ�Ŀ��·���еġ�/��λ��
			pos=desFilePath.find_first_of('/',pos+1);
			if(pos==-1)
				break;
			tempPath=desFilePath.substr(0,pos+1);
			if(_access(tempPath.c_str(),0)==-1)
			{
				FtpCreateDirectory(hftp,tempPath.c_str());
			}
		}

        //���FtpSetCurrentDirectoryһֱ������һ��desFilePath·���Ƿ񴴽��ɹ�������Դ���������ܷ�ֱ�ӷ���ftp
		if (FtpSetCurrentDirectory(hftp,desFilePath.c_str()))
		{
			//�ϴ��ļ�ԴΪһ���ļ�
			if(0==size)
			{ 
				//��ȡ�ϴ�·���е��ļ���
				int pos=sourceFilePath.find_last_of('/');
				string desfilename=sourceFilePath.substr(pos+1);
				int i=FtpPutFile(hftp,sourceFilePath.c_str(),desfilename.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
			}
			//�ϴ��ļ�ԴΪ�ļ���
			else
			{
				for (int i = 0;i < size;i++)
				{
					std::string tempFilePath=files[i].c_str();
					//��ȡ�ϴ�·���е��ļ���
					int pos=tempFilePath.find_last_of('/');
					string desfilename=tempFilePath.substr(pos+1);
					FtpPutFile(hftp,tempFilePath.c_str(),desfilename.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);		
				}
			}
		}
	}while(0);

	InternetCloseHandle(hftp);
	InternetCloseHandle(hint);

	cout<<"�ϴ��ļ����"<<endl;
	system("pause");

	return 0;
}


void GetFiles( string path, vector<string>& files )  
{  
	//�ļ����  
	long hFile = 0;  
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;  
	std::string p;  
	if((hFile = _findfirst(p.assign(path).append("/*").c_str(),&fileinfo))!= -1)  
	{
		do  
		{  
			//�����Ŀ¼,����֮  
			//�������,�����б�  
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

//UTF-8��GB2312��ת�� 
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

//GB2312��UTF-8��ת�� 
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