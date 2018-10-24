# VK C++ BOT

VK based bot on C++

## Getting Started

First of all, we need to install a lot of packages.

For install on Ubuntu/Debian:
```
sudo apt-get install git gcc g++ make libcurl4-openssl-dev libgd-dev libboost-python-dev python3-dev python3-pip
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
¯\_(ツ)_/¯
```
For install on Gentoo:
```
¯\_(ツ)_/¯
```
### Compiling

```
git clone https://github.com/EVGESHAd/vk-cpp-bot/
cd ./vk-cpp-bot
git submodule update --init vox thr py json
make
```

On Windows(no python)
```
git submodule update --init win32deps
and compile in Project

copy *.exe vox and win32deps/libs to bot folder
```

### Configuration
after first launch configure bot in config.json
