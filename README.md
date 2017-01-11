[![Build Status](https://travis-ci.org/Iunusov/LameVST.svg?branch=master)](https://travis-ci.org/Iunusov/LameVST)
# LameVST
MP3 as VST-effect. 

Ideal solution, if you need to preview you track in the mp3-mode during playback.

![LameVST](https://static.kvraudio.com/i/b/lamevst.png "LameVST")

## Prerequisites

[Visual Studio 2013+](https://www.visualstudio.com/downloads/download-visual-studio-vs)

[CMake >=3.0](https://cmake.org/download/)

[GIT](https://git-scm.com/download/win)

## Windows Build

    $ cd deps
    $ sync
    $ cd ..
    $ md build && cd build
    $ cmake ..   # Or whatever generator (-G) you want to use cmake --help for a list.
    $ cmake --build . --config Release
    $ start LameVST_Solution.sln

or just use bat-file: 

```
!!make_project!!.bat
```

## Linux Build (e.g. UBUNTU)

    $ sudo apt-get install build-essential
    
    $ cd deps
    $ ./sync.sh
    $ cd ..
    
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

## Result

After successfull build the resulting DLL file will be available there (you can copy it into your VST folder):
```
LameVST\LameVST_DLL
```

