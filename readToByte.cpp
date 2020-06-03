#include <iostream>
#include <fstream>
#include <iomanip>  

using namespace std;


char* readFileBytes(const char *name,const char *out)  
{  
    ifstream fl(name);  
    fl.seekg( 0, ios::end );  
    size_t len = fl.tellg();  
    char *ret = new char[len];  
    fl.seekg(0, ios::beg);   
    fl.read(ret, len);  
    fl.close();

    for(int i=0;i<len;i++)
        printf("%2x ", ret[i] & 0xff);
    return ret;  
}  

int main(){
    char inFile[30] = "./2020_JPEG/monalisa.jpg";
    char outFile[30] = "./monalisa_byte.txt";
    readFileBytes(inFile,outFile);

    return 0;
}

