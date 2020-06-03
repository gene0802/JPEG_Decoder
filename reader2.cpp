#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <math.h>
using namespace std;
// SOF,DQT, DHT,SOS maker 
const char* marker[4] = {"\xff\xc0","\xff\xdb","\xff\xc4","\xff\xda"};
const char* ma = "\xff\xc0";
//0xffc0,0xffdb,0xffc4,0xffda
class Reader{

    private:
        // SOF

        struct SOFst{
            int height;
            int width;
            struct Channel{
                unsigned char id;
                unsigned char sampleRate;
                unsigned char DQTid;
                Channel (){};
                Channel(char a,char b, char c):id(a),sampleRate(b),DQTid(c){};
            }Y,Cb,Cr;
            SOFst (){};
            SOFst (string s):height( (unsigned char)s[2]<<8 |(unsigned char) s[3] ),width((unsigned char)s[4]<<8 |(unsigned char)s[5]),Y(s[7],s[8],s[9]),Cb(s[10],s[11],s[12]),Cr(s[13],s[14],s[15]){};
        }SOF;

        //DQT
        struct DQTst{
            int DQT_length;
            char DQT_head;
            vector <string> QT;
        }DQT;
        
        //DHT
        struct DHTst{
            int DHT_length;
            char DHT_head;
            vector <string> HT;
        }DHT;

        //SOS
        struct SOSst{
            struct Channel{
                char id;
                char DQTid;
                Channel (){};
                Channel(char a,char b):id(a),DQTid(b){};
            }Y,Cb,Cr;
            
            struct MCU{
                struct Block{
                vector <vector <unsigned char> > v; 
                    Block(){};
                    Block(string s,int &start){
                        v.assign(8,vector<unsigned char>(8,0));
                        //cout << "start:"<<endl;
                        for(int i=0;i<8;i++){
                            for(int j=0;j<8;j++){
                               // cout << start<<" ";
                                v[i][j] = s[start++];
                                if (s[start]=='\xff') // ff follow 00 ,
                                    start++;
                            }
                        }
                    };
                };
                vector <Block> Y;
                vector <Block> Cb;
                vector <Block> Cr;
                MCU(){

                }
                MCU(string s,int &start,int Y_c,int Cb_c,int Cr_c){
                    //cout <<"here1"<<endl;
                    for(int i=0;i<Y_c;i++){
                        Block b(s,start);
                        Y.push_back(b);
                    }
                    for(int i=0;i<Cb_c;i++){
                        Block b(s,start);
                        Cb.push_back(b);
                    }
                    for(int i=0;i<Cr_c;i++){
                        Block b(s,start);
                        Cr.push_back(b);
                    }
                }
            };
            vector <vector <MCU> > mcu;
            
            SOSst (){};
            SOSst (string s,int h,int w,int Y_SRate,int Cb_SRate ,int Cr_SRate):Y(s[2],s[3]),Cb(s[4],s[5]),Cr(s[6],s[7]){

                s = s.substr(12);

                //cout <<"s length:"<<s.size()<<endl;
                int maxRate;
                maxRate = max(Y_SRate,Cb_SRate);
                maxRate = max(maxRate,Cr_SRate);
                int mcu_H = ceil((float)h / (float)maxRate) ;
                int mcu_W = ceil((float)w / (float)maxRate) ;
                mcu.assign(mcu_H,vector<MCU>(mcu_W));
                int start =0;
                for(int i=0;i<mcu_H;i++){
                    for(int j=0;j<mcu_W;j++){
                        mcu[i][j] = MCU(s,start,Y_SRate,Cb_SRate,Cr_SRate);
                        //cout << "i,j:"<<i<<" "<<j<<endl;
                    }
                }
            };
        }SOS;


