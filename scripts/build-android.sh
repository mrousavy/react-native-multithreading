#!/bin/bash

CWD="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd $CWD
cd ..

echo 'Building Android in:'
echo $PWD

rm -rf android-npm/

cd android
rm -rf build/

# Build .aar files
./gradlew clean
./gradlew assembleDebug assembleRelease

rm -rf ../android-npm
mkdir ../android-npm

# Move over the .aar files
shopt -s dotglob nullglob
mv -v build/outputs/aar/* ../android-npm

# Move over the build.gradle file
cd $CWD
cd ..
cd scripts
cp template.build.gradle ../android-npm/build.gradle
