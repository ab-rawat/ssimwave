*********SSIMWAVE coding challenge***************

This is the multithreaded version of the code demo.cpp
It reads all the video files from the specified directory one at a time, processes one frame per thread until all frames have
been processed. It calculates the luminance per BGR color plane and averages them to get the frame luminance.
This is not the best way to do this. We should have converted BGR --> LAB and used the L plane for luminance estimation. 
But it has been implemented as such since it was explicitely asked in the assignment.
Gamma is assumed to be 1 (since it was not specified).
Also the BGR simple averaging has been done instead of weighted averaging (again as asked in the assignment).
In the absence of any camera specifications, this estimate of luminance is just the "relative luminance" so it should be fine.
Since the number of frames >> number of threads, the frames are processed in chunks of num_threads each. This book-keeping is
performed in main().
Once all the frames are processed, the average luminance value for the video file is calculated and output to terminal.
The above process repeats for all video files in the directory.
Once all files have been processed, the min, max, mean and median luminance values across files are calculated and printed
to the terminal.
Basic error handling is incorporated in the code.
*********Uses opencv***********
***********Uses c++11 multithreading thread.h********************

Tested on OSX only. Should work on Linux.
Compile as: 

g++ demopp.cpp -o demopp `pkg-config --libs opencv` -I/usr/local/Cellar/opencv/3.3.1_1/include -std=c++11 -pthread

User will need to tweak the above include path etc for compilation on user machine.

Sample Usage:  

./demopp data_files 8


Where:   
data_files is the directory and 
8 is the number of threads to run (>=1). Cannot be 0!

The directory path is without the trailing /
Will work on linux/OSX only!
***********Uses c++11 multithreading thread.h********************

This code has been written by Abhishek Rawat, Phd.
All rights reserved (c) 2017.
