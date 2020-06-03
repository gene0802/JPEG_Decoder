#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <streambuf>
using namespace std;
// SOF,DQT, DHT,SOS maker 
const char* marker[4] = {"\xff\xc0","\xff\xdb","\xff\xc4","\xff\xda"};
const char* ma = "\xff\xc0";
//0xffc0,0xffdb,0xffc4,0xffda
class Reader{

    private:
        vector <string> DQT;
        vector <string> DHT;
        string SOF;
        string SOS;
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
            
            // segment string by marker
            size_t last_pos = 0;
            size_t n_pos = -1;
            size_t found = 0;
            char last_match ;
            
            while( found !=string::npos){
                found = str.find("\xff",n_pos+1);
                n_pos = found;
                // cout << n_pos <<" ,"<<last_pos<<endl;
                //n_pos++;
                if(str[n_pos+1] == '\x00')
                    continue;                

                if(last_match == '\xc0'){
                    SOF = str.substr(last_pos+2,n_pos-(last_pos+2));
                }else if(last_match == '\xdb'){
                    DQT.push_back ( str.substr(last_pos+2,n_pos-(last_pos+2)));
                    //printf("\n\n");
                }else if(last_match == '\xc4'){
                    DHT.push_back ( str.substr(last_pos+2,n_pos-(last_pos+2)));
                    //printf("\n\n");
                }else if(last_match == '\xda'){
                    SOS = str.substr(last_pos+2,n_pos-(last_pos+2));
                }

                last_match = str[n_pos+1];
                last_pos = n_pos;
            }
            //debug
            for(int i=0;i<SOF.size();i++)
                    printf("%2x ", SOF[i] & 0xff);
                printf("\n\n");

            for(int k=0;k<DQT.size();k++){
                for(int i=0;i<DQT[k].size();i++)
                    printf("%2x ", DQT[k][i] & 0xff);
                printf("\n");
            }
            printf("\n\n");

            for(int k=0;k<DHT.size();k++){
                for(int i=0;i<DHT[k].size();i++)
                    printf("%2x ", DHT[k][i] & 0xff);
                printf("\n");
            }
            printf("\n\n");

             for(int i=0;i<SOS.size();i++)
                    printf("%2x ", SOS[i] & 0xff);
                printf("\n\n");
        } 

};

int main(){
    Reader reader("./2020_JPEG/monalisa.jpg");
}