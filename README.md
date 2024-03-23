This is now deprecated. </br>
Development moved to a [New repository](https://github.com/rhyrak/go-schedule) && [My fork](https://github.com/He-Is-HaZaRdOuS/go-schedule)

# 404pr1
Placeholder repository for CENG404's project. </br>
https://github.com/He-Is-HaZaRdOuS/404pr1 </br>
</br>
Originally derived from [CENG303's Term project](https://github.com/He-Is-HaZaRdOuS/303pr1)

# TODO
- [ ] Re-write the Solution class to be faster and neater
- [ ] Use more C++ STL features
- [ ] Parallelize any repetitive independent operation wherever possible
- [ ] Make an interactive GUI 
- [ ] Implement networking to allow for communication with the outside world
- [ ] TBD...

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
gcc --version
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
The CMakeLists.txt is currently set up in such a way which is suitable for a development environment such as CLion or VisualStudio IDE's. </br>
Consquently, the generated executable is hard linked to the resource path of the project's current directory and thus, is not portable. </br>
The solution for this is to open the CMakeLists.txt file and comment out [this line](https://github.com/He-Is-HaZaRdOuS/404pr1/blob/76107977cdbff54f7d173d3d3c52d21d62819a51/CMakeLists.txt#L35) and uncomment [this line](https://github.com/He-Is-HaZaRdOuS/404pr1/blob/76107977cdbff54f7d173d3d3c52d21d62819a51/CMakeLists.txt#L36). </br>
This effectively changes the RESOURCES_PATH macro to be relative to the current path of the executable, making it portable (Release Ready). </br>
Now re-compile the project by running the build command(s) from the previous section. </br>
The only thing left to do is to copy the res/ folder from the project root and paste it into the directory of the new executable. </br>
To run the executable, open a terminal/powershell window and type </br>
```bash
./404pr1 <N-value> <course-list.csv> <classrooms.csv> <?blocked.csv>
```
Or type the following for help
```bash
./404pr1 -h
```
The following explains the arguments and their format.
* N-value: Number of iterations to find optimal schedule.
* course-list.csv: CSV file containing course lists. </br> StudentID,Professor Name,CourseID,ExamDuration(in mins)
* classrooms.csv: CSV file containing classrooms. (RoomID,Capacity)
* blocked.csv: (Optional) CSV file containing blocked hours. (Day,TimeInterval,Course)
</br>

## IDE's
#### CLion (Stable/Nova)
Open the project folder in CLion and configure CMake presets. </br>

#### Visual Studio (2022)
Be sure to have installed the CMake build tools from the Visual installer. </br>
Open the project folder in explorer, shift-right click and open with Visual Studio. </br>
