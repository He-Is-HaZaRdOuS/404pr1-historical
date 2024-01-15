# 303pr1
Placeholder repository for CENG303's Term project.

# Disclaimer!
CMake does not recognize non-english characters in the build path. </br>
If your computer's file structure contains any non-english characters then you won't be able to compile the program. </br>
Please place the project folder somewhere such that the absolute path to it won't contain any non-english characters. </br>

## Installing CMake (3.28.1)
### (Windows && MacOS) </br>
Download the respective binaries from the following link and install them. </br>
https://cmake.org/download/ </br>
</br>
### (Linux) </br>
Open a terminal window and execute the following commands line by line </br>

```bash
 version=3.28
 build=1
 limit=3.20
 result=$(echo "$version >= $limit" | bc -l)
 os=$([ "$result" == 1 ] && echo "linux" || echo "Linux")
 mkdir ~/temp
 cd ~/temp
 wget https://cmake.org/files/v$version/cmake-$version.$build-$os-x86_64.sh
 sudo mkdir /opt/cmake
 sudo sh cmake-$version.$build-$os-x86_64.sh --prefix=/opt/cmake #(Type "y" to accept the license agreement and type "n" to forego installing inside the subdirectory)
 cmake --version #(expected output is "cmake version 3.28.1") 
```
</br>

## Installing MinGW (gcc for Windows)
* Download and install the gcc compiler for Windows from the following link. </br>
* https://nuwen.net/files/mingw/mingw-18.0-without-git.exe </br>
* Extract to somewhere temporary and then move it inside the "C:\Program Files" directory. </br>
* Then update the System Environment Path Variables to point to the /bin directory of MinGW </br>
* to verify the compiler is detected, open a powershell window and type 
```bash
"gcc --version" 
```

## Compilation
### (Linux  && MacOS)
open a terminal window and cd into the project folder </br>

#### (Release)
```bash
 mkdir build-release
 cd build-release
 cmake -DCMAKE_BUILD_TYPE=Release ..
 cmake --build .
```

#### (Debug)
```bash
 mkdir build-debug
 cd build-debug
 cmake -DCMAKE_BUILD_TYPE=Debug ..
 cmake --build .
```

#### (Release With Debug Info)
```bash
 mkdir build-relwithdebinfo
 cd build-relwithdebinfo
 cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
 cmake --build .
```

#### (Release With Minimum Size)
```bash
 mkdir build-minsizerel
 cd build-minsizerel
 cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
 cmake --build .
```
</br>

the executable will be generated inside the respective build-X folder. </br>

### (Windows)
open a powershell window and cd into the project folder </br>
#### (Release)
```bash
 mkdir build-release
 cd build-release
 cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
 make
```

#### (Debug)
```bash
 mkdir build-debug
 cd build-debug
 cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
 make
```
#### (Release With Debug Info)
```bash
 mkdir build-relwithdebinfo
 cd build-relwithdebinfo
 cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
 make
```

#### (Release With Minimum Size)
```bash
 mkdir build-minsizerel
 cd build-minsizerel
 cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel ..
 make
```
</br>

the executable will be generated inside the respective build-X folder. </br>

## Running
The CMakeLists.txt is currently set up in such a way which generates a portable executable. </br>
Consquently, it is only really desirable to be used from a terminal window and not with an IDE. </br>
This is because the RESOURCE_PATH macro is set to be relative to the generated executable's location which is not usually the case while using an IDE. </br>
The solution is to edit the CMakeLists.txt file to change the mentioned macro to be hard-linked to the project's current directory. </br>
Simply open the file and comment out line 12 and uncomment line 11. </br>
Now re-compile the project by running the build command(s) from the previous section. </br>
To run the executable, open a terminal/powershell window and type </br>
```bash
./303pr1 <N-value> <course-list.csv> <classrooms.csv> <?blocked.csv>
```
Or type the following for help
```bash
./303pr1 -h
```
The following explains the arguments and their format.
* N-value: Number of iterations to find optimal schedule.
* course-list.csv: CSV file containing course lists. </br> StudentID,Professor Name,CourseID,ExamDuration(in mins)
* classrooms.csv: CSV file containing classrooms. (RoomID,Capacity)
* blocked.csv: (Optional) CSV file containing blocked hours. (Day,TimeInterval,Course)
</br>
</br>
Keep in mind that the .csv files must be placed inside a folder called "res" and for that folder to be located in the directory of the portable executable.
</br>
Otherwise using an IDE with the current project structure should automatically pick up the path provided the CMakeLists.txt has been altered and the .csv files are provided as launch arguments.

## IDE's
#### CLion (Stable/Nova)
Open the project folder in CLion and configure CMake presets. </br>

#### Visual Studio (2022)
Be sure to have installed the CMake build tools from the Visual installer. </br>
Open the project folder in explorer, shift-right click and open with Visual Studio. </br>

</br>

## Group Members

* 19050111022, FURKAN DEMİR, GH0ST-TF141
* 20050111011, İBRAHİM BAHÇA, rogrammer
* 20050111034, MERTER ÇOBAN, mertercoban
* 20050111008, SELÇUK GENÇAY, rhyrak
* 21050141038, YOUSIF HARITH SUHAIL SUHAIL, He-Is-HaZaRdOuS
