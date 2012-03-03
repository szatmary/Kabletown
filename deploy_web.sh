#!/bin/sh
rm -r ./Kabletown.app Kabletown.pkg Kabletown.dmg
qmake kabletown.pro && make
macdeployqt ./Kabletown.app -dmg
# /Developer/usr/bin/packagemaker --doc installer.pmdoc -o Kabletown.pkg
make clean && rm Makefile && rm -r ./Kabletown.app
