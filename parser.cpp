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
#include "data.cpp"
using namespace std;

const int mask[8] = {128,64,32,16,8,4,2,1};
//SOF,DQT, DHT,SOS
class Parser{
    private:
        Data* data;
        
        // for scan data usage (pointer to scanned bit)
        void parseData(string &s){
            
            int row = 1+(data->height - 1)/data->height_MCU;
            int col = 1+(data->width - 1)/data->width_MCU;

            int Y_SRate = data->Y_inform.sRateH * data->Y_inform.sRateV;
            int Cb_SRate = data->Cb_inform.sRateH * data->Cb_inform.sRateV;
            int Cr_SRate = data->Cr_inform.sRateH * data->Cr_inform.sRateV;


            //for MCU i,j
            for (int i=0; i<row;i++){
                for (int j=0; j<col;j++){
                    // Parse  Y
                    
                    for (int m=0;m<data->Y_inform.sRateV;m++){
                        for(int n=0;n<data->Y_inform.sRateH;n++){
                            //cout <<"Y:"<<(data->Y_inform.sRateH*i+m)<<","<<(data->Y_inform.sRateV*j+n)<<endl;
                            parseBlock(s,data->Huffman[0][data->Y_inform.DHT_ID_DC],data->Huffman[1][data->Y_inform.DHT_ID_AC],data->Y[data->Y_inform.sRateV*i+m][data->Y_inform.sRateH*j+n],0);
                        }
                    }
                    //Parse  Cb
                    for (int m=0;m<data->Cb_inform.sRateV;m++){
                        for(int n=0;n<data->Cb_inform.sRateH;n++){
                            //cout <<"Cb:"<<(Cb_SRate*i+m)<<","<<(Cr_SRate*j+n)<<endl;
                            parseBlock(s,data->Huffman[0][data->Cb_inform.DHT_ID_DC],data->Huffman[1][data->Cb_inform.DHT_ID_AC],data->Cb[data->Cb_inform.sRateV*i+m][data->Cb_inform.sRateH*j+n],1);
                        }
                    }
                    //Parse Cr
                    for (int m=0;m<data->Cr_inform.sRateV;m++){
                        for(int n=0;n<data->Cr_inform.sRateH;n++){
                            //cout <<"Cr:"<<(Cr_SRate*i+m)<<","<<(Cr_SRate*j+n)<<endl;
                            parseBlock(s,data->Huffman[0][data->Cr_inform.DHT_ID_DC],data->Huffman[1][data->Cr_inform.DHT_ID_AC],data->Cr[data->Cr_inform.sRateV*i+m][data->Cr_inform.sRateH*j+n],2);
                        }
                    }
                }
            }
            //cout <<"here\n";
            return;
        }
        void parseBlock(string &s, unordered_map<string ,int>&hDC,unordered_map<string ,int>&hAC,Block &b, int type){
        // 1. get DC coff
            // (1)find T
            static int T;
            static int bitLoc =0;
            static short preY_DC=0,preCb_DC=0,preCr_DC=0;

            int diff=0;

            string codeH;

            do{
                codeH = codeH + (((bool) (s[bitLoc>>3] & mask[bitLoc++%8]))? "1":"0");
            }while(hDC.count(codeH)==0);

            T = hDC[codeH];

            //(2)find diff
            for(int i=0;i<T;i++)
                diff = (diff << 1) + (bool) (s[bitLoc>>3] & mask[bitLoc++%8]);

            diff = (diff >= (1<<T-1)) ? diff : diff+1-(1<<T); 
            //b.v[0][0] = diff;
            if (type == 0){
                b.v[0][0] = diff +preY_DC;
                preY_DC = b.v[0][0];
            }else if(type ==1){
                b.v[0][0] = diff +preCb_DC;
                preCb_DC = b.v[0][0];
            }else if(type ==2){
                b.v[0][0] = diff + preCr_DC;
                preCr_DC = b.v[0][0];
            }
            

        //2.get AC 63 coff
            int coffIdx=1;
            int coff;
            while(coffIdx < 64){
            // (1)find T
                coff = 0;
                
                codeH.clear();
                do{    
                    codeH = codeH + (((bool) (s[bitLoc>>3] & mask[bitLoc++%8]))? "1":"0");
                }while(hAC.count( codeH )==0);

                T = hAC[codeH];

            //(2)find coeff
                if (T == 0x00)  //0x00 means left AC coff are all zero 
                     break;
             
                if (T == 0xF0){ //0xF0 means current and next 15 AC are all zero
                    coffIdx+=16;
                    continue;
                }
                coff = 0;
                coffIdx += (T >>4);  
                int lowT = T & 0x0F;           
                for(int i=0;i<lowT;i++)
                    coff =  (coff << 1) + (bool) (s[bitLoc>>3] & mask[bitLoc++%8]);

                b.v[coffIdx/8][coffIdx%8] = (coff >= (1<<lowT-1)) ? coff : coff+1-(1<<lowT); 
                
                coffIdx++;
            }

            return;
        }
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

