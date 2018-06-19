# VK C++ BOT

VK based bot on C++

## Getting Started

First of all, we need to install a lot of packages.

For install on Ubuntu/Debian:
```
sudo apt-get install git gcc g++ make libcurl4-gnutls-dev libgd-dev libboost-python-dev python-pip
pip install psutil untangle
export TERMUX=1
````
For install on Termux:
```
apt install libcurl-dev libgd-dev python2-dev boost-dev clang make git libcrypt-dev pkg-config
pip2 install psutil untangle
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
