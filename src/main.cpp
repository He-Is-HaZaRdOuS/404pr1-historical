#include <iostream>
#include <fstream>
#include <string>
typedef std::string String;
typedef std::fstream fstream;

int main(){
    std::cout << "Hello CMake!\n";

    fstream fileP;
    fileP.open(RESOURCES_PATH "sampletext.txt", std::ios::in);

    if(fileP.is_open()) {
        String buf;
        while(getline(fileP, buf)) {
            std::cout << buf << "\n";
        }
        fileP.close();
    }

    return 0;
}
