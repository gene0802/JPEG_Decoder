#ifndef DATACPP
#define DATACPP

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <math.h>
#include <unordered_map>
#include <bitset>
using namespace std;

class Channel{
    public:
        unsigned char id;
        unsigned char sRateH;
        unsigned char sRateV;
        unsigned char DQT_ID;
        unsigned char DHT_ID_DC;
        unsigned char DHT_ID_AC;
};
class RGB{
    public:
        unsigned char r,b,g;
        RGB(){};
        RGB(unsigned char r,unsigned char g,unsigned char b):r(r),g(g),b(b){};
};
class YCbCr{
    public:
        int r,b,g;
        YCbCr(){};
        YCbCr(int r,int g,int b):r(r),g(g),b(b){};
};
class Block{
    public:
        short v[8][8]={0}; 
    Block(){};
};
class BlockF{
    public:
        float v[8][8]={0}; 
    BlockF(){};
};
class MCU{
    public:
        vector <Block> Y;
        vector <Block> Cb;
        vector <Block> Cr;
        MCU(){}
        MCU(int YNum,int CbNum,int CrNum){
            Y.assign(YNum,Block());
            Cb.assign(CbNum,Block());
            Cr.assign(CrNum,Block());
        }
};
class MCU_F{
    public:
        vector <BlockF> Y;
        vector <BlockF> Cb;
        vector <BlockF> Cr;
        MCU_F(){}
        MCU_F(int YNum,int CbNum,int CrNum){
            Y.assign(YNum,BlockF());
            Cb.assign(CbNum,BlockF());
            Cr.assign(CrNum,BlockF());
        }
};
class Qtable{
    public:
        unsigned char id;
        unsigned char size;
        int v[8][8];
};
class Data{
    public:
        int height;
        int width;
        int height_MCU;
        int width_MCU;
        int sRateV_Max;
        int sRateH_Max;
        
        Channel Y_inform,Cb_inform,Cr_inform;
        unordered_map<string,int> Huffman[2][2]; //exact 4  DC:00,01  AC:10,11 // map code to word
        vector < Qtable > qTables; //at most 4

        vector <vector <Block>> Y;
        vector <vector <Block>> Cb;
        vector <vector <Block>> Cr;

