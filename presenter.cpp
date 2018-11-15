#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "defines.hpp"
#include "strlib.hpp"
using namespace std;

// we need to declare them globally, so that we can use them in compare() for sorting
string sort_type;
int sort_field_index, num_of_fields;

// used in sort
bool compare(string s1, string s2) {
    stringstream ss1(s1), ss2(s2);
    string field1, field2;
    int i = 0;
    while (i < num_of_fields) {
        getline(ss1, field1, WS);
        getline(ss2, field2, WS);
        if (i == sort_field_index) {
            if (is_num(field1) && is_num(field2)) {
                int f1 = atoi(field1.c_str()), f2 = atoi(field2.c_str());
                return sort_type == ASCEND ? (f1 < f2) : (f1 > f2);
            } else
                return sort_type == ASCEND ? (field1 < field2) : (field1 > field2);
        }
        i++;
    }
    return false; // if sorting not provided by user, this does not do anything :D
}

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
            
            // pipe_stream >> num_of_reults; // this didn't work :(
            getline(pipe_stream, line);
            num_of_fields = atoi(line.c_str());
            getline(pipe_stream, line);
            num_of_reults = atoi(line.c_str());

            int i;
            // finding which one is sorting value (if any)
            i = 0;
            while(i < num_of_fields) {
                getline(pipe_stream, line);
                if(line == sort_by)
                    sort_field_index = i;
                i++;
            }

            // reading results of this worker
            i = 0;
            while(i < num_of_reults) {
                getline(pipe_stream, line);
                worker_data.push_back(line);
                i++;
            }
            
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

    // closing input stream for named pipe
    pipe_stream.close();

    return 0;
}