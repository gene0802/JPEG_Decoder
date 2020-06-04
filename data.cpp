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
#include <bitset>
using namespace std;

class Channel{
    public:
        unsigned char id;
        unsigned char sRateH;
        unsigned char sRateV;
        unsigned char DQT_ID;
};
class Block{
    public:
        unsigned char v[8][8]; 
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
        Channel Y_inform,Cb_inform,Cr_inform;
        map<int,int> Huffman[2][2]; //exact 4  DC:00,01  AC:10,11 // map code to word
        vector < Qtable > qTables; //at most 4
        vector <Block> Y;
        vector <Block> Cb;
        vector <Block> Cr;

        void printData(const char *outFile){
            FILE *ofp = fopen(outFile,"w");
            
            fprintf(ofp,"height: %d\n",height);
            fprintf(ofp,"width: %d\n",width);

            fprintf(ofp,"Y channel: \n");
            fprintf(ofp,"\tid:  %u\n",    Y_inform.id);
            fprintf(ofp,"\tsRateH:  %u\n", Y_inform.sRateH);
            fprintf(ofp,"\tsRateV:  %u\n", Y_inform.sRateV);
            fprintf(ofp,"\tDQT_ID:  %u\n",Y_inform.DQT_ID);
            fprintf(ofp,"Cb channel: \n");
            fprintf(ofp,"\tid:  %u\n",    Cb_inform.id);
            fprintf(ofp,"\tsRateH:  %u\n", Cb_inform.sRateH);
            fprintf(ofp,"\tsRateV:  %u\n", Cb_inform.sRateV);
            fprintf(ofp,"\tDQT_ID:  %u\n",Cb_inform.DQT_ID);
            fprintf(ofp,"Cr channel: \n");
            fprintf(ofp,"\tid:  %u\n",    Cr_inform.id);
            fprintf(ofp,"\tsRateH:  %u\n", Cr_inform.sRateH);
            fprintf(ofp,"\tsRateV:  %u\n", Cr_inform.sRateV);
            fprintf(ofp,"\tDQT_ID:  %u\n",Cr_inform.DQT_ID);

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
            for(int i=0;i<2;i++){
                for(int j=0;j<2;j++){
                    fprintf(ofp,"Huffman %d,%d\n ",i,j);
                    for(auto it = Huffman[i][j].cbegin(); it != Huffman[i][j].cend(); ++it)
                        fprintf(ofp,"\t%04x => %4d\n",it->first,it->second);
                }
            }
            
            fclose(ofp);
        }
};    
