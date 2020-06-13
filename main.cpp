#include "reader.cpp"
#include "parser.cpp"
#include "decoder.cpp"
#include <time.h> 

using namespace std;

int main(int argc,char* argv[]){
    clock_t start, end;
    double cpu_time_used;

    Data data ;
    Reader* reader = new Reader(argv[1]);
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
    decoder.toRGB("output.bmp");
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" toRGB  time :%f\n",cpu_time_used);

    return 0;

}