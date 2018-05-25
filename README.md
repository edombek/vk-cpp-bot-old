# VK C++ BOT

VK based bot on C++

## Getting Started

First of all, we need to install a lot of packages.

For install on Ubuntu/Debian:
```
sudo apt-get install git gcc g++ make libcurl4-gnutls-dev libgd-dev libboost-all-dev python-pip
pip install psutil
````
For install on Termux:
```
pkg install libcurl-dev libgd-dev python-dev clang make git
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
git submodule update --init vox thr py
make
cp ./vkbot ../
```

### Configuration
after first launch configure bot in config.json
