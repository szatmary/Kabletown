; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply
; prompts the user asking them where to install, and drops a copy of "MyProg.exe"
; there.

;--------------------------------

; The name of the installer
Name "ComcastBandwidthMeter"

; The file to write
OutFile "ComcastBandwidthMeter_Web.exe"

; The default installation directory
InstallDir $PROGRAMFILES\ComcastBandwidthMeter

; The text to prompt the user to enter a directory
; DirText "This will install My Cool Program on your computer. Choose a directory"

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

; Set output path to the installation directory.
SetOutPath $INSTDIR

; Put file there
File release\ComcastBandwidthMeter.exe
File win_depends\libeay32.dll
File win_depends\ssleay32.dll
File c:\Qt\2010.05\mingw\bin\mingwm10.dll
File c:\Qt\2010.05\mingw\bin\libgcc_s_dw2-1.dll
FILE c:\Qt\2010.05\qt\bin\QtCore4.dll
FILE c:\Qt\2010.05\qt\bin\QtGui4.dll
FILE c:\Qt\2010.05\qt\bin\QtNetwork4.dll
FILE c:\Qt\2010.05\qt\bin\QtSql4.dll
FILE c:\Qt\2010.05\qt\bin\QtWebKit4.dll
FILE c:\Qt\2010.05\qt\bin\phonon4.dll

File vcredist_x86.exe

; Execute the redist
ExecWait "$INSTDIR\vcredist_x86.exe /q:a"

; Qt Plugins
SetOutPath $INSTDIR
File qt.conf

SetOutPath "$INSTDIR\plugins\imageformats"
File "C:\Qt\2010.05\qt\plugins\imageformats\qgif4.dll"

SetOutPath "$INSTDIR\plugins\sqldrivers"
File "C:\Qt\2010.05\qt\plugins\sqldrivers\qsqlite4.dll"

writeUninstaller "$INSTDIR\uninstaller.exe"

; Now create shortcuts
CreateDirectory "$SMPROGRAMS\Comcast Bandwidth Meter"
CreateShortCut "$SMPROGRAMS\Comcast Bandwidth Meter\Comcast Bandwidth Meter.lnk" "$INSTDIR\ComcastBandwidthMeter.exe"
CreateShortCut "$SMPROGRAMS\Comcast Bandwidth Meter\Uninstall Comcast Bandwidth Meter.lnk" "$INSTDIR\uninstaller.exe"

; Write uninstall into to registry 
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Comcast Bandwidth Meter" "DisplayName" "Comcast Bandwidth Meter"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Comcast Bandwidth Meter" "UninstallString" "$\"$INSTDIR\uninstaller.exe$\""


; Execute the program
ExecShell "open" "$INSTDIR\ComcastBandwidthMeter.exe"


SectionEnd ; end the section


Section "Uninstall"

DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Comcast Bandwidth Meter"
RMDir /R "$INSTDIR"
RMDir /R "$SMPROGRAMS\Comcast Bandwidth Meter"

SectionEnd ; end the section
