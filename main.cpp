#include "reader.cpp"
#include "parser.cpp"

using namespace std;

int main(){


    Data data ;
    Reader* reader = new Reader("./2020_JPEG/teatime.jpg");
    Parser parser(&data);

    reader->segment();
    //
    //reader->printByteArray("./teatime_data/teatime_byte.txt");
    parser.parseSOF(reader->getSOF());
    parser.parseDQT(reader->getDQTs());
    //
    data.printData("./teatime_data/teatime_parse.txt");

    return 0;

}