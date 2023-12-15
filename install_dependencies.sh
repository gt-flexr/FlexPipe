# Dependency Installation

mkdir deps && cd deps

# 1. RaftLib
echo ********** Installing RaftLib **********
git clone https://github.com/gt-flexr/RaftLib.git && cd RaftLib
mkdir build && cd build && cmake .. && make -j$(nproc)
sudo make install
cd ../..

# 2. libzmq & cppzmq
echo ********** Installing libzmq & cppzmq **********
sudo apt install libsodium-dev
git clone https://github.com/zeromq/libzmq.git && cd libzmq
git checkout v4.3.4
mkdir build && cd build && cmake ..
make -j$(nproc)
sudo make install
cd ../..

git clone https://github.com/zeromq/cppzmq.git && cd cppzmq
mkdir build && cd build && cmake ..
make -j$(nproc)
sudo make install
cd ../..

# 3. spdlog
echo ********** Installing spdlog **********
git clone https://github.com/gabime/spdlog.git && cd spdlog
git checkout v1.8.2
mkdir build && cd build
cmake .. && make -j$(nproc)
sudo make install
cd ../..

## 4. uvgRTP
echo ********** Installing uvgRTP **********
sudo apt install libcrypto++-dev
git clone https://github.com/gt-flexr/uvgRTP.git
cd uvgRTP && mkdir build && cd build
cmake -DDISABLE_CRYPTO=1 .. && make -j$(nproc)
sudo make install
cd ../..

## 5. Catch2
echo ********** Installing Catch2 **********
git clone https://github.com/jheo4/Catch2.git
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install
cd ..

## 6. Boost
echo ********** Installing Boost **********
sudo apt-get install libboost-all-dev

## 7. Shared memory queue
echo ********** Installing Shared memory queue **********
git clone https://github.com/gt-flexr/FleXR_ShmQ.git
cd FleXR_ShmQ && mkdir build && cd build && cmake ..
make -j$(nproc)
sudo make install
cd ../..
