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
//const char* marker[4] = {"\xff\xc0","\xff\xdb","\xff\xc4","\xff\xda"};

class Reader{

    private:
        string allText;
        string SOF;
        vector <string> DQTs;
        vector <string> DHTs;
        string SOS;
    public:
        Reader(const char* inFile){

            ifstream fl(inFile);
            fl.seekg( 0, ios::end );  
            size_t len = fl.tellg();  
            char *ret = new char[len];  
            fl.seekg(0, ios::beg);   
            fl.read(ret, len);  
            fl.close();
            allText = string(ret,ret+len);
        }

        void segment(){

            size_t last_pos = 0;
            size_t n_pos = -1;
            size_t found = 0;
            char last_match ;
            
            while( found !=string::npos){
                found = allText.find("\xff",n_pos+1);
                n_pos = found;

                if(allText[n_pos+1] == '\x00')
                    continue;                

                if(last_match == '\xc0'){
                    SOF = allText.substr(last_pos+3,n_pos-(last_pos+3));
                }else if(last_match == '\xdb'){
                    DQTs.push_back (allText.substr(last_pos+3,n_pos-(last_pos+3)));
                }else if(last_match == '\xc4'){
                    DHTs.push_back (allText.substr(last_pos+2,n_pos-(last_pos+2)));
                }else if(last_match == '\xda'){
                    SOS = allText.substr(last_pos+3,n_pos-(last_pos+3));
                }
                last_match = allText[n_pos+1];
                last_pos = n_pos;
            }
        }

        string getSOF(){
            return SOF;
        }
        vector <string> getDQTs(){
            return DQTs;
        }
        vector <string> getDHTs(){
            return DHTs;
        }
        string getSOS(){
            return SOS;
        }
        void printByteArray(const char* outFile){

            FILE * ofp = fopen(outFile,"w");

            for(int i=0;i<allText.size();i++)
                fprintf(ofp,"%2x ", allText[i] & 0xff);
            
            fclose(ofp);
        }
};