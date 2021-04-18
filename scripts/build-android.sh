#!/bin/bash

CWD="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd $CWD
cd ..

echo 'Building Android in:'
echo $PWD

cd android
rm -rf build/

./gradlew clean
./gradlew assembleDebug assembleRelease

rm -rf ../android-npm
mkdir ../android-npm

shopt -s dotglob nullglob
mv -v build/outputs/aar/* ../android-npm

cd $CWD
cd scripts
cp template.build.gradle ../android-npm/build.gradle
