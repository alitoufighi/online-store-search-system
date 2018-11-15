#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <sstream>
#include <dirent.h>
#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

#define DASH '-'
#define EQUAL '='
#define WS ' '
#define DOT '.'
#define COLON ':'
#define WORKER_FILENAME "worker"
#define PRESENTER_FILENAME "presenter"
#define WORKER_HEADER "--W--" // to distinguish it's a fresh message from worker
#define PRESENTER_HEADER "--P--" // it's a message from presenter (not used)
#define LOADBALANCER_HEADER "--LB--" // it's a message from load balancer
#define NEWLINE '\n'
#define SLASH '/'
#define QUIT "quit"
#define PRC_CNT "prc_cnt"
#define DIRECTORY "dir"
#define ASCEND "ascend"
#define DESCEND "descend"
#define EMPTY_STR ""
#define FIFO_TEMP_PATH "/tmp/osca2"

string remove_space(char* line){
    char* i = line, *j = line;
    while(*j != '\0'){
        *i = *j++;
        if(*i != WS)
            i++;
    }
    *i = '\0';
    return line;
}

string lower(string str){
    std::locale loc;
    string res = "";
    for (std::string::size_type i=0; i<str.length(); ++i)
        res += tolower(str[i],loc);
    return res;
}

typedef std::pair<std::string, std::string> Pair;

int main(){
    string msg;
    while (getline(cin, msg)){
        msg = remove_space((char*)msg.c_str());
        stringstream line(msg);
        string sort_type;
        string sort_by = EMPTY_STR; // to be checked if not assigned
        vector<Pair> fields; // first is field_name and second is field_value
        int prc_num = -1; // to be checked if not assigned
        string folder_name = EMPTY_STR; // to be checked if not assigned
        string tok;

        if(lower(msg) == QUIT){
            cout << "Exiting..." << endl;
            exit(0);
        }

        while (getline(line, tok, DASH)) {
            stringstream line(tok);
            string first, second;
            getline(line, first, EQUAL);
            getline(line, second, EQUAL);
            if (first == PRC_CNT){
                prc_num = atoi(second.c_str());
            } else if (first == DIRECTORY) {
                folder_name = second;
            } else if (second == ASCEND || second == DESCEND) {
                sort_type = second;
                sort_by = first;
            } else {
                fields.push_back(Pair(first, second));
            }
        }

        if(folder_name == EMPTY_STR || prc_num == -1){
            cerr << "Invalid Inputs." << endl;
            continue;
        }
        pid_t cpid;
        vector<string> files;
        vector<vector<string> > worker_files(prc_num);

        // read file names from directory
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(folder_name.c_str())) != NULL){
            while ((ent = readdir(dir)) != NULL){
                if (ent->d_name[0] != DOT){
                    // cout << ent->d_name << endl;
                    files.push_back(ent->d_name);
                }
            }
            closedir(dir);
        } else {
            cerr << "Error in opening directory!" << endl;
            return -1;
        }

        // assigning files to workers according to number of processes
        for (int i = 0; i < files.size(); ++i){
            worker_files[i % prc_num].push_back(files[i]);
            // cout << files[i] << endl;
        }

        // creating named pipe
        string fifo_path = FIFO_TEMP_PATH;
        unlink(FIFO_TEMP_PATH); // to remove previous file
        if(mkfifo(FIFO_TEMP_PATH, 0666) < 0){
            cerr << "Error in creating fifo." << endl;
            return -1;
        }

        // creating presenter process
        int ppid = fork();
        // if it's presenter (child)
        if(ppid == 0){
            char* argv[] = {NULL};
            execve(PRESENTER_FILENAME, argv, NULL);
        } else if(ppid > 0) { // if it's parent (lb)
            stringstream ss;
            ss << LOADBALANCER_HEADER << endl << prc_num << WS;   // HEADER\n`prc_num`\n
            ss << sort_by << WS << sort_type << endl; // `sort_by` `sort_type`\n
            ofstream pipe_stream(FIFO_TEMP_PATH);
            if(pipe_stream.is_open()){
                pipe_stream << ss.str();
                pipe_stream.close();
            }
            else {
                cerr << "ERROR OPENING FILE IN LB" << endl;
                exit(-1);
            }
        } else {
            cerr << "Oops! Failed to create Presenter process" << endl;
            return -1;
        }

        // creating worker processes
        for (int i = 0; i < prc_num; ++i){
            int fd[2];
            if (pipe(fd) != 0){
                cerr << "pipe failed." << endl;
                return -1;
            }
            int cpid = fork();
            if (cpid == 0){
                close(fd[1]);
                if(dup2(fd[0], STDIN_FILENO)==-1){
                    cerr << "Dup failed" <<endl;
                    return -1;
                }
                char* argv[] = {NULL};
                execve(WORKER_FILENAME, argv, NULL);
                // exeve(WORKER_FILENAME, WORKER_FILENAME, (char*)NULL);
            }
            else if (cpid > 0){
                close(fd[0]);
                stringstream ss;
                ss << LOADBALANCER_HEADER << endl << worker_files[i].size() << endl << fields.size() << endl;
                for(int j = 0; j < worker_files[i].size(); ++j)
                    ss << folder_name << SLASH << worker_files[i][j] << endl;
                for(int j = 0; j < fields.size(); ++j)
                    ss << fields[j].first << endl << fields[j].second << endl;
                if(write(fd[1], ss.str().c_str(), ss.str().length()) < 0){
                    cerr << "Write failed!" << endl;
                    return -1;
                }
                close(fd[1]);
                if(wait(NULL)==-1){
                    cerr << "Error in wait" << endl;
                    return -1;
                }
            } else {
                cerr << "fork failed." << endl;
            }
        }
    }
    return 0;
}