sudo apt-get install pthread
sudo apt-get install libpthread-stubs0-dev
brew install cmake
sudo apt-get update -y
sudo apt-get install cmake build-essential colordiff git doxygen -y
sudo git clone https://github.com/mavlink/MAVSDK.git
cd MAVSDK
sudo git checkout master
sudo git checkout develop
sudo git submodule update --init --recursive
sudo cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON -Bbuild/default -H.
sudo cmake --build build/default
sudo cmake --build build/default --target install # sudo is required to install to system directories!
sudo ldconfig
cp ~/MAVSDK/src/core/*.h /usr/local/include/mavsdk/plugins/action/


