#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
using namespace std;

struct Process {
    int id, arrivalTime, burstTime, priority, remainingTime, completionTime, turnaroundTime, waitingTime;
};

struct Output {
    string algorithmName;
    vector<Process> processes;
    double averageWaitingTime, averageTurnaroundTime, cpuUtilization;
    vector<pair<int, pair<int, int>>> ganttChart; // {process_id, {start_time, end_time}}
};

// Global variables
vector<Process> inputProcesses;
int quantumTime;

// Helper function to calculate metrics
Output calculateMetrics(vector<Process>& processes) {
    Output output;
    int totalWaitingTime = 0, totalTurnaroundTime = 0;
    for (auto& p : processes) {
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
    }
    output.processes = processes;
    output.averageWaitingTime = (double)totalWaitingTime / processes.size();
    output.averageTurnaroundTime = (double)totalTurnaroundTime / processes.size();
    output.cpuUtilization = 100.0 * accumulate(processes.begin(), processes.end(), 0, [](int sum, Process& p) {
        return sum + p.burstTime;
    }) / processes.back().completionTime;
    return output;
}

// First-Come, First-Served (FCFS) Scheduling
Output fcfs() {
    Output output;
    output.algorithmName = "First-Come, First-Served (FCFS)";
    vector<Process> processes = inputProcesses;
    sort(processes.begin(), processes.end(), [](Process& a, Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    int currentTime = 0;
    vector<pair<int, pair<int, int>>> ganttChart;
    for (auto& p : processes) {
        if (currentTime < p.arrivalTime) {
            currentTime = p.arrivalTime;
        }
        ganttChart.push_back({p.id, {currentTime, currentTime + p.burstTime}});
        p.completionTime = currentTime + p.burstTime;
        currentTime = p.completionTime;
    }
    output = calculateMetrics(processes);
    output.ganttChart = ganttChart;
    return output;
}
// Shortest Job First (SJF) Scheduling
Output sjf() {
    Output a;
    a.algorithmName = "Shortest Job First (SJF) Scheduling";
    vector<Process> processes = inputProcesses;
    vector<Process> completedProcesses;
    sort(processes.begin(), processes.end(), [](Process& a, Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    int currentTime = 0;
    vector<Process> readyQueue;
    vector<pair<int, pair<int, int>>> ganttChart;
    while (!processes.empty() || !readyQueue.empty()) {
        while (!processes.empty() && processes.front().arrivalTime <= currentTime) {
            readyQueue.push_back(processes.front());
            processes.erase(processes.begin());
        }
        if (readyQueue.empty()) {
            currentTime = processes.front().arrivalTime;
            continue;
        }
        auto shortestJob = min_element(readyQueue.begin(), readyQueue.end(), [](Process& a, Process& b) {
            return a.burstTime < b.burstTime;
        });
        ganttChart.push_back({shortestJob->id, {currentTime, currentTime + shortestJob->burstTime}});
        currentTime += shortestJob->burstTime;
        shortestJob->completionTime = currentTime;
        completedProcesses.push_back(*shortestJob);
        readyQueue.erase(shortestJob);
    }
    Output output = calculateMetrics(completedProcesses);
    output.ganttChart = ganttChart;
    return output;
}


// Priority Scheduling
Output priorityScheduling() {
    Output a;
    a.algorithmName = "Priority Scheduling";
    vector<Process> processes = inputProcesses;
    vector<Process> completedProcesses;
    sort(processes.begin(), processes.end(), [](Process& a, Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    int currentTime = 0;
    vector<Process> readyQueue;
    vector<pair<int, pair<int, int>>> ganttChart;
    while (!processes.empty() || !readyQueue.empty()) {
        while (!processes.empty() && processes.front().arrivalTime <= currentTime) {
            readyQueue.push_back(processes.front());
            processes.erase(processes.begin());
        }
        if (readyQueue.empty()) {
            currentTime = processes.front().arrivalTime;
            continue;
        }
        auto highestPriority = min_element(readyQueue.begin(), readyQueue.end(), [](Process& a, Process& b) {
            return a.priority < b.priority;
        });
        ganttChart.push_back({highestPriority->id, {currentTime, currentTime + highestPriority->burstTime}});
        currentTime += highestPriority->burstTime;
        highestPriority->completionTime = currentTime;
        completedProcesses.push_back(*highestPriority);
        readyQueue.erase(highestPriority);
    }
    Output output = calculateMetrics(completedProcesses);
    output.ganttChart = ganttChart;
    return output;
}


// Round Robin (RR) Scheduling
Output rr() {
    Output a;
    a.algorithmName = "Round Robin (RR) Scheduling";
    vector<Process> processes = inputProcesses;
    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
    }
    sort(processes.begin(), processes.end(), [](Process& a, Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    int currentTime = 0;
    queue<Process*> readyQueue;
    vector<pair<int, pair<int, int>>> ganttChart;
    for (auto& p : processes) {
        readyQueue.push(&p);
    }
    while (!readyQueue.empty()) {
        Process* p = readyQueue.front();
        readyQueue.pop();
        if (currentTime < p->arrivalTime) {
            currentTime = p->arrivalTime;
        }
        int executionTime = min(quantumTime, p->remainingTime);
        p->remainingTime -= executionTime;
        ganttChart.push_back({p->id, {currentTime, currentTime + executionTime}});
        currentTime += executionTime;
        if (p->remainingTime > 0) {
            readyQueue.push(p);
        } else {
            p->completionTime = currentTime;
        }
    }
    Output output = calculateMetrics(processes);
    output.ganttChart = ganttChart;
    return output;
}

// Improved Round Robin (IRR) Scheduling
Output irr() {
    Output a;
    a.algorithmName = "Improved Round Robin (IRR) Scheduling";
    vector<Process> processes = inputProcesses;
    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
    }
    sort(processes.begin(), processes.end(), [](Process& a, Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    int currentTime = 0;
    queue<Process*> readyQueue;
    vector<pair<int, pair<int, int>>> ganttChart;
    for (auto& p : processes) {
        readyQueue.push(&p);
    }
    // First round of RR
    while (!readyQueue.empty()) {
        Process* p = readyQueue.front();
        readyQueue.pop();
        if (currentTime < p->arrivalTime) {
            currentTime = p->arrivalTime;
        }
        int executionTime = min(quantumTime, p->remainingTime);
        p->remainingTime -= executionTime;
        ganttChart.push_back({p->id, {currentTime, currentTime + executionTime}});
        currentTime += executionTime;
        if (p->remainingTime > 0) {
            readyQueue.push(p);
        } else {
            p->completionTime = currentTime;
        }
    }
    // Second round with shortest job first
    vector<Process*> remainingProcesses;
    for (auto& p : processes) {
        if (p.remainingTime > 0) {
            remainingProcesses.push_back(&p);
        }
    }
    sort(remainingProcesses.begin(), remainingProcesses.end(), [](Process* a, Process* b) {
        return a->remainingTime < b->remainingTime;
    });
    while (!remainingProcesses.empty()) {
        Process* p = remainingProcesses.front();
        remainingProcesses.erase(remainingProcesses.begin());
        ganttChart.push_back({p->id, {currentTime, currentTime + p->remainingTime}});
        currentTime += p->remainingTime;
        p->remainingTime = 0;
        p->completionTime = currentTime;
    }
    Output output = calculateMetrics(processes);
    output.ganttChart = ganttChart;
    return output;
}

// Function to find the best algorithm based on lowest average waiting time and average turnaround time
void findBestAlgorithm(const vector<Output>& outputs) {
    int bestIndex = 0;
    double lowestAverageWaitingTime = outputs[0].averageWaitingTime;
    double lowestAverageTurnaroundTime = outputs[0].averageTurnaroundTime;
    for (int i = 1; i < outputs.size(); ++i) {
        if (outputs[i].averageWaitingTime < lowestAverageWaitingTime) {
            lowestAverageWaitingTime = outputs[i].averageWaitingTime;
            bestIndex = i;
        }
        if (outputs[i].averageTurnaroundTime < lowestAverageTurnaroundTime) {
            lowestAverageTurnaroundTime = outputs[i].averageTurnaroundTime;
            bestIndex = i;
        }
    }
    cout << "The best scheduling algorithm is: ";
    switch (bestIndex) {
        case 0: cout << "First-Come, First-Served (FCFS) Scheduling\n"; break;
        case 1: cout << "Shortest Job First (SJF) Scheduling\n"; break;
        case 2: cout << "Priority Scheduling\n"; break;
        case 3: cout << "Round Robin (RR) Scheduling\n"; break;
        case 4: cout << "Improved Round Robin (IRR) Scheduling\n"; break;
    }
    cout << "With the lowest average waiting time of: " << lowestAverageWaitingTime << " ms\n";
    cout << "And the lowest average turnaround time of: " << lowestAverageTurnaroundTime << " ms\n";
}

int main() {
    int n;
    cin >> n;
    inputProcesses.resize(n);
    for (int i = 0; i < n; ++i) {
        inputProcesses[i].id = i + 1;
        cin >> inputProcesses[i].arrivalTime >> inputProcesses[i].burstTime >> inputProcesses[i].priority;
    }
    cin >> quantumTime;

    vector<Output> outputs;
    outputs.push_back(fcfs());
    outputs.push_back(sjf());
    outputs.push_back(priorityScheduling());
    outputs.push_back(rr());
    outputs.push_back(irr());

    for (const auto& output : outputs) {
        cout << "\nGantt Chart:\n";
        for (const auto& entry : output.ganttChart) {
            cout << "P" << entry.first << ": " << entry.second.first << " - " << entry.second.second << "\n";
        }
        cout << "Average Waiting Time: " << output.averageWaitingTime << " ms\n";
        cout << "Average Turnaround Time: " << output.averageTurnaroundTime << " ms\n";
        cout << "CPU Utilization: " << output.cpuUtilization << " %\n";
    }

    findBestAlgorithm(outputs);
    return 0;
}
