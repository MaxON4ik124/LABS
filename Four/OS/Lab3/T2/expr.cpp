#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <chrono>
#include <cerrno>
#include <cstring>

using namespace std;

struct Task {
    int index;
    long long sum;
};

const int STOP_INDEX = -1;
const int SPLIT_DEPTH = 20;

queue<Task> taskQueue;

sem_t state_sem;
sem_t tasks_sem;
sem_t done_sem;
sem_t started_sem;

vector<int> A;
int N;
long long TARGET;
long long ANSWER = 0;

int THREADS;

long long tasks_in_system = 0;

void sem_wait_nointr(sem_t* sem)
{
    while (sem_wait(sem) == -1) {
        if (errno != EINTR) {
            cerr << "sem_wait error: " << strerror(errno) << endl;
            exit(1);
        }
    }
}

long long dfs_seq(Task task)
{
    if (task.index == N)
        return task.sum == TARGET;

    Task taskAdd = {task.index + 1, task.sum + A[task.index]};
    Task taskSub = {task.index + 1, task.sum - A[task.index]};

    return dfs_seq(taskAdd) + dfs_seq(taskSub);
}

void push_task(Task task)
{
    sem_wait_nointr(&state_sem);

    taskQueue.push(task);
    tasks_in_system++;

    sem_post(&state_sem);
    sem_post(&tasks_sem);
}

void push_stop_task()
{
    sem_wait_nointr(&state_sem);

    taskQueue.push({STOP_INDEX, 0});

    sem_post(&state_sem);
    sem_post(&tasks_sem);
}

void finish_task()
{
    sem_wait_nointr(&state_sem);

    tasks_in_system--;

    if (tasks_in_system == 0) {
        sem_post(&done_sem);
    }

    sem_post(&state_sem);
}

void add_to_answer(long long value)
{
    if (value == 0)
        return;

    sem_wait_nointr(&state_sem);

    ANSWER += value;

    sem_post(&state_sem);
}

void* worker(void*)
{
    sem_post(&started_sem);

    while (true) {
        sem_wait_nointr(&tasks_sem);

        sem_wait_nointr(&state_sem);

        Task task = taskQueue.front();
        taskQueue.pop();

        sem_post(&state_sem);

        if (task.index == STOP_INDEX) {
            return nullptr;
        }

        long long local_ans = 0;

        if (task.index == N) {
            local_ans = task.sum == TARGET;
        }
        else if (task.index >= SPLIT_DEPTH) {
            local_ans = dfs_seq(task);
        }
        else {
            push_task({task.index + 1, task.sum + A[task.index]});
            push_task({task.index + 1, task.sum - A[task.index]});
        }

        add_to_answer(local_ans);

        finish_task();
    }
}

int main()
{
    sem_init(&state_sem, 0, 1);
    sem_init(&tasks_sem, 0, 0);
    sem_init(&done_sem, 0, 0);
    sem_init(&started_sem, 0, 0);


    ifstream file("input.txt");
    file >> THREADS >> N;
    A.resize(N);
    for (int i = 0; i < N; i++) file >> A[i];
    file >> TARGET;

    vector<pthread_t> workers(THREADS);

    int created_threads = 0;

    for (int i = 0; i < THREADS; i++) {
        int rc = pthread_create(&workers[i], nullptr, worker, nullptr);

        created_threads++;
    }

    for (int i = 0; i < THREADS; i++) {
        sem_wait_nointr(&started_sem);
    }

    auto start = chrono::high_resolution_clock::now();

    push_task({1, A[0]});

    sem_wait_nointr(&done_sem);

    auto end = chrono::high_resolution_clock::now();

    long long time_ms =
        chrono::duration_cast<chrono::milliseconds>(end - start).count();

    for (int i = 0; i < THREADS; i++) {
        push_stop_task();
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(workers[i], nullptr);
    }

    ofstream out("output.txt");

    out << THREADS << "\n";
    out << N << "\n";
    out << ANSWER;

    ofstream tout("time.txt");


    tout << time_ms;

    sem_destroy(&state_sem);
    sem_destroy(&tasks_sem);
    sem_destroy(&done_sem);
    sem_destroy(&started_sem);

    return 0;
}