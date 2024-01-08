# 303pr1
* Placeholder repository for CENG303's Term project.

## Installing CMake
Windows && MacOS: </br>
Download the respective binaries from the following link and install them. </br>
https://cmake.org/download/ </br>
</br>
Linux: </br>
Execute the following commands line by line </br>
* version=3.28 </br>
* build=1 </br>
* limit=3.20 </br>
* result=$(echo "$version >= $limit" | bc -l) </br>
* os=$([ "$result" == 1 ] && echo "linux" || echo "Linux") </br>
* mkdir ~/temp </br>
* cd ~/temp </br>
* wget https://cmake.org/files/v$version/cmake-$version.$build-$os-x86_64.sh  </br>
* sudo mkdir /opt/cmake </br>
* sudo sh cmake-$version.$build-$os-x86_64.sh --prefix=/opt/cmake  </br>
(Type "y" to accept the license agreement and type "n" to forego installing inside the subdirectory) </br>
* cmake --version (expected output is "cmake version 3.28.1") </br>
</br>

## Installing MinGW (gcc for Windows)
* Download and install the gcc compiler for Windows from the following link. </br>
* https://nuwen.net/files/mingw/mingw-18.0-without-git.exe </br>
* Extract to somewhere temporary and then move it inside the "C:\Program Files" directory. </br>
* Then update the System Environment Path Variables to point to the /bin directory of MinGW </br>
* Open a terminal and type "gcc --version" to verify the compiler is detected. </br>
</br>

## Compilation
### (Linux  && MacOS)
open a terminal and cd into the project folder </br>
* cmake -B build -S . </br>
* cmake --build build </br>
</br>
the executable will be generated inside the /build folder. </br>
</br>

### (Windows)
open a terminal and cd into the project folder </br>
* cmake -S . -B build -G "MinGW Makefiles" </br>
* cd /build </br>
* make </br>
</br>
the executable will be generated inside the /build folder. </br>
</br>

## Running
* The CMakeLists.txt is currently set up in such a way which is suitable for a development environment such as CLion or VisualStudio IDE's. </br>
* Consquently, the generated executable is hard linked to the resource path of the project's current directory and thus, is not portable. </br>
* The solution for this is to open the CMakeLists.txt file and comment line 11 and uncomment line 12. </br>
* This effectively changes the resource path to be relative to the current path of the executable, making it portable (Release Ready). </br>
* Now re-compile the project by running the build command from the previous section.
* The only thing left to do is to copy the /res folder from the project root and paste it in the directory of the executable. </br>
* To run the executable, open a terminal and type "./303pr1". </br>
</br>

## Group Members

* 19050111022, FURKAN DEMİR, GH0ST-TF141
* 20050111011, İBRAHİM BAHÇA, rogrammer
* 20050111034, MERTER ÇOBAN, mertercoban
* 20050111008, SELÇUK GENÇAY, rhyrak
* 21050141038, YOUSIF HARITH SUHAIL SUHAIL, He-Is-HaZaRdOuS
