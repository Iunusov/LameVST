# LameVST
Lame MP3 "emulator" in form of VST-plugin (mp3 as VST effect).


## Prerequisites

[Visual Studio 2013+](https://www.visualstudio.com/downloads/download-visual-studio-vs)

[CMake >=3.0](https://cmake.org/download/)

[GIT](https://git-scm.com/download/win)

## Build

```
git clone https://github.com/Iunusov/LameVST.git
```

```
cd LameVST
```

```
!!make_project!!.bat
```

## Build (x64)

```
mkdir build64
```

```
cd build64
```

```
cmake -G "Visual Studio 14 2015 Win64" ..
```

## Linux Build (e.g. UBUNTU)

```
sudo apt-get install build-essential
```

```
cd deps
./sync.sh
cd ..
```

```
mkdir build
cd build
cmake ..
make
```

## Result

After successfull build the resulting DLL file will be available there (you can copy it into your VST folder):
```
LameVST\LameVST_DLL
```

