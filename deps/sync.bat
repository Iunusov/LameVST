@echo off

echo Sync started.

echo libmp3lame...
if not exist libmp3lame-CMAKE (
    (git clone https://github.com/R-Tur/libmp3lame-CMAKE.git)
) else (
    (cd libmp3lame-CMAKE) && (git pull) && (cd ..)
)

echo VST_SDK_2.4...
if not exist VST_SDK_2.4 (
    (git clone https://r-tur@bitbucket.org/r-tur/vst_sdk_2.4.git)
) else (
    (cd VST_SDK_2.4) && (git pull) && (cd ..)
)


echo Done.
