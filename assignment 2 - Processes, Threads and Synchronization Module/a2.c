#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"

int sem_id0, sem_id1, sem_id2, sem_id3, sem_id4, sem_id5;
int i = 0;
pthread_mutex_t lock;
pthread_cond_t wait_T15;
pthread_cond_t wait_for_5_threads;
pthread_cond_t waiting;
int OK = 0;
int waits = 1;

void P(int sem_id, int sem_no)
{
    struct sembuf op = { sem_no, -1, 0 };
    semop(sem_id, &op, 1);
}

void V(int sem_id, int sem_no)
{
    struct sembuf op = { sem_no, +1, 0 };
    semop(sem_id, &op, 1);
}

void* execThread1(void* threadID) {
    int ID1 = *((int*)threadID);

    if (ID1 == 1) {
        P(sem_id0, 0); //T7.1 needs to wait for T7.3 to start before starting itself
    }

    if (ID1 == 4) {
        P(sem_id3, 0); //T7.4 needs to start only after T3.4 ends
    }

    info(BEGIN, 7, ID1);

    if (ID1 == 3) {
        V(sem_id0, 0); //once T7.3 starts, it gives permission to T7.1 to start
        P(sem_id2, 0); //T7.3 needs to wait until T7.1 ends
    }

    info(END, 7, ID1);

    if (ID1 == 1) {
        V(sem_id2, 0); //once T7.1 ends, T7.3 can end as well
    }

    if(ID1==4){
        V(sem_id4,0);     //once T7.4 ends, T3.6 can start
    }

    return NULL;
}

void* execThread2(void* threadID) {
    int ID2 = *((int*)threadID);

    if (ID2 != 15) {
        pthread_mutex_lock(&lock);

        while (OK == 0) {  //OK will be changed once the sequence containing T6.15 is processed, though the waiting will not be needed for the rest of the threads
            pthread_cond_wait(&wait_T15, &lock);  //In that sequence of 5 threads, we want T6.15 to be the first one so the others are locked until T6.15 starts
        }

        pthread_mutex_unlock(&lock);
    }

    P(sem_id1, 0);
    info(BEGIN, 6, ID2);

    P(sem_id5, 0);
    i++;              // counting the number of threads currently in the critical region
    if (i == 5) {
        pthread_cond_signal(&waiting);  //once there are 5 threads in total, T6.15 can terminate
    }
    V(sem_id5, 0);

    if (ID2 == 15) {
        OK = 1;  //OK is changed once T6.15 is found
        pthread_cond_broadcast(&wait_T15);  //free all the threads waiting, because we already got T6.15 as first

        pthread_mutex_lock(&lock);
        pthread_cond_wait(&waiting, &lock); //the thread should wait until there are 5 threads in total
        pthread_mutex_unlock(&lock);

       //once the number of threads besides T6.15 = 4, that means that T6.15 can terminate and it will release the blocked threads that waited for his execution
            info(END, 6, ID2);
            waits = 0;
            pthread_cond_broadcast(&wait_for_5_threads);
            V(sem_id1, 0);
    }
    else
    {
            pthread_mutex_lock(&lock);

            while (waits == 1) { //for all the threads that have begun in the sequence with T6.15 (that means waits is 1), it is needed for them to wait until 5 threads are in the critical region
                pthread_cond_wait(&wait_for_5_threads, &lock); //the waits variable will be changed once the final number in the sequence is 5 => the other threads that will come later will not be blocked, they will terminate normally
            }

            pthread_mutex_unlock(&lock);
            info(END, 6, ID2);
            V(sem_id1, 0);
    }

    return NULL;
}

void* execThread3(void* threadID) {
    int ID3 = *((int*)threadID);

    if(ID3==6){
        P(sem_id4,0); //T3.6 needs to start only after T7.4 ends
    }

    info(BEGIN, 3, ID3);
    info(END, 3, ID3);

    if (ID3 == 4) {
        V(sem_id3, 0);  //once T3.4 ends, T7.4 can start
    }

    return NULL;
}