            int sRateH_Max = max (data-> Y_inform.sRateH,data-> Cb_inform.sRateH);
            sRateH_Max = max(sRateH_Max,(int)data-> Cr_inform.sRateH);
            int sRateV_Max = max (data-> Y_inform.sRateV,data-> Cb_inform.sRateV);
            sRateV_Max = max(sRateV_Max,(int)data-> Cr_inform.sRateV);

            data-> height_MCU = 8*sRateV_Max;
            data-> width_MCU = 8*sRateH_Max;
            data-> sRateH_Max = sRateH_Max;
            data-> sRateV_Max = sRateV_Max;

            //allocate Data memory
            // int row_MCU = (1+(data->height - 1)/data->height_MCU);
            // int col_MCU = (1+(data->width - 1)/data->width_MCU);
            // int YNum = data->Y_inform.sRateH * data->Y_inform.sRateV;
            // int CbNum= data->Cb_inform.sRateH * data->Cb_inform.sRateV;
            // int CrNum = data->Cr_inform.sRateH * data->Cr_inform.sRateV;
           
            data->Y.assign (data->height/(8*sRateV_Max/data->Y_inform.sRateV),vector<Block>(data->width/(8*sRateH_Max/data->Y_inform.sRateH)));
            data->Cb.assign (data->height/(8*sRateV_Max/data->Cb_inform.sRateV),vector<Block>(data->width/(8*sRateH_Max/data->Cb_inform.sRateH)));
            data->Cr.assign (data->height/(8*sRateV_Max/data->Cr_inform.sRateV),vector<Block>(data->width/(8*sRateH_Max/data->Cr_inform.sRateH)));
            data->color.assign(data->height,vector<RGB>(data->width));
            data->YUV.assign(data->height,vector<YCbCr>(data->width));
            //data->mcu.assign(row_MCU, vector<MCU>(col_MCU,MCU(YNum,CbNum,CrNum)));

            //data->mcu_f.assign(row_MCU, vector<MCU_F>(col_MCU,MCU_F(YNum,CbNum,CrNum)));
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
                            const int bitlen = level+1;
                            string code = bitset<16>((unsigned long long)cValue).to_string();
                            code =  code.substr(16-(level+1));
                            data->Huffman[ind_0][ind_1][code] =  (unsigned char)sVec[i][16+leaf];
                        
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
        
            data->Y_inform.DHT_ID_DC = (unsigned char)s[3]>>4;
            data->Y_inform.DHT_ID_AC= (unsigned char)s[3]%16;
            data->Cb_inform.DHT_ID_DC = (unsigned char)s[5]>>4;
            data->Cb_inform.DHT_ID_AC = (unsigned char)s[5]%16;
            data->Cr_inform.DHT_ID_DC = (unsigned char)s[7]>>4;
            data->Cr_inform.DHT_ID_AC = (unsigned char)s[7]%16;

            string dataS = s.substr(11);


            parseData(dataS);

            return ;
        }
        
        
};