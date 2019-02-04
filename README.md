# VK C++ BOT  [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/EVGESHAd/vk-cpp-bot?svg=true)](https://ci.appveyor.com/project/EVGESHAd/vk-cpp-bot)

VK based bot on C++

## Getting Started

First of all, we need to install a lot of packages.

For install on Ubuntu/Debian:
```
sudo apt-get install git gcc make libcurl4-openssl-dev libgd-dev libboost-python-dev python3-dev python3-pip libopencv-imgcodecs-dev libopencv-imgproc-dev libopencv-objdetect-dev libopencv-photo-dev libgif-dev libblas-dev liblapack-dev libdlib-dev
pip3 install psutil untangle py-cpuinfo
````
For install on Android/Termux(termux.com):
```
apt install libcurl-dev libgd-dev python-dev boost-dev clang make git libcrypt-dev pkg-config
pip install psutil untangle py-cpuinfo
export TERMUX=1
```
For install on Arch:
```
pacman -S git gcc make curl opencv boost gd python-pip python
pip3 install psutil untangle py-cpuinfo
```
For install on Gentoo:
```
¯\_(ツ)_/¯
```
### Compiling

```
git clone https://github.com/EVGESHAd/vk-cpp-bot/
cd ./vk-cpp-bot
git submodule update --init vox thr py
make
```

On Windows
```
install python3 to global PATH

git submodule update --init win32deps
pip install psutil untangle py-cpuinfo
winlib.bat

and compile in Project

copy *.exe vox and win32deps/libs to bot folder
```

### Configuration
after first launch configure bot in config.json
