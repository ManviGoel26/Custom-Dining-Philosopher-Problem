// Blocking Semaphore 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* eating(void* arg);

// UTILITY CODE
void perror_exit (char *s)
{
    perror(s);  
    exit(-1);
}

void *check_malloc(int size)
{
    void *p = malloc (size);
    if (p == NULL) 
        {
            perror_exit ("malloc failed");
        }

    return p;
}



// MUTEX WRAPPER
typedef pthread_mutex_t Mutex;

Mutex *make_mutex ()
{
    Mutex *mutex = check_malloc (sizeof(Mutex));
    int n = pthread_mutex_init (mutex, NULL);
    if (n != 0) 
        {
            perror_exit ("make_lock failed"); 
        }

    return mutex;
}

void mutex_lock (Mutex *mutex)
{
    int n = pthread_mutex_lock (mutex);
    if (n != 0) 
    {
        perror_exit ("lock failed");
    }
}

void mutex_unlock (Mutex *mutex)
{
    int n = pthread_mutex_unlock (mutex);
    if (n != 0) 
        {
            perror_exit ("unlock failed");
        }
}



// COND WRAPPER
typedef pthread_cond_t Cond;

Cond *make_cond ()
{
    Cond *cond = check_malloc (sizeof(Cond)); 
    int n = pthread_cond_init (cond, NULL);
    if (n != 0) 
        {
            perror_exit ("make_cond failed");
        }
 
    return cond;
}

void cond_wait (Cond *cond, Mutex *mutex)
{
    int n = pthread_cond_wait (cond, mutex);
    if (n != 0) 
        {
            perror_exit ("cond_wait failed");
        }
}

void cond_signal (Cond *cond)
{
    int n = pthread_cond_signal (cond);
    if (n != 0) 
        {
            perror_exit ("cond_signal failed");
        }
}



typedef struct sem_t{
    int value, wakeups;
    Mutex *mutex1;
    Mutex *mutex2;
    Cond *cond;
} my_semaphore;


// SEMAPHORE
my_semaphore *make_semaphore (int value)
{
  my_semaphore *semaphore = check_malloc (sizeof(my_semaphore));
  semaphore->value = value;
  semaphore->wakeups = 0;
  semaphore->mutex1 = make_mutex ();
  semaphore->mutex2 = make_mutex ();
  semaphore->cond = make_cond ();

  return semaphore;
}

void sem_wait (my_semaphore *semaphore)
{
    mutex_lock (semaphore->mutex1);
    semaphore->value--;

    if (semaphore->value < 0) 
    {
        do 
        {
            cond_wait (semaphore->cond, semaphore->mutex1);
        } 
        while (semaphore->wakeups < 1);
    
        semaphore->wakeups--;
    }

    mutex_unlock (semaphore->mutex1);
}

void sem_signal (my_semaphore *semaphore)
{
    mutex_lock(semaphore->mutex1);
    semaphore->value++;

    if (semaphore->value <= 0) 
    {
        semaphore->wakeups++;
        cond_signal(semaphore->cond);
    }
    mutex_unlock (semaphore->mutex1);
}


void sem_signal_printValue(int printValue, my_semaphore *semaphore)
{
  mutex_lock(semaphore->mutex2);
  semaphore->value++;

  if (semaphore->value <= 0) 
  {
    semaphore->wakeups++;
    cond_signal(semaphore->cond);
  }
  printf("Value: %d\n",semaphore->value);
  mutex_unlock (semaphore->mutex2);
}

int n;
my_semaphore *forks[100];
my_semaphore *bowls;


int main(int argc, char* argv[]) 
{
    n = atoi(argv[1]); //The number of philisophers
    int list_philosphers[n];
    pthread_t philisophers[n];

    bowls = make_semaphore(1);
    for (int i = 0; i < n; i++)
    {
        forks[i] = make_semaphore(1);
    }

    for (int i = 0; i < n; i++)
    {
        list_philosphers[i] = i;
        pthread_create(&philisophers[i], NULL, eating, (void*)&list_philosphers[i]);
    }

    for (int i = 0; i < n; i++)
    {
        pthread_join(philisophers[i], NULL);
    }
    return 0; 
} 


void* eating(void* num) 
{ 
    int phil = *(int *)num;

    while (1)
    {
        int left_fork = phil;
        int right_fork = (phil+1)%n;
        sem_wait(forks[left_fork]);
        sem_wait(forks[right_fork]);
        sem_wait(bowls);
        
        sleep(5);
        printf("\nPhilosopher %d eats using forks %d and %d",phil, left_fork, right_fork);

        sem_signal(forks[right_fork]);
        sem_signal(forks[left_fork]);
        sem_signal(bowls);
        
        sleep(2);

    }
    
} 

