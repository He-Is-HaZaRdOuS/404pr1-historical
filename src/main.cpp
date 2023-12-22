#include <CSV.h>
#include <main.h>
#include <iostream>
#include <fstream>
#include <string>
typedef std::string String;
typedef std::fstream fstream;

int main(){
    /*
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
    std::cout << SOMEIMPORTANTCONSTANT << std::endl;

    if (__cplusplus == 202101L) std::cout << "C++23";
    else if (__cplusplus == 202002L) std::cout << "C++20";
    else if (__cplusplus == 201703L) std::cout << "C++17";
    else if (__cplusplus == 201402L) std::cout << "C++14";
    else if (__cplusplus == 201103L) std::cout << "C++11";
    else if (__cplusplus == 199711L) std::cout << "C++98";
    else std::cout << "pre-standard C++." << __cplusplus;
    std::cout << "\n";
    */

    CSV test{RESOURCES_PATH "test.csv"};
    test.printRows();

    return 0;
}
