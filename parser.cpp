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
#include "data.cpp"
using namespace std;

//SOF,DQT, DHT,SOS
class Parser{
    private:
        Data* data;
    public:
        Parser(Data *d){
            data = d;
        }
        bool parseSOF(const string &s){
            data-> height = (unsigned char)s[2]<<8 |(unsigned char) s[3];
            data-> width  = (unsigned char)s[4]<<8 |(unsigned char) s[5];
            
            data-> Y_inform.id = s[7];
            data-> Y_inform.sRateH =(unsigned char)s[8]>>4;
            data-> Y_inform.sRateV =(unsigned char)s[8]%16;
            data-> Y_inform.DQT_ID = s[9];

            data-> Cb_inform.id = s[10];
            data-> Cb_inform.sRateH =(unsigned char)s[11]>>4;
            data-> Cb_inform.sRateV =(unsigned char)s[11]%16;
            data-> Cb_inform.DQT_ID = s[12];

            data-> Cr_inform.id = s[13];
            data-> Cr_inform.sRateH =(unsigned char)s[14]>>4;
            data-> Cr_inform.sRateV =(unsigned char)s[14]%16;
            data-> Cr_inform.DQT_ID = s[15];
            return true;
        }
        bool parseDQT(const vector <string> & sVec){
            for(int i=0;i<sVec.size();i++)
            {
                Qtable qT;
                qT.size = (unsigned char)sVec[i][1] >>4;
                qT.id = (unsigned char)sVec[i][1] %16;

                if (qT.size==0){
                    for(int j=0;j<8;j++)
                        for (int k=0;k<8;k++)
                        qT.v[j][k] = sVec[i][1+j*8+k];
                }else{
                    for(int j=0;j<8;j++)
                        for (int k=0;k<8;k++)
                        qT.v[j][k] = (unsigned int)sVec[i][1+(j*8+k)*2] <<4 + (unsigned int)sVec[i][1+(j*8+k)*2+1];
                }
                data->qTables.push_back(qT);
            }
            
            return true;
        }
        bool parseDHT(const vector <string> & sVec){
            return true;
        }
        bool parseSOS(const string &s){
            return true;
        }
        
};