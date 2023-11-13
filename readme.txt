Instructions

The following instructions assume you are using the MSYS2 software and a ChipKIT Uno32 on a windows system

STEP 1
Open up MSYS2 and enter the following line

. /opt/mcb32tools/environment

STEP 2
Now find the directory of the program and type in "cd " followed by the directory path to the folder the program is located in, an example is given below

cd /c/Users/myName/IS1200/miniProject

STEP 3
Next you can type the following to compile the program if it has not already been compiled

make

STEP 4
Lastly you type in the following while the Uno32 chipKit is plugged into your computer, replacing the x at the end with the COM number for your device

make install TTYDEV=/dev/ttySx

STEP 5
Enjoy!