        vector <vector <RGB>> color;
        vector <vector <YCbCr>> YUV;
        
        
        void printData(const char *outFile){
            FILE *ofp = fopen(outFile,"w");
            
            fprintf(ofp,"height: %d\n",height);
            fprintf(ofp,"width: %d\n",width);
            fprintf(ofp,"height_MCU: %d\n",height_MCU);
            fprintf(ofp,"width_MCU: %d\n",width_MCU);
            fprintf(ofp,"sRateV_Max: %d\n",sRateV_Max);
            fprintf(ofp,"sRateH_Max: %d\n",sRateH_Max);


            fprintf(ofp,"Y channel: \n");
            fprintf(ofp,"\tid:  %u\n",    Y_inform.id);
            fprintf(ofp,"\tsRateH:  %u\n", Y_inform.sRateH);
            fprintf(ofp,"\tsRateV:  %u\n", Y_inform.sRateV);
            fprintf(ofp,"\tDQT_ID:  %u\n",Y_inform.DQT_ID);
            fprintf(ofp,"\tDHT_ID_DC:  %u\n",Y_inform.DHT_ID_DC);
            fprintf(ofp,"\tDHT_ID_AC:  %u\n",Y_inform.DHT_ID_AC);
            fprintf(ofp,"Cb channel: \n");
            fprintf(ofp,"\tid:  %u\n",    Cb_inform.id);
            fprintf(ofp,"\tsRateH:  %u\n", Cb_inform.sRateH);
            fprintf(ofp,"\tsRateV:  %u\n", Cb_inform.sRateV);
            fprintf(ofp,"\tDQT_ID:  %u\n",Cb_inform.DQT_ID);
            fprintf(ofp,"\tDHT_ID_DC:  %u\n",Cb_inform.DHT_ID_DC);
            fprintf(ofp,"\tDHT_ID_AC:  %u\n",Cb_inform.DHT_ID_AC);
            fprintf(ofp,"Cr channel: \n");
            fprintf(ofp,"\tid:  %u\n",    Cr_inform.id);
            fprintf(ofp,"\tsRateH:  %u\n", Cr_inform.sRateH);
            fprintf(ofp,"\tsRateV:  %u\n", Cr_inform.sRateV);
            fprintf(ofp,"\tDQT_ID:  %u\n",Cr_inform.DQT_ID);
            fprintf(ofp,"\tDHT_ID_DC:  %u\n",Cr_inform.DHT_ID_DC);
            fprintf(ofp,"\tDHT_ID_AC:  %u\n",Cr_inform.DHT_ID_AC);
            for (int i=0;i<qTables.size();i++){
                fprintf(ofp,"QTable %u \n",qTables[i].id);
                fprintf(ofp,"\t size :%u \n",qTables[i].size);
                for(int j=0;j<8;j++){
                        fprintf(ofp,"\t");
                    for(int k=0;k<8;k++){
                        fprintf(ofp,"%3d ",qTables[i].v[j][k]);
                    }
                        fprintf(ofp,"\n");
                }
            }
            char buffer [33];
            
            for(int i=0;i<2;i++){
                for(int j=0;j<2;j++){
                    fprintf(ofp,"Huffman %d,%d\n ",i,j);
                    for(auto it = Huffman[i][j].cbegin(); it != Huffman[i][j].cend(); ++it){
                        fprintf(ofp,"\t%16s => %4d\n",it->first.c_str(),it->second);
                    }
                }
            }

            int row = 1+(height - 1)/height_MCU;
            int col = 1+(width - 1)/width_MCU;
            for (int i=0; i<row;i++){
                for (int j=0; j<col;j++){
                    fprintf(ofp,"MCU (%d,%d) :\n",i,j);
                    for (int m=0;m<Y_inform.sRateV;m++){
                        for(int n=0;n<Y_inform.sRateH;n++){
                            fprintf(ofp,"\t Y (%d,%d) :\n",m,n);
                            for(int k=0;k<8;k++){
                                fprintf(ofp,"\t\t");
                                for(int l=0;l<8;l++){
                                    fprintf(ofp,"%4d ",Y[Y_inform.sRateV*i+m][Y_inform.sRateH*j+n].v[k][l]);
                                }
                                fprintf(ofp,"\n");
                            }
                        }
                    }
                    for (int m=0;m<Cb_inform.sRateV;m++){
                        for(int n=0;n<Cb_inform.sRateH;n++){
                            fprintf(ofp,"\t Cb (%d,%d) :\n",m,n);
                            for(int k=0;k<8;k++){
                                fprintf(ofp,"\t\t");
                                for(int l=0;l<8;l++){
                                    fprintf(ofp,"%4d ",Cb[Cb_inform.sRateV*i+m][Cb_inform.sRateH*j+n].v[k][l]);
                                }
                                fprintf(ofp,"\n");
                            }
                        }
                    }
                    for (int m=0;m<Cr_inform.sRateV;m++){
                        for(int n=0;n<Cr_inform.sRateH;n++){
                            fprintf(ofp,"\t Cr (%d,%d) :\n",m,n);
                            for(int k=0;k<8;k++){
                                fprintf(ofp,"\t\t");
                                for(int l=0;l<8;l++){
                                    fprintf(ofp,"%4d ",Cr[Cr_inform.sRateV*i+m][Cr_inform.sRateH*j+n].v[k][l]);
                                }
                                fprintf(ofp,"\n");
                            }
                        }
                    }
                }
            }
            fprintf(ofp,"\nYUV  :\n");
            for(int i=0;i<height;i++){
                fprintf(ofp,"\t height:%d \n",i);
                for(int j=0;j<width;j++){
                    fprintf(ofp,"%3d,%3d:(%3d,%3d,%3d):\n",i,j,YUV[i][j].r,YUV[i][j].g,YUV[i][j].b);
                }
            }

            fprintf(ofp,"\nRGB  :\n");
            for(int i=0;i<height;i++){
                fprintf(ofp,"\t height:%d \n",i);
                for(int j=0;j<width;j++){
                    fprintf(ofp,"%3d,%3d:(%d,%d,%d) :\n",i,j,color[i][j].r,color[i][j].g,color[i][j].b);
                }
            }
          
            fclose(ofp);
        }
};    
#endif