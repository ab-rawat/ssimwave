// *************see readme.txt for details******************
// This is the multithreaded version of the code demo.cpp
// It reads all the video files from the specified directory one at a time, processes one frame per thread until all frames have 
// been processed. It calculates the luminance per BGR color plane and averages them to get the frame luminance.
// Since the number of frames >> number of threads, the frames are processed in chunks of num_threads each. This book-keeping is
// performed in main().
// Once all the frames are processed, the average luminance value for the video file is calculated and output to terminal.
// The above process repeats for all video files in the directory.
// Once all files have been processed, the min, max, mean and median luminance values across files are calculated and printed
// to the terminal. 
// Basic error handling is incorporated in the code.
// **********Uses opencv************
// Tested on OSX only. Should compile on Linux.
// Compiled as g++ demopp.cpp -o demopp `pkg-config --libs opencv` -I/usr/local/Cellar/opencv/3.3.1_1/include -std=c++11 -pthread
// Will need to tweak the above include path etc for compilation on user machine.
// Usage:  ./demopp data_files 4
// Where:   data_files is the directory and 4 is the number of threads to run (>=1). Cannot be 0!
// The directory path is without the trailing /
// Will work on linux/OSX only!
// Uses c++11 multithreading thread.h
// This code has been written by Abhishek Rawat, Phd.
// All rights reserved (c) 2017.


#include "opencv2/opencv.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


using namespace cv;
using namespace std;

//function declaration here

void process_frame(cv::Mat, int, float *, int, int);



int main(int argc, char* argv[])
{
clock_t t1, t2;
t1=clock();
int debug=0;   //set =1 for detailed verbose output.
float luminance[10000];
int i,j,k;
float total_luminance=0.0;  //sum over all video files

	if(argc != 3) {
		std::cerr << "Expects two command line arguments. See readme.txt for details..." << std::endl;
		return -1;
	}

string filepath;
string dir = argv[1];
DIR *dp;
struct dirent *dirp;
struct stat filestat;

static const int num_threads = atoi(argv[2]);   // number of threads
cout << "running with " << num_threads << " threads"    << endl;

dp = opendir(argv[1]);

	if (dp == NULL)
    	{
    		cout << "Error(" << errno << ") opening " << dir << endl;
    		return errno;
    	}

int fcount=0; //keep count of the number of files
while ((dirp = readdir( dp )))    //loop over the files one by one
    {
 	   filepath = dir + "/" + dirp->d_name;

// filepath is a sub-directory? skip it!
 
    	if (stat( filepath.c_str(), &filestat )) continue;
    	if (S_ISDIR( filestat.st_mode ))         continue;


cout<< "trying to open " << filepath << endl;

	VideoCapture cap(filepath.c_str());

	if(!cap.isOpened()){
    		std::cout<<"cannot read video!\n";
    		return -1;
	}

fcount++; //video file opened successfully

	double rate = cap.get(CV_CAP_PROP_FPS);
	const int total_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);

	if (debug==1){
		printf("frame rate %lf\n",rate);
		printf("total frames in file %d\n",total_frames);
	}

static const int N=total_frames;   //how many elements?

float lum[10000];
float sum=0.0;   //sum over frames
int count=0;  //frame counter
    

//multithreading book-keeping starts

std::thread t[num_threads];    
int chunks=N/num_threads;    //how many chunks?  
int limits[chunks];

	for(i=0;i<chunks;i++){
		limits[i]=i*num_threads;
	}


for(j=0;j<chunks;j++){ // how many chunks

    for(i=0;i<num_threads;i++)          //start looping over the frames
    {
        Mat frame;
        cap >> frame; // get a new BGR frame from the video

 	if (frame.empty()) { 
                           printf("end of file... \n");
                           break;
        }

	count++;

	t[i] = std::thread(process_frame, frame, i, lum, debug, limits[j]);

    } //finished looping over all the threads

//Join the threads with the main thread

         for (k = 0; k < i; k++) {
             t[k].join();
         }

}  //done with all the frames in this video file


	for(i=0;i<count;i++){     //sum over lum values for all the frames
		sum+=lum[i];
	}


//now calculate the mean luminance over the frames of the video
	if (debug==1){
		std::cout<<sum<<std::endl;
		std::cout<<count<<std::endl;
	}

luminance[fcount-1]=sum/count;         //average over all the frames of the current video file
total_luminance += luminance[fcount-1];   //sum over all video files

std::cout<< "Luminance for this video is " << luminance[fcount-1] <<std::endl;


std::cout<<"I am done with "<< filepath  <<std::endl;

} //finished looping over files

closedir(dp);

cout<<"Done with all the "<< fcount <<" files in the directory."<<endl;
cout<<"Here are the combined statistics"<<endl;

//now calculate the min, max, mean and median luminance values


cout<<"Mean luminance = " <<total_luminance/fcount<<endl;


//sort the array

sort( luminance, luminance + fcount );

cout << "Sorted ascending:"<<endl;

	for ( i = 0; i < fcount; i++){
  		cout<< luminance[i] << endl;
	}



cout<<"Min luminance = " <<luminance[0]<<endl;

cout<<"Max luminance = " <<luminance[fcount-1]<<endl;

//calculate median
	if(fcount % 2 != 0){    // is the number of elements odd
        	int temp = ((fcount+1)/2)-1;
        	cout << "Median luminance " << luminance[temp] << endl;
    	}
    	else{// number of elements is even
     	        cout << "Median luminance "<< (luminance[(fcount/2)-1] + luminance[fcount/2])/2.0 << endl;
    	}

t2 = clock();
float diff=float(t2-t1)/1000000.0;
cout<<"total runtime (sec) " << diff << endl;


return 0;

}



//function definition goes here

void process_frame(cv::Mat frame, int tid, float lum[], int debug, int L){

Scalar myMatMean = mean(frame);   //find the mean for each BGR frame

	if (debug==1){
		std::cout<<L+tid+1<<std::endl;
		std::cout<< myMatMean<<std::endl;
		std::cout<< myMatMean.val[0] << std::endl;   //B
		std::cout<< myMatMean.val[1] << std::endl;   //G
		std::cout<< myMatMean.val[2] << std::endl;   //R
	}

lum[L+tid]=(myMatMean.val[0]+myMatMean.val[1]+myMatMean.val[2])/3.0;

	if (debug==1){
		std::cout<< lum[L+tid] <<std::endl;
	}

}


