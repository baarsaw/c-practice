#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define NUM_DESKS 10            // Number of desks available
#define NUM_STUDENTS 12         // Number of students that need to study

sem_t desks;

void *handle_student(void *arg) {
    int student = *((int*)arg);
    
    // Randomize students arrival time
    int arrival_time = rand() % 5 + 1;
    sleep(arrival_time);
    printf("Student %d arrives after %d seconds\n", student, arrival_time);

    // Get or wait for a desk
    sem_wait(&desks);

    // Critical Section
    printf("Student %d got a desk\n", student);

    int study_time = rand() % 5 + 1;
    sleep(study_time);
    printf("Student %d finished studying after %d seconds\n", student, study_time);
    // End of Critical Section

    // Release a desk for other students
    sem_post(&desks);
    printf("Student %d leaves the library\n", student);

    return NULL;
}

int main() {
    pthread_t threads[NUM_STUDENTS];
    int students[NUM_STUDENTS];

    srand(time(NULL));

    // Initalize the semaphore (at most NUM_DESKS can be occupied)
    sem_init(&desks, 0, NUM_DESKS);

    // Spawn student threads
    for (int i = 0; i < NUM_STUDENTS; i++) {
        students[i] = i + 1;
        pthread_create(&threads[i], NULL, handle_student, &students[i]);
    }

    // Wait for every student to finish studying
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}