#include "reader.cpp"
#include "parser.cpp"

using namespace std;

int main(){


    Data data ;
    Reader* reader = new Reader("./2020_JPEG/teatime.jpg");
    //Reader* reader = new Reader("./2020_JPEG/monalisa.jpg");
    Parser parser(&data);

    reader->segment();
    //
    //reader->printByteArray("./teatime_data/teatime_byte.txt");
    //reader->printByteArray("./monalisa_data/monalisa_byte2.txt");
    parser.parseSOF(reader->getSOF());
    parser.parseDQT(reader->getDQTs());
    parser.parseDHT(reader->getDHTs());
    //
    data.printData("./teatime_data/teatime_parse.txt");
    //data.printData("./monalisa_data/monalisa_parse.txt");

    return 0;

}