int main() {
    init();

    pid_t pid2 = -1;
    pid_t pid6 = -1;
    pid_t pid3 = -1;
    pid_t pid5 = -1;
    pid_t pid8 = -1;
    pid_t pid4 = -1;
    pid_t pid7 = -1;

    info(BEGIN, 1, 0);

    sem_id0 = semget(10000, 1, IPC_CREAT | 0600);
    sem_id1 = semget(30000, 1, IPC_CREAT | 0600);
    sem_id2 = semget(20000, 1, IPC_CREAT | 0600);
    sem_id3 = semget(40000, 1, IPC_CREAT | 0600);
    sem_id4 = semget(50000, 1, IPC_CREAT | 0600);
    sem_id5 = semget(60000, 1, IPC_CREAT | 0600);

    if (sem_id0 < 0) {
        perror("Error creating the semaphore set");
        exit(0);
    }

    if (sem_id1 < 0) {
        perror("Error creating the semaphore set");
        exit(1);
    }

    if (sem_id2 < 0) {
        perror("Error creating the semaphore set");
        exit(2);
    }

    if (sem_id3 < 0) {
        perror("Error creating the semaphore set");
        exit(3);
    }

    if (sem_id4 < 0) {
        perror("Error creating the semaphore set");
        exit(4);
    }

    if (sem_id5 < 0) {
        perror("Error creating the semaphore set");
        exit(5);
    }

    semctl(sem_id0, 0, SETVAL, 0);
    semctl(sem_id1, 0, SETVAL, 5);
    semctl(sem_id2, 0, SETVAL, 0);
    semctl(sem_id3, 0, SETVAL, 0);
    semctl(sem_id4, 0, SETVAL, 0);
    semctl(sem_id5, 0, SETVAL, 1);

    pid2 = fork();

    if (pid2 == -1) {
        printf("Couldn't create child process 2");
        exit(6);
    }
    else if (pid2 > 0) {
        waitpid(pid2, NULL, 0);
    }
    else if (pid2 == 0) {
        info(BEGIN, 2, 0);

        pid3 = fork();

        if (pid3 == -1) {
            printf("Couldn't create child process 3");
            exit(7);
        }
        else if (pid3 == 0) {
            info(BEGIN, 3, 0);

            pid4 = fork();

            if (pid4 == -1) {
                printf("Couldn't create child process 4");
                exit(8);
            }
            else if (pid4 > 0) {
                pthread_t* t3;
                t3 = (pthread_t*)malloc(sizeof(pthread_t) * 6);
                int* ids3;
                ids3 = (int*)malloc(sizeof(int) * 6);

                for (int i = 0; i < 6; i++) {
                    ids3[i] = i + 1;
                    if (pthread_create(&t3[i], NULL, execThread3, (void*)&ids3[i]) != 0) {
                        perror("Error creating a new thread");
                        exit(9);
                    }
                }

                for (int i = 0; i < 6; i++) {
                    pthread_join(t3[i], NULL);
                }

                semctl(sem_id3, 0, IPC_RMID, 0);
                semctl(sem_id4, 0, IPC_RMID, 0);

                waitpid(pid4, NULL, 0);
            }
            else if (pid4 == 0) {
                info(BEGIN, 4, 0);
                info(END, 4, 0);
                exit(10);
            }

            info(END, 3, 0);
            exit(11);
        }

        pid5 = fork();

        if (pid5 == -1) {
            printf("Couldn't create child process 5");
            exit(12);
        }
        else if (pid5 == 0) {
            info(BEGIN, 5, 0);

            pid7 = fork();

            if (pid7 == -1) {
                printf("Couldn't create child process 7");
                exit(13);
            }
            else if (pid7 > 0) {
                waitpid(pid7, NULL, 0);
            }
            else if (pid7 == 0) {
                info(BEGIN, 7, 0);

                pthread_t* t1;
                t1 = (pthread_t*)malloc(sizeof(pthread_t) * 5);
                int* ids1;
                ids1 = (int*)malloc(sizeof(int) * 5);

                for (int i = 0; i < 5; i++) {
                    ids1[i] = i + 1;
                    if (pthread_create(&t1[i], NULL, execThread1, (void*)&ids1[i]) != 0) {
                        perror("Error creating a new thread");
                        exit(14);
                    }
                }

                for (int i = 0; i < 5; i++) {
                    pthread_join(t1[i], NULL);
                }

                semctl(sem_id0, 0, IPC_RMID, 0);
                semctl(sem_id2, 0, IPC_RMID, 0);

                info(END, 7, 0);
                exit(15);
            }

            info(END, 5, 0);
            exit(16);
        }

        pid8 = fork();

        if (pid8 == -1) {
            printf("Couldn't create child process 8");
            exit(17);
        }
        else if (pid8 == 0) {
            info(BEGIN, 8, 0);
            info(END, 8, 0);
            exit(18);
        }

        waitpid(pid8, NULL, 0);
        waitpid(pid5, NULL, 0);
        waitpid(pid3, NULL, 0);

        info(END, 2, 0);
        exit(19);
    }

    pid6 = fork();

    if (pid6 == -1) {
        printf("Couldn't create child process 6");
        exit(20);
    }
    else if (pid6 > 0) {
        waitpid(pid6, NULL, 0);
    }
    else if (pid6 == 0) {
        info(BEGIN, 6, 0);

        pthread_t* t2;
        t2 = (pthread_t*)malloc(sizeof(pthread_t) * 37);
        int* ids2;
        ids2 = (int*)malloc(sizeof(int) * 37);

        for (int i = 0; i < 37; i++) {
            ids2[i] = i + 1;
            if (pthread_create(&t2[i], NULL, execThread2, (void*)&ids2[i]) != 0) {
                perror("Error creating a new thread");
                exit(21);
            }
        }

        for (int i = 0; i < 37; i++) {
            pthread_join(t2[i], NULL);
        }

        semctl(sem_id1, 0, IPC_RMID, 0);
        semctl(sem_id5, 0, IPC_RMID, 0);

        info(END, 6, 0);
        exit(22);
    }

    info(END, 1, 0);
    return 0;
}
