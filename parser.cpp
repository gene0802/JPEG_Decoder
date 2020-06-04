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
        void parseSOF(const string &s){
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
            return ;
        }
        void parseDQT(const vector <string> & sV){
            vector <string> sVec = sV;

            for(int i=0;i<sVec.size();i++)
            {   
                int remainLen = (unsigned char)sVec[i][0]-2;
                sVec[i] = sVec[i].substr(1);

                while(remainLen >0){
                    int lenCount=0;
                    Qtable qT;
                    qT.size = (unsigned char)sVec[i][0] >>4;
                    qT.id = (unsigned char)sVec[i][0] %16;
                    lenCount++;

                    if (qT.size==0){
                        for(int j=0;j<8;j++)
                            for (int k=0;k<8;k++){
                                qT.v[j][k] = sVec[i][1+j*8+k];
                                lenCount++;
                            }
                    }else{
                        for(int j=0;j<8;j++)
                            for (int k=0;k<8;k++){
                                qT.v[j][k] = (unsigned int)sVec[i][1+(j*8+k)*2] <<4 + (unsigned int)sVec[i][1+(j*8+k)*2+1];
                                lenCount+=2;
                            }
                    }
                    data->qTables.push_back(qT);

                    sVec[i] = sVec[i].substr(lenCount);
                    remainLen -= lenCount;
                }
            }
            
            return ;
        }
        void parseDHT(const vector <string> & sV){
            vector <string> sVec = sV;
            
            for(int i=0;i<sVec.size();i++)
            {   
                //cout << (( (unsigned char) sVec[i][0])<<8) <<","<<(int)(unsigned char)sVec[i][1]<<endl;
                int remainLen = (int)(((unsigned char)sVec[i][0])<<8) + (int)((unsigned char)sVec[i][1])-2;
                sVec[i] = sVec[i].substr(2);
                //cout <<remainLen<<endl;
                while(remainLen >0){
                    int lenCount=0;
                    int ind_0 = (unsigned char)sVec[i][0] >> 4 ;
                    int ind_1 = (unsigned char)sVec[i][0] % 16 ;
                    sVec[i] = sVec[i].substr(1);
                    remainLen--;

                    int leaf = 0 ;
                    int cValue = 0;
                    for(int level=0;level<16;level++){
                        for(int j=0;j<(unsigned char)sVec[i][level];j++){
                            data->Huffman[ind_0][ind_1][cValue] =  (unsigned char)sVec[i][16+leaf];
                            //cout << data->Huffman[ind_0][ind_1][cValue] <<"  "<< (unsigned char)sVec[i][16+leaf]<<endl;
                            cValue ++;
                            leaf++;
                        }
                        cValue *=2;
                    }
                    sVec[i] = sVec[i].substr(16+leaf);
                    remainLen -= (16+leaf);             
                }
            }
            return ;
        }

        void parseSOS(const string &s){
            
            
            
            return ;
        }
        
        
};