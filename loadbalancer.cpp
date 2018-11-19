#include <string>
#include <vector>
#include <sstream>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "defines.hpp"
#include "strlib.hpp"
using namespace std;

int main() {
    cout << ">>> By Mohammad Ali Toufighi 810195371 <<<" << endl << endl;

    // creating named pipe
    unlink(FIFO_TEMP_PATH); // to remove previous file
    if (mkfifo(FIFO_TEMP_PATH, 0666) < 0) {
        cerr << "error in creating fifo." << endl;
        exit(1);
    }

    // creating presenter process
    pid_t ppid = fork();
    if (ppid == 0) {
        char* argv[] = {NULL};
        execve(PRESENTER_FILENAME, argv, NULL);
    }

    string msg;
    while (getline(cin, msg)) {
        string sort_type, tok;
        string sort_by = EMPTY_STR; // to be checked if not assigned
        string folder_name = EMPTY_STR; // to be checked if not assigned
        vector<Pair> fields; // first is field_name and second is field_value
        vector<string> files; // file names
        vector<pid_t> worker_ids; // process ids of workers (used in waitpid)
        int prc_num = -1; // to be checked if not assigned

        msg = remove_space((char*)msg.c_str()); // removing excess spaces in input 

        if (lower(msg) == QUIT) {
            // sending quit message to presenter
            stringstream ss;
            ss << LOADBALANCER_HEADER << endl << QUIT << endl;
            ofstream pipe_stream(FIFO_TEMP_PATH, ofstream::in | ofstream::out);
            if (pipe_stream.is_open()) {
                pipe_stream << ss.str();
                pipe_stream.close();
            }
            break;
        }

        // tokenizing and process input
        stringstream line(msg);
        while (getline(line, tok, DASH)) {
            stringstream line(tok);
            string first, second;
            getline(line, first, EQUAL);
            getline(line, second, EQUAL);
            if (first == PRC_CNT)
                prc_num = atoi(second.c_str());
            else if (first == DIRECTORY)
                folder_name = second;
            else if (second == ASCEND || second == DESCEND) {
                sort_type = second;
                sort_by = first;
            } else
                fields.push_back(Pair(first, second));
        }

        if (folder_name == EMPTY_STR || prc_num <= 0) {
            cout << "invalid inputs." << endl;
            continue;
        }

        // read file names from directory
        DIR* dir;
        struct dirent* ent;
        if ((dir = opendir(folder_name.c_str())) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_name[0] != DOT) // '.' and '..'
                    files.push_back(ent->d_name);
            }
            closedir(dir);
        } else {
            cout << "no such file or directory: " << folder_name << endl;
            continue;
        }

        vector<vector<string> > worker_files(prc_num);
        // assigning files to workers according to number of processes
        for (size_t i = 0; i < files.size(); ++i)
            worker_files[i % prc_num].push_back(files[i]);


        // writing data to presenter
        stringstream ss;
        ss << LOADBALANCER_HEADER << endl << prc_num << WS;
        ss << sort_by << WS << sort_type << endl;
        ofstream pipe_stream(FIFO_TEMP_PATH);
        if (pipe_stream.is_open()) {
            pipe_stream << ss.str();
            pipe_stream.close();
        }
        else {
            cerr << "error in opening file." << endl;
            exit(1);
        }

        // creating worker processes
        for (int i = 0; i < prc_num; ++i) {
            int fd[2];
            if (pipe(fd) != 0) {
                cerr << "failed to create pipe." << endl;
                exit(1);
            }
            pid_t cpid = fork();
            if (cpid == 0) {
                close(fd[1]); // close write end of pipe
                if (dup2(fd[0], STDIN_FILENO)==-1) { // duplicate read end of pipe to STDIN
                    cerr << "failed to duplicate pipe fd to cin." <<endl;
                    exit(1);
                }
                char* argv[] = {NULL};
                execve(WORKER_FILENAME, argv, NULL);
            }
            else if (cpid > 0) {
                // adding worker process id
                // used in wait at end of while
                worker_ids.push_back(cpid);

                close(fd[0]); // close read end of pipe
                stringstream ss;
                ss << LOADBALANCER_HEADER << endl << worker_files[i].size() << WS << fields.size() << endl;
                for (size_t j = 0; j < worker_files[i].size(); ++j)
                    ss << folder_name << SLASH << worker_files[i][j] << endl;
                for (size_t j = 0; j < fields.size(); ++j)
                    ss << fields[j].first << endl << fields[j].second << endl;
                if (write(fd[1], ss.str().c_str(), ss.str().length()) < 0) {
                    cerr << "failed to write on unnamed pipe." << endl;
                    exit(1);
                }
                close(fd[1]); // close write end of pipe (writing finished)
            } else {
                cerr << "fork failed." << endl;
                exit(1);
            }
        }

        // waits for workers to finish
        // prevents conflict on named pipe when large inputs arrive
        for (size_t i = 0; i < worker_ids.size(); ++i)
            waitpid(worker_ids[i], NULL, (int)NULL);
    }

    // wait for presenter to quit
    waitpid(ppid, NULL, (int)NULL);
    
    cout << "exiting..." << endl;

    return 0;
}
