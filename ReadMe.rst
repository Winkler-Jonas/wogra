ReadMe
======

How to compile C-Code:

 1. Python Header wird benötigt `SO-Install Python Header-File`_

.. _SO-Install Python Header-File: https://stackoverflow.com/questions/21530577/fatal-error-python-h-no-such-file-or-directory

 2. CMake muss mindestens Version 3.20 sein. **cmake --version**

 3. Neueste Version auf Webseite_ runterladen

.. _Webseite: https://cmake.org/download/

 4. Extrahieren **sudo sh cmake-3.22.1-linux-x86_64.sh --prefix=/usr/local/ --eclcude-subdir**

 5. Im Ordner **c_lib/**

 - cmake ./
 - cmake --build ./

 6. ./testapp sollte das Programm ausführen.