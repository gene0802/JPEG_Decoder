#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <math.h>
#include <map>
using namespace std;

class Channel{
    unsigned char id;
    unsigned char sampleRate;
    unsigned char DQT_ID;
    unsigned char DQTid;
};
class Block{
   unsigned char v[8][8]; 
};
class Data{
    int height;
    int width;
    Channel Y,Cb,Cr;
    vector < map<int,int> >Huffman; //exact 4  DC,AC 0 1
    vector <vector <vector <int> > > QTable; //at most 4
    vector <Block> Y;
    vector <Block> Cb;
    vector <Block> Cr;
};
class SOF{

    public:
        int height;
        int width;
        struct Channel{
            unsigned char id;
            unsigned char sampleRate;
            unsigned char DQTid;
            Channel (){};
            Channel(char a,char b, char c):id(a),sampleRate(b),DQTid(c){};
        }Y,Cb,Cr;
        SOF (){};
        SOF (string s):height( (unsigned char)s[2]<<8 |(unsigned char) s[3] ),width((unsigned char)s[4]<<8 |(unsigned char)s[5]),Y(s[7],s[8],s[9]),Cb(s[10],s[11],s[12]),Cr(s[13],s[14],s[15]){};
};

class DQT{

    public:
        int DQT_length;
        char DQT_head;
        vector <string> QT;
};

class DHT{

    public:
        int DHT_length;
        char DHT_head;
        vector <string> HT;
};

class SOS{

    public:
        struct Channel{
            char id;
            char DQTid;
            Channel (){};
            Channel(char a,char b):id(a),DQTid(b){};
        }Y,Cb,Cr;
        string data;
        
};
           
