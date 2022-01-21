#include <iostream>
#include <cstdio>
#include <fstream>
#include "byte_buffer2.hpp"
using namespace std;

// class BootRecord
// {
// public:
// BootRecord(char * buffer, int size)
// {
//     // Byte Buffer bb() buffer
// }
// };

int main(int argc, char* argv[])
{
    string filePath = "../../FAT32_simple.mdf";
    char buffer[0X60] = {0};

    // fopen
    ifstream readFile(filePath);

    // fread into buffer
    if (readFile.is_open()) {
        while(!readFile.eof()) {
            // readFile.getline(buffer, 0X60);
            readFile.get(buffer, 0X60);
            cout << hex << buffer << endl;
        }
        readFile.close();
    } else {
        cout << "file is not opend!" << endl;
    }

    // Bootrecord에 buffer 넣기
    // BootRecord(buffer, 0X60);

    return 0;
}