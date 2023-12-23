#include "main.h"
#include "CSV.h"
#include "Timeslot.h"
#include "Typedefs.h"
#include <iostream>
#include <fstream>

#define courseColumnSize 4
#define classroomColumnSize 2

std::vector<std::vector<String>> courseData;

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

    Timeslot* table[7][108];
    CSV test{RESOURCES_PATH "course-list1.csv"};
    courseData = test.getData();

    table[0][0] = new Timeslot();
    table[0][0]->currentCourse = new Course(courseData.at(0).at(1),
        courseData.at(0).at(2),
        stoi(courseData.at(0).at(3)));

    std::cout << table[0][0]->currentCourse->code << std::endl;
    std::cout << table[0][0]->currentCourse->professorName << std::endl;
    std::cout << table[0][0]->currentCourse->examDuration << std::endl;

    /*
    for(int i = 0; i < courseColumnSize; i++)
        std::cout << courseData.at(3196).at(i) << " ";
    std::cout << std::endl;
    std::cout << courseData.capacity();
    */


    return 0;
}
