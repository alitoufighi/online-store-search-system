#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "defines.hpp"
#include "globals.hpp"
#include "strlib.hpp"
using namespace std;

int main() {
    ifstream pipe_stream(FIFO_TEMP_PATH, ifstream::in | ifstream::out);
    string line, sort_by;
    int workers_received = 0, num_of_reults, num_of_workers;
    vector<string> results; // final container vector

    while(true) {
        vector<string> tmp; // used in merge sort
        // read header
        getline(pipe_stream, line);

        // if we found loadbalancer's data
        if(line == LOADBALANCER_HEADER) {
            getline(pipe_stream, line);
            stringstream ss(line);
            ss >> num_of_workers >> sort_by >> sort_type; // if sorting is not provided, these values will be empty strings (which is ok)
        }
        // if we found a data from our workers
        else if (line == WORKER_HEADER) {
            vector<string> worker_data;
            workers_received++; // add this to number of workers' files received
            
            getline(pipe_stream, line);
            num_of_fields = atoi(line.c_str());
            getline(pipe_stream, line);
            num_of_reults = atoi(line.c_str());

            // // finding which one is sorting value (if any)
            for(int i = 0; i < num_of_fields && getline(pipe_stream, line); ++i){
                if(line == sort_by)
                    sort_field_index = i;
            }

            // reading results of this worker
            for(int i = 0; i < num_of_reults && getline(pipe_stream, line); ++i)
                worker_data.push_back(line);
            
            // using in-place merge sort :)
            sort(worker_data.begin(), worker_data.end(), compare);
            tmp.reserve(results.size() + worker_data.size());
            merge(worker_data.begin(), worker_data.end(), results.begin(), results.end(), back_inserter(tmp), compare);
            results.swap(tmp);

            // check if we are done
            if(workers_received >= num_of_workers)
                break;
        }
    }

    // printing results on consolse
    for(int i = 0; i<results.size(); ++i)
        cout << results[i] << endl;
    cout << "--------------------" << endl;

    // closing input stream for named pipe
    pipe_stream.close();

    return 0;
}