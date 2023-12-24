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

    Timeslot table[7][108];
    CSV test{RESOURCES_PATH "course-list1.csv"};
    courseData = test.getData();

    table[0][1].status = OCCUPIED;
    table[0][1].currentCourse.professorName = courseData.at(0).at(1);
    table[0][1].currentCourse.code = courseData.at(0).at(2);
    table[0][1].currentCourse.examDuration = std::stoi(courseData.at(0).at(3));
    table[0][1].currentCourse.studentCount = 0;

    std::cout << table[0][0].currentCourse.code << std::endl;
    std::cout << table[0][0].currentCourse.professorName << std::endl;
    std::cout << table[0][0].currentCourse.examDuration << std::endl;

    std::cout << table[0][1].currentCourse.code << std::endl;
    std::cout << table[0][1].currentCourse.professorName << std::endl;
    std::cout << table[0][1].currentCourse.examDuration << std::endl;

    int count = test.filter(2, [](const String&value) {
        return value == "CENG463";
    }).size();

    std::cout << "There are " << count << " students taking CENG463";


    /*
    for(int i = 0; i < courseColumnSize; i++)
        std::cout << courseData.at(3196).at(i) << " ";
    std::cout << std::endl;
    std::cout << courseData.capacity();
    */


    return 0;
}
