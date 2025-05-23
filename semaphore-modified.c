#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define NUM_DESKS 5            // Number of desks available
#define NUM_STUDENTS 8         // Number of students that need to study

sem_t desks;
sem_t open;
int program_loop = 1;

void* handle_library(void* arg) {
    int open_hours = 3;
    // library starts closed, open by posting to sem
    printf("\nLIBRARY IS CLOSED\n\n");
    while (program_loop == 1) {
        printf("\nLIBRARY IS OPEN\n\n");
        sem_post(&open);
        sleep(open_hours);
        printf("\nLIBRARY IS CLOSED\n\n");
        sem_wait(&open);
        sleep(open_hours);
    }
}

void *handle_student(void *arg) {
    int student = *((int*)arg);
    int library_is_open = 1;
    
    // Randomize students arrival time
    int arrival_time = rand() % 5 + 1;
    sleep(arrival_time);
    printf("Student %d arrives after %d seconds\n", student, arrival_time);

    // check if library is open before allowing the student to take a desk.
    sem_getvalue(&open, &library_is_open);
    while (!library_is_open) {
        printf("Student %d is waiting to enter the library.\n", student);
        sleep(1);
        sem_getvalue(&open, &library_is_open);
    }

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
    pthread_t lib_thread;
    int students[NUM_STUDENTS];

    srand(time(NULL));

    // Initalize the semaphore (at most NUM_DESKS can be occupied)
    sem_init(&desks, 0, NUM_DESKS);
    sem_init(&open, 0, 0); // library starts closed

    // Spawn student threads
    for (int i = 0; i < NUM_STUDENTS; i++) {
        students[i] = i + 1;
        pthread_create(&threads[i], NULL, handle_student, &students[i]);
    }
    // spawn library thread.
    pthread_create(&lib_thread, NULL, handle_library, NULL);
    
    // Wait for every student to finish studying
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(threads[i], NULL);
    }
    // stop library thread
    program_loop = 0;
    pthread_join(lib_thread, NULL);

    return 0;
}
