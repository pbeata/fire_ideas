/* 
  
  07-18-17 by Paul A. Beata
  takes data files and produces small data packages for each step

    $ lcm-gen -x sensor.lcm
    $ g++ MultiSensor.cpp -fopenmp -llcm -std=c++11
    $ export OMP_NUM_THREADS=4   # or 1, 2, 8, 16, etc..
    $ ./a.out

*/

// standard include directives
#include <iostream>
#include <omp.h>
#include <fstream>
#include <math.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

// LCM include directives
#include <lcm/lcm-cpp.hpp>
#include "sim_sensor/sensor_data.hpp"

// main sensor program
int main(int argc, char* argv[])
{

  // initialize constants
  const int NUM_ROOMS = 128;  // number of rooms in simulation
  const int NUM_DATA = 6;     // number of columns in data files
  double nominalFreq = 1.00;  // [Hz]
  double noise = 0.25;        // [%]
  int convFact = 1000000;     // [s] to [us]
  float roundup = 0.5;        // [us]
  srand(22);                  // seed for random numbers
  int testing = 0;            // use for testing room 0 only for now 
  int msgLimit = 301;         // use for testing with a small number of messages

  // unit conversions (if needed)
  double convTemp = 1.0;
  double convO2 = 100.0;
  double convCO = pow(10.0, 6);
  double convCO2 = 100.0;
  double convHCN = pow(10.0, 6);
  double convFlux = 1.0;

  // ================================================================

  // check if LCM is working
  lcm::LCM lcm;
  if(!lcm.good())
  {
    return 1;
  }

  // declare variables
  int num_threads = 0;
  double my_row[NUM_DATA];
  std::string file_prefix = "./data/file";
  std::string file_suffix = ".csv";
  std::string channel_pre = "ROOM";

  // sensor variables
  float period = 1.0 / nominalFreq;
  float min_per = (1.0 - noise) * period;
  float max_per = (1.0 + noise) * period;
  min_per = convFact * min_per + roundup;
  max_per = convFact * max_per + roundup; 
  int min_time = (int)min_per;
  int max_time = (int)max_per;
  int rand_range = max_time - min_time;

  //=================================
  // timer 
  // official start time
  char buffer1[26];
  int millisec1;
  struct tm* tm_info1;
  struct timeval tv1; 
  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  gettimeofday(&tv1, NULL);
  millisec1 = lrint(tv1.tv_usec/1000.0); // Round to nearest millisec
  if (millisec1>=1000) 
  { 
    // Allows for rounding up to nearest second
    millisec1 -=1000;
    tv1.tv_sec++;
  }
  tm_info1 = localtime(&tv1.tv_sec);
  strftime(buffer1, 26, "%H:%M:%S", tm_info1);
  printf("START TIME: %s.%03d\n", buffer1, millisec1);
  //=================================    

  // distribute one room to each thread
  #pragma omp parallel private(my_row)
  {
    // get unique thread number
    int pid = omp_get_thread_num();
    //printf("this is process #%d \n", pid);

    // check to make sure #threads = #rooms
    if (pid == 0)
    {
      num_threads = omp_get_num_threads();
      if (num_threads != NUM_ROOMS)
      {
        printf("***warning: must set proper number of threads (%d) for %d rooms; use \n", num_threads, NUM_ROOMS);
        printf("$ export OMP_NUM_THREADS=%d \n", NUM_ROOMS);
      }
    }

    // declare unique LCM data packet and channel name
    sim_sensor::sensor_data my_data;
    std::string my_chan = channel_pre + std::to_string(pid);

    // open "my" data file
    std::string my_file;
    my_file = file_prefix + std::to_string(pid) + file_suffix;
    if (testing == 1)
    {
      my_file = "./data/test3.csv";
    }
    std::ifstream if_file(my_file.c_str());

    // timer setup
    char buffer[26];
    int millisec;
    struct tm* tm_info;
    struct timeval tv;     

    // read in data from file and publish to LCM network
    std::chrono::steady_clock::time_point current;
    std::string my_line;
    int col = 0;
    double readValue;
    int rand_int;
    double rand_val;
    double my_time = 0.0;
    int numMsgSent = 0;

    while (getline(if_file, my_line, ','))
    {
      std::istringstream iss(my_line);
      while (iss >> readValue)
      {
        my_row[col] = readValue;
        if (col < NUM_DATA - 1)
        {
          // continue reading on row
          col += 1;  
        }
        else if (numMsgSent < msgLimit)
        {
          // start on new row
          col = 0;

          // package the data from the file
          my_data.roomNum = pid;
          my_data.temperature = my_row[0] * convTemp;
          my_data.O2conc = my_row[1] * convO2;
          my_data.COconc = my_row[2] * convCO;
          my_data.CO2conc = my_row[3] * convCO2;
          my_data.HCNconc = my_row[4] * convHCN;
          my_data.heatFlux = my_row[5] * convFlux;

          // compute random noise for delay time
          rand_int = rand()%rand_range + min_time;
          rand_val = ((double)rand_int) / convFact;    

          // wait random time and then send
          my_data.sendTime = my_time;
          my_time += rand_val;
 
          //===================================================================
          // PUBLISH LCM MSG TO MAIN PROGRAM WITH NEW SENSOR DATA
          usleep( rand_val * pow(10.0, 6.0) );
          gettimeofday(&tv, NULL);
          lcm.publish(my_chan, &my_data);
          numMsgSent += 1;
          millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
          if (millisec>=1000) 
          { 
            // Allows for rounding up to nearest second
            millisec -=1000;
            tv.tv_sec++;
          }
          tm_info = localtime(&tv.tv_sec);
          strftime(buffer, 26, "%H:%M:%S", tm_info);
          //printf("sensor #%d sent msg #%d at time %s.%03d\n", pid, numMsgSent, buffer, millisec);
          printf("%d, %d, %s.%03d \n", pid, numMsgSent, buffer, millisec);
          //===================================================================
        }
        else
        {
          break;
        }
      }
    }
    
    // close "my" data file
    if_file.close();

  } // exit parallel loop

  //=================================
  // timer 
  gettimeofday(&tv1, NULL);
  millisec1 = lrint(tv1.tv_usec/1000.0); // Round to nearest millisec
  if (millisec1>=1000) 
  { 
    // Allows for rounding up to nearest second
    millisec1 -=1000;
    tv1.tv_sec++;
  }
  tm_info1 = localtime(&tv1.tv_sec);
  strftime(buffer1, 26, "%H:%M:%S", tm_info1);
  printf("END TIME: %s.%03d\n", buffer1, millisec1);
  //================================= 

  return 0;
}
