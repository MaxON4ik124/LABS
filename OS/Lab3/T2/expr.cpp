#include <pthread.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <chrono>

using namespace std;

struct Task {
    int index;
    long long sum;
};

queue<Task> taskQueue;

pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;
pthread_cond_t done_cond;

bool stop_all = false;
int working_threads = 0;

vector<int> A;
long long TARGET;
long long ANSWER = 0;

int THREADS;
const int SPLIT_DEPTH = 20;

long long dfs_seq(int idx, long long sum)
{
    if(idx == A.size()) return (sum == TARGET);

    return dfs_seq(idx+1, sum + A[idx]) + dfs_seq(idx+1, sum - A[idx]);
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

        long long local_ans = 0;

        if(task.index == A.size())
        {
            local_ans = (task.sum == TARGET);
        }
        else if(task.index >= SPLIT_DEPTH)
        {
            local_ans = dfs_seq(task.index, task.sum);
        }
        else
        {
            pthread_mutex_lock(&queue_mutex);
            taskQueue.push({task.index+1, task.sum + A[task.index]});
            taskQueue.push({task.index+1, task.sum - A[task.index]});
            pthread_cond_broadcast(&queue_cond);
            pthread_mutex_unlock(&queue_mutex);
        }

        if(local_ans > 0)
        {
            pthread_mutex_lock(&queue_mutex);
            ANSWER += local_ans;
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
    pthread_mutex_init(&queue_mutex,NULL);
    pthread_cond_init(&queue_cond,NULL);
    pthread_cond_init(&done_cond,NULL);

    ifstream file("input.txt");
    int N;
    file >> THREADS >> N;

    A.resize(N);
    for(int i=0;i<N;i++) file >> A[i];
    file >> TARGET;

    vector<pthread_t> workers(THREADS);
    for(int i=0;i<THREADS;i++)
        pthread_create(&workers[i],NULL,worker,NULL);

    auto start = chrono::high_resolution_clock::now();

    pthread_mutex_lock(&queue_mutex);
    taskQueue.push({1, A[0]});
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);

    pthread_mutex_lock(&queue_mutex);
    while(!(taskQueue.empty() && working_threads==0))
        pthread_cond_wait(&done_cond,&queue_mutex);
    pthread_mutex_unlock(&queue_mutex);

    auto end = chrono::high_resolution_clock::now();
    long long time_ms =
        chrono::duration_cast<chrono::milliseconds>(end-start).count();

    pthread_mutex_lock(&queue_mutex);
    stop_all = true;
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);

    for(auto &t : workers) pthread_join(t,NULL);

    ofstream out("output.txt");
    out << THREADS << "\n";
    out << N << "\n";
    out << ANSWER;

    ofstream tout("time.txt");
    tout << time_ms;
}