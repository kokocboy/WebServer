#include<iostream>
#include<fstream>
#include<string.h>
using namespace std;
string get_file()
{
    filebuf *pbuf;
    ifstream filestr;
    long size;
    char * buffer;
    filestr.open ("content", ios::binary);
    // 获取filestr对应buffer对象的指针 
    pbuf=filestr.rdbuf();
  
    // 调用buffer对象方法获取文件大小
    size=pbuf->pubseekoff (0,ios::end,ios::in);
    pbuf->pubseekpos (0,ios::in);
    // 分配内存空间 +1是关键，不然又些文件会乱码
    buffer=new char[size+1];
    // 获取文件内容
    pbuf->sgetn (buffer,size);
    buffer[size]='\0';//0这是关键
    filestr.close();
    // 输出到标准输出
    string htmlContent;
    for(int i=0;i<size;i++)
        htmlContent+=buffer[i];
    delete []buffer;
    return htmlContent;
}
int main()
{
    string str="\n\r";
    for(auto it:str)
        cout<<(int)it<<endl;



    return 0;
}