#include "reader.cpp"
#include "parser.cpp"
#include "decoder.cpp"
#include <time.h> 

using namespace std;

int main(int argc,char* argv[]){
    // for(int l=0,x=l*2+1; l<8; l++,x+=2){
    //     for(int k=0,y=l*2+1; k<8; k++,x+=2)
    //         for(int u=0;u<8;u++)
    //             for(int v=0;v<8;v++)
    //                 printf("(%d,%d,%d,%d): %.8f\n",l,k,u,v,cos(x*u*M_PI/16.0)* cos(y*v*M_PI/16.0));
    // }
    // printf("{");
    // for(int l=0,x=l*2+1; l<8; l++,x+=2){
    //         printf("{");
    //         for(int u=0;u<8;u++){
    //             if (l==0 && u==0)
    //                 printf("%.10f,",cos(x*u*M_PI/16.0)*0.5);
    //             else if (l==0)
    //                 printf("%.10f,",(cos(x*u*M_PI/16.0)*0.7071067811865476));
    //             else 
    //                  printf("%.10f,",cos(x*u*M_PI/16.0));
    //         }
    //         printf("}\n");
    // }
    // printf("}");

    clock_t start, end;
    double cpu_time_used;

    Data data ;
    Reader* reader = new Reader(argv[1]);
    //Reader* reader = new Reader("./2020_JPEG/monalisa.jpg");
    Parser parser(&data);
    Decoder decoder(&data);
    start = clock();
    reader->segment();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" segment  time :%f\n",cpu_time_used);

    start = clock();
    parser.parseSOF(reader->getSOF());
    parser.parseDQT(reader->getDQTs());
    parser.parseDHT(reader->getDHTs());
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" parser1   time :%f\n",cpu_time_used);

    

    start = clock();
    parser.parseSOS(reader->getSOS());
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" parser2   time :%f\n",cpu_time_used);


    start = clock();
    decoder.invQuantize();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" invQ   time :%f\n",cpu_time_used);


    start = clock();
    decoder.invZigZag();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" invZig   time :%f\n",cpu_time_used);

    start = clock();
    decoder.invDCT();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" invDCT   time :%f\n",cpu_time_used);
    
    start = clock();
    decoder.reSample();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" resample  time :%f\n",cpu_time_used);

    start = clock();
    decoder.toRGB();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" toRGB  time :%f\n",cpu_time_used);


    //data.printData("./monalisa_data/monalisa_parse.txt");

    return 0;

}