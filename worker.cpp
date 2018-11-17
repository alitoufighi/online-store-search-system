#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "defines.hpp"
using namespace std;

int main() {
    string header, num_of_files_str, num_of_fields_str;
    int num_of_files;
    int num_of_fields;
    vector<string> file_fields;
    vector<string> results; // container for filtered results
    vector<int> filter_columns; // container for indices of filtering columns

    // read end of pipe is mapped to cin
    cin >> header >> num_of_files >> num_of_fields;

    vector<string> files(num_of_files);
    vector<Pair> search_fields(num_of_fields);

    // reading file names
    for (int i = 0; i < num_of_files; ++i)
        cin >> files[i];
    // reading filtering fields
    for (int i = 0; i < num_of_fields; ++i)
        cin >> search_fields[i].first >> search_fields[i].second;

    // for each file, filter data
    for (int i = 0; i < num_of_files; ++i) {
        ifstream f(files[i].c_str()); // open file
        string line;
        bool first_line_read = false; // first line of each file is field names. skip them after read once

        // reading from file
        while (getline(f, line)) {
            stringstream ss(line);
            string tok;

            // read tokens of file (values)
            for (int j = 0; getline(ss, tok, WS); ++j) {
                if (first_line_read == false) { // if we haven't read the first row (the fields)
                    if (i == 0) { // only for first file!
                        file_fields.push_back(tok);
                        for (int k = 0; k < search_fields.size(); ++k) {
                            if (tok == search_fields[k].first)
                                filter_columns.push_back(j);
                        }
                    }
                } else {
                    for (int k = 0; k < filter_columns.size(); ++k) {
                        if (j == filter_columns[k] && tok == search_fields[k].second) // if we should filter this one
                            results.push_back(line);
                    }
                }
            }
            first_line_read = true;
        }
        f.close();
    }

    // making results unique (if pushed back by two filters)
    std::vector<string>::iterator it;
    it = unique(results.begin(), results.end());
    results.resize(distance(results.begin(), it));

    // gathering results into a stringstream
    stringstream ss;
    ss << WORKER_HEADER << endl;
    ss << file_fields.size() << WS << results.size() << endl;
    for (int i = 0; i < file_fields.size(); ++i)
        ss << file_fields[i] << endl;
    for (int i = 0; i < results.size(); ++i)
        ss << results[i] << endl;

    // write results to named pipe
    ofstream pipe_stream(FIFO_TEMP_PATH, ofstream::in | ofstream::out);
    if (pipe_stream.is_open()) {
        pipe_stream << ss.str();
        pipe_stream.close();
    }
    else
        cerr << "ERROR OPENING FILE!" << endl;
    return 0;
}
