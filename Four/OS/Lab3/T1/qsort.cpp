#include <pthread.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <chrono>

using namespace std;

struct Task {
    vector<int>* arr;
    int beg;
    int end;
};

queue<Task> taskQueue;

pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;
pthread_cond_t done_cond;

bool stop_all = false;
int working_threads = 0;
int THREADS;

const int INSERTION_THRESHOLD = 1000;

int partition_qs(Task task)
{
    int pivot = (*task.arr)[task.end];
    int i = task.beg - 1;

    for(int j = task.beg; j < task.end; j++)
    {
        if((*task.arr)[j] <= pivot)
        {
            swap((*task.arr)[++i], (*task.arr)[j]);
        }
    }

    swap((*task.arr)[i + 1], (*task.arr)[task.end]);

    return i + 1;
}

void quicksort_seq(Task task)
{
    if(task.beg >= task.end)
        return;

    int p = partition_qs(task);

    quicksort_seq({task.arr, task.beg, p - 1});
    quicksort_seq({task.arr, p + 1, task.end});
}

void* worker(void*)
{
    while(true)
    {
        pthread_mutex_lock(&queue_mutex);

        while(taskQueue.empty() && !stop_all)
            pthread_cond_wait(&queue_cond, &queue_mutex);

        if(stop_all && taskQueue.empty())
        {
            pthread_mutex_unlock(&queue_mutex);
            return NULL;
        }

        Task task = taskQueue.front();
        taskQueue.pop();
        working_threads++;

        pthread_mutex_unlock(&queue_mutex);

        int size = task.end - task.beg + 1;

        if(size <= INSERTION_THRESHOLD)
        {
            quicksort_seq(task);
        }
        else
        {
            int piv = partition_qs(task);

            pthread_mutex_lock(&queue_mutex);

            taskQueue.push({task.arr, task.beg, piv - 1});
            taskQueue.push({task.arr, piv + 1, task.end});

            pthread_cond_broadcast(&queue_cond);

            pthread_mutex_unlock(&queue_mutex);
        }

        pthread_mutex_lock(&queue_mutex);

        working_threads--;

        if(taskQueue.empty() && working_threads == 0)
            pthread_cond_signal(&done_cond);

        pthread_mutex_unlock(&queue_mutex);
    }
}

int main()
{
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);
    pthread_cond_init(&done_cond, NULL);

    ifstream file("input.txt");

    int N;

    file >> THREADS >> N;

    vector<int> arr(N);

    for(int i = 0; i < N; i++)
        file >> arr[i];

    vector<pthread_t> workers(THREADS);

    for(int i = 0; i < THREADS; i++)
        pthread_create(&workers[i], NULL, worker, NULL);

    auto start = chrono::high_resolution_clock::now();

    pthread_mutex_lock(&queue_mutex);

    taskQueue.push({&arr, 0, N - 1});

    pthread_cond_broadcast(&queue_cond);

    pthread_mutex_unlock(&queue_mutex);

    pthread_mutex_lock(&queue_mutex);

    while(!(taskQueue.empty() && working_threads == 0))
        pthread_cond_wait(&done_cond, &queue_mutex);

    pthread_mutex_unlock(&queue_mutex);

    auto end = chrono::high_resolution_clock::now();

    long long time = chrono::duration_cast<chrono::milliseconds>(end - start).count();


    pthread_mutex_lock(&queue_mutex);

    stop_all = true;

    pthread_cond_broadcast(&queue_cond);

    pthread_mutex_unlock(&queue_mutex);

    for(auto &t : workers)
        pthread_join(t, NULL);

    ofstream out("output.txt");

    out << THREADS << "\n" << N << "\n";

    for(int x : arr)
        out << x << " ";

    ofstream tout("time.txt");

    tout << time;

    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);
    pthread_cond_destroy(&done_cond);
}