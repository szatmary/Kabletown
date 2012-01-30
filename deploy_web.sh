#!/bin/sh
rm -r ./Kabletown.app
/usr/local/Trolltech/Qt-4.8.0/bin/qmake kabletown.pro
make
make clean
rm Makefile
/usr/local/Trolltech/Qt-4.8.0/bin/macdeployqt ./Kabletown.app
/Developer/usr/bin/packagemaker --doc installer.pmdoc -o Kabletown.pkg
