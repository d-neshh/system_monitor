#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unistd.h>      
#include <dirent.h>     
#include <sys/types.h>
#include <signal.h>      
#include <chrono>
#include <thread>


using namespace std;

struct Process {
    int pid;
    string name;
    double cpuUsage;
    double memUsage;
};

// Function to read total system memory
long getTotalMemory() {
    ifstream meminfo("/proc/meminfo");
    string key;
    long value;
    string unit;
    while (meminfo >> key >> value >> unit) {
        if (key == "MemTotal:") {
            return value;
        }
    }
    return 1; // prevent divide by zero
}

// Function to get CPU total time from /proc/stat
long getTotalCPUTime() {
    ifstream file("/proc/stat");
    string cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal;
    file >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    return user + nice + system + idle + iowait + irq + softirq + steal;
}

// Function to get process CPU time
long getProcessCPUTime(int pid) {
    string path = "/proc/" + to_string(pid) + "/stat";
    ifstream file(path);
    if (!file) return 0;
    string tmp;
    long utime, stime;
    for (int i = 0; i < 13; i++) file >> tmp; // skip first 13 fields
    file >> utime >> stime;
    return utime + stime;
}

// Function to get process name
string getProcessName(int pid) {
    string path = "/proc/" + to_string(pid) + "/comm";
    ifstream file(path);
    string name;
    getline(file, name);
    return name;
}

// Function to get memory usage of process
double getMemoryUsage(int pid, long totalMem) {
    string path = "/proc/" + to_string(pid) + "/statm";
    ifstream file(path);
    long mem;
    file >> mem;
    return (double)mem * getpagesize() / 1024.0 / totalMem * 100.0;
}

// Gather process data
vector<Process> getProcesses(long totalMem, long totalCPUTimePrev, vector<long> &prevTimes) {
    vector<Process> processes;
    DIR *dir = opendir("/proc");
    if (!dir) return processes;

    struct dirent *entry;
    long totalCPUTimeNow = getTotalCPUTime();

    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            int pid = stoi(entry->d_name);
            string name = getProcessName(pid);
            long procTimeNow = getProcessCPUTime(pid);
            long procTimePrev = (pid < (int)prevTimes.size()) ? prevTimes[pid] : 0;
            double cpuUsage = 100.0 * (procTimeNow - procTimePrev) / (double)(totalCPUTimeNow - totalCPUTimePrev + 1);
            double memUsage = getMemoryUsage(pid, totalMem);
            processes.push_back({pid, name, cpuUsage, memUsage});
            if (pid >= (int)prevTimes.size()) prevTimes.resize(pid + 1);
            prevTimes[pid] = procTimeNow;
        }
    }
    closedir(dir);
    return processes;
}

int main() {
    long totalMem = getTotalMemory();
    long totalCPUPrev = getTotalCPUTime();
    vector<long> prevTimes(10000, 0);
    vector<Process> processes;

    char choice;
    while (true) {
        system("clear");
        cout << "===================== System Monitor Tool =====================\n";
        cout << " PID\tCPU(%)\tMEM(%)\tNAME\n";
        cout << "---------------------------------------------------------------\n";

        processes = getProcesses(totalMem, totalCPUPrev, prevTimes);
        totalCPUPrev = getTotalCPUTime();

        sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
            return a.cpuUsage > b.cpuUsage;
        });

        int count = 0;
        for (auto &p : processes) {
            if (count++ > 15) break;
            cout << " " << p.pid << "\t" << p.cpuUsage << "\t" << p.memUsage << "\t" << p.name << endl;
        }

        cout << "\nOptions:\n"
             << "[1] Sort by CPU\n"
             << "[2] Sort by Memory\n"
             << "[3] Kill process\n"
             << "[r] Refresh automatically (every 3s)\n"
             << "[q] Quit\n"
             << "Enter choice: ";
        cin >> choice;

        if (choice == '1') {
            sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
                return a.cpuUsage > b.cpuUsage;
            });
        } else if (choice == '2') {
            sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
                return a.memUsage > b.memUsage;
            });
        } else if (choice == '3') {
            int pid;
            cout << "Enter PID to kill: ";
            cin >> pid;
            if (kill(pid, SIGTERM) == 0)
                cout << "Process " << pid << " terminated.\n";
            else
                perror("Error killing process");
            this_thread::sleep_for(chrono::seconds(2));
        } else if (choice == 'r') {
            for (int i = 0; i < 5; i++) { // refresh 5 times
                system("clear");
                processes = getProcesses(totalMem, totalCPUPrev, prevTimes);
                totalCPUPrev = getTotalCPUTime();
                sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
                    return a.cpuUsage > b.cpuUsage;
                });
                cout << "===================== Auto Refresh Mode =====================\n";
                cout << " PID\tCPU(%)\tMEM(%)\tNAME\n";
                cout << "-------------------------------------------------------------\n";
                int count = 0;
                for (auto &p : processes) {
                    if (count++ > 15) break;
                    cout << " " << p.pid << "\t" << p.cpuUsage << "\t" << p.memUsage << "\t" << p.name << endl;
                }
                this_thread::sleep_for(chrono::seconds(3));
            }
        } else if (choice == 'q') break;
    }

    return 0;
}