    public:
        Reader(const char* inFile){
            
            // read whole file to string
            ifstream fl(inFile);
            fl.seekg( 0, ios::end );  
            size_t len = fl.tellg();  
            char *ret = new char[len];  
            fl.seekg(0, ios::beg);   
            fl.read(ret, len);  
            fl.close();
            string str(ret,ret+len);

            // for(int i=0;i<len;i++)
            //     printf("%2x ", ret[i] & 0xff);

            
            // segment string by marker
            size_t last_pos = 0;
            size_t n_pos = -1;
            size_t found = 0;
            char last_match ;
            
            while( found !=string::npos){
                found = str.find("\xff",n_pos+1);
                n_pos = found;

                if(str[n_pos+1] == '\x00')
                    continue;                

                if(last_match == '\xc0'){
                    SOF = SOFst(str.substr(last_pos+3,n_pos-(last_pos+3)));
                }else if(last_match == '\xdb'){
                    DQT.DQT_length = str.substr(last_pos+3,1)[0];
                    DQT.DQT_head = str.substr(last_pos+4,1)[0];
                    DQT.QT.push_back ( str.substr(last_pos+5,n_pos-(last_pos+5)));
                    //printf("\n\n");
                }else if(last_match == '\xc4'){
                    DHT.DHT_length = str.substr(last_pos+3,1)[0];
                    DHT.DHT_head = str.substr(last_pos+4,1)[0];
                    DHT.HT.push_back ( str.substr(last_pos+5,n_pos-(last_pos+5)));
                    //printf("\n\n");
                }else if(last_match == '\xda'){
                
                    //SOS = SOSst(str.substr(last_pos+3,n_pos-(last_pos+3)),SOF.height ,SOF.width,SOF.Y.sampleRate,SOF.Cb.sampleRate,SOF.Cr.sampleRate);
                }

                last_match = str[n_pos+1];
                last_pos = n_pos;
            }
            //debug
            printf("SOF:\n");
            printf("\theight: %d\n ", SOF.height);
            printf("\twidtgh: %d\n ", SOF.width);
            printf("\tY id: %2x\n ", SOF.Y.id & 0xff);
            printf("\tY sampleRate: %2x\n ", SOF.Y.sampleRate & 0xff);
            printf("\tY DQTid: %2x\n ", SOF.Y.DQTid & 0xff);
            printf("\tCb id: %2x\n ", SOF.Cb.id & 0xff);
            printf("\tCb sampleRate: %2x\n ", SOF.Cb.sampleRate & 0xff);
            printf("\tCb DQTid: %2x\n ", SOF.Cb.DQTid & 0xff);
            printf("\tCr id: %2x\n ", SOF.Cr.id & 0xff);
            printf("\tCr sampleRate: %2x\n ", SOF.Cr.sampleRate & 0xff);
            printf("\tCr DQTid: %2x\n ", SOF.Cr.DQTid & 0xff);
            printf("\n\n");

            printf("DQT: \n");
            for(int k=0;k<DQT.QT.size();k++){
                printf("count: %d\n",DQT.QT[k].size());
                for(int i=0;i<DQT.QT[k].size();i++)
                    printf("%2x ", DQT.QT[k][i] & 0xff);
                printf("\n\n");
            }
            printf("\n\n");
            printf("DHT:\n");
            for(int k=0;k<DHT.HT.size();k++){
                printf("count: %d\n",DHT.HT[k].size());
                for(int i=0;i<DHT.HT[k].size();i++)
                    printf("%2x ", DHT.HT[k][i] & 0xff);
                printf("\n\n");
            }
            printf("\n\n");

            printf("SOS:\n");
            printf("\tY id: %2x\n ", SOS.Y.id & 0xff);
            printf("\tY DQTid: %2x\n ", SOS.Y.DQTid & 0xff);
            printf("\tCb id: %2x\n ", SOS.Cb.id & 0xff);
            printf("\tCb DQTid: %2x\n ", SOS.Cb.DQTid & 0xff);
            printf("\tCr id: %2x\n ", SOS.Cr.id & 0xff);
            printf("\tCr DQTid: %2x\n ", SOS.Cr.DQTid & 0xff);
            printf("\n\n");
        } 

};

// int main(){
//     Reader reader("./2020_JPEG/teatime.jpg");
// }