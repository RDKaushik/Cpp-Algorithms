#include <iostream>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>

#define TOTAL_CPU_TIME (rand() % 100 + 1) * 1000000
#define RANDOM_PROCESS_INCOMING (rand() % 10000 + 1)
#define RANDOM_BURST_TIME (rand() % 700 + 1)
#define INITIAL_PROCESS_COUNT 5
#define TIME_QUANTUM 50

int p_id = 1; // Initial Process ID

using namespace std;

class GanttChart
{
public:
  int p_id;
  int startTime;
  int endTime;
};

class Process
{
public:
  int p_id;
  int burstTime;
  Process *next;
  Process *prev;
};

void scheduleProcesses(Process **);
void dispatchProcesses(Process **);
void automateRoundRobin(Process **);
void removeProcess(Process **);
Process *getInitialProcesses();
void addProcesses(Process **, int, int);
void printProcesses(Process *);

int main()
{
  srand(time(NULL));

  Process *head = getInitialProcesses();
  scheduleProcesses(&head);

  /* Debug Purposes*/
  //printProcesses(head);

  return 0;
}

void scheduleProcesses(Process **head)
{
  int totalCpuTime = TOTAL_CPU_TIME;
  chrono::duration<long int, std::micro> cpuTimeDifference, // keeps track of when to stop cpu
      addProcessTime;                                       // keeps track of when to bring new process
  auto cpuStart = chrono::high_resolution_clock::now();
  auto newProcessStart = chrono::high_resolution_clock::now();

  cout << "::Scheduler picked up the initial processes::" << endl
       << endl;

  do
  {
    if (*head)
    {
      cout << "<-Processes in queue->" << endl;
      printProcesses(*head);

      cout << "--Process with process_id " << (*head)->p_id << " is dispatched to the CPU--" << endl;
      dispatchProcesses(head);
    }
    else
    {
      cout << "Process queue Empty!" << endl
           << endl;
    }

    auto cpuEnd = chrono::high_resolution_clock::now();
    addProcessTime = chrono::duration_cast<chrono::microseconds>(cpuEnd - newProcessStart);
    cpuTimeDifference = chrono::duration_cast<chrono::microseconds>(cpuEnd - cpuStart);

    if (addProcessTime.count() >= RANDOM_PROCESS_INCOMING)
    {
      addProcesses(head, p_id++, RANDOM_BURST_TIME);
      newProcessStart = chrono::high_resolution_clock::now();
    }
  } while (totalCpuTime >= cpuTimeDifference.count());
}

void dispatchProcesses(Process **head)
{
  automateRoundRobin(head);
}

void automateRoundRobin(Process **head)
{
  int burstTime = (*head)->burstTime;
  int processingTime;

  if (TIME_QUANTUM > burstTime)
  {
    processingTime = burstTime;
  }
  else
  {
    processingTime = TIME_QUANTUM;
  }
  (*head)->burstTime -= TIME_QUANTUM;

  cout << "**CPU processing the process with process_id " << (*head)->p_id << "**" << endl;
  this_thread::sleep_for(std::chrono::microseconds(processingTime));

  if ((*head)->burstTime <= 0)
  {
    cout << "~~CPU has processed the process with process_id " << (*head)->p_id << "~~" << endl
         << endl;
    removeProcess(head);
  }
  else
  {
    cout << "``CPU has served the process with process_id " << (*head)->p_id << "``" << endl
         << endl;
    (*head) = (*head)->next;
  }
}

void removeProcess(Process **head)
{
  Process *temp = *head;

  if ((*head)->next == *head)
  {
    *head = NULL;
  }
  else
  {
    *head = temp->next;
    temp->prev->next = *head;
    (*head)->prev = temp->prev;
  }

  delete temp;
}

Process *getInitialProcesses()
{
  Process *head = NULL;

  srand(time(NULL));

  for (int i = 0; i < INITIAL_PROCESS_COUNT; i++)
  {
    addProcesses(&head, p_id++, RANDOM_BURST_TIME);
  }

  return head;
}

void addProcesses(Process **head, int p_id, int burstTime)
{
  Process *temp = new Process;
  temp->p_id = p_id;
  temp->burstTime = burstTime;

  if (*head)
  {
    temp->next = *head;
    temp->prev = (*head)->prev;
    (*head)->prev->next = temp;
    (*head)->prev = temp;
  }
  else
  {
    *head = temp;
    (*head)->next = *head;
    (*head)->prev = *head;
  }
}

void printProcesses(Process *head)
{
  Process *temp;
  if (head)
  {
    temp = head;
    do
    {
      cout << temp->p_id << " " << temp->burstTime << endl;
      temp = temp->next;
    } while (temp != head);

    cout << endl;
  }
}