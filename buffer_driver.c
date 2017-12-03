/*
 * This problem has you solve the classic "bounded buffer" problem with
 * one producer and multiple consumers:
 *
 *                                       ------------
 *                                    /->| consumer |
 *                                    |  ------------
 *                                    |
 *  ------------                      |  ------------
 *  | producer |-->   bounded buffer --->| consumer |
 *  ------------                      |  ------------
 *                                    |
 *                                    |  ------------
 *                                    \->| consumer |
 *                                       ------------
 *
 *  The program below includes everything but the implementation of the
 *  bounded buffer itself.  main() does this:
 *
 *  1. starts N consumers as per the first argument (default 1)
 *  2. starts the producer
 *
 *  The producer reads positive integers from standard input and passes those
 *  into the buffer.  The consumers read those integers and "perform a
 *  command" based on them (all they really do is sleep for some period...)
 *
 *  on EOF of stdin, the producer passes N copies of 0 into the buffer.
 *  The consumers interpret 0 as a signal to exit.
 */

#include <stdio.h>
#include <stdlib.h>             /* atoi() */
#include <unistd.h>             /* usleep() */
#include <assert.h>             /* assert() */
#include <signal.h>             /* signal() */
#include <alloca.h>             /* alloca() */
#include <pthread.h>

#include "buffer.h"

/**************************************************************************\
 *                                                                        *
 * consumer thread.  main starts any number of consumer threads.  Each    *
 * consumer thread reads and "interprets" numbers from the bounded        *
 * buffer.                                                                *
 *                                                                        *
 * The interpretation is as follows:                                      *
 *                                                                        *
 * o  positive integer N: sleep for N * 100ms                             *
 * o  negative integer:  exit                                             *
 *                                                                        *
 * Note that a thread procedure (called by pthread_create) is required to *
 * take a void * as an argument and return a void * as a result.  We play *
 * a dirty trick: we pass the thread number (main()'s idea of the thread  *
 * number) as the "void *" argument.  Hence the casts.  The return value  *
 * is ignored so we return NULL.                                          *
 *                                                                        *
\**************************************************************************/

void *consumer_thread(void *raw_consumerno)
{
  int consumerno = (int)raw_consumerno; /* dirty trick to pass in an integer */

  printf("  consumer %d: starting\n", consumerno);
  while (1)
    {
      //printf("Has not extracted buffer yet but next line does\n");
      int number = buffer_extract();
      //printf("Buffer has extracted: %i\n", number);
      if (number == 0) {
        break;
      }

      printf("  consumer %d: %d\n", consumerno, number);
      fflush(stdout);
    }

  printf("  consumer %d: exiting\n", consumerno);
  return(NULL);
}

/**************************************************************************\
 *                                                                        *
 * producer.  main calls the producer as an ordinary procedure rather     *
 * than creating a new thread.  In other words the original "main" thread *
 * becomes the "producer" thread.                                         *
 *                                                                        *
 * The producer reads numbers from stdin, and inserts them into the       *
 * bounded buffer.  On EOF from stdin, it finished up by inserting a 0   *
 * for every consumer thread so that all the consumer thread shut down    *
 * cleanly.                                                               *
 *                                                                        *
\**************************************************************************/

#define MAXLINELEN 128

void producer(int nconsumers)
{
  char buffer[MAXLINELEN];
  int number;

  printf("  producer: starting\n");

  while (fgets(buffer, MAXLINELEN, stdin) != NULL)
    {
      number = atoi(buffer);
      printf("producer: %d\n", number);
      buffer_insert(number);
    }

  printf("producer: read EOF, sending %d '0' numbers\n", nconsumers);
  for (number = 0; number < nconsumers; number++)
    buffer_insert(0);

  printf("producer: exiting\n");
}

/*************************************************************************\
 *                                                                       *
 * main program.  Main calls buffer_init and does other initialization,  *
 * fires of N copies of the consumer (N given by command-line argument), *
 * then calls the producer.                                              *
 *                                                                       *
\*************************************************************************/

int main(int argc, char *argv[])
{
  pthread_t *consumers;
  int nconsumers = 1;
  int kount;

  if (argc > 1)
    nconsumers = atoi(argv[1]);

  printf("main: nconsumers = %d\n", nconsumers);
  assert(nconsumers >= 0);

  /*
   * 1. initialization
   */
  buffer_init();
  signal(SIGALRM, SIG_IGN);     /* evil magic for usleep() under solaris */

  /*
   * 2. start up N consumer threads
   */
  consumers = (pthread_t *)alloca(nconsumers * sizeof(pthread_t));
  for (kount = 0; kount < nconsumers; kount++)
    {
      int test = pthread_create(&consumers[kount], /* pthread number */
                                NULL,            /* "attributes" (unused) */
                                consumer_thread, /* procedure */
                                (void *)kount);  /* hack: consumer number */

      assert(test == 0);
    }

  /*
   * 3. run the producer in this thread.
   */
  producer(nconsumers);

  /*
   * n. clean up: the producer told all the consumers to shut down (by
   *    sending 0 to each).  Now wait for them all to finish.
   */
  for (kount = 0; kount < nconsumers; kount++)
    {
      int test = pthread_join(consumers[kount], NULL);

      assert(test == 0);
    }
  return(0);

  //Fall 2017
}
