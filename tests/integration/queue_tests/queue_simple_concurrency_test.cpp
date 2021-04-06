#include "queue_tests.h"
#include "etcpal/thread.h"
#include <unordered_map>

static const unsigned large_prime = 115249;

bool writer_failure = false;
bool reader_failure = false;

static unsigned num_writes = 0;
static unsigned num_reads = 0;

std::unordered_map<long long int, unsigned> val_occurrences_map;

etcpal_sem_t occ_map_lock;
void init_concurrency_test()
{
  etcpal_sem_create(&occ_map_lock, 1, 1);
}

static void register_value_received(long long int val)
{
  etcpal_sem_wait(&occ_map_lock);

  if (val_occurrences_map.find(val) == val_occurrences_map.end())
  {
    val_occurrences_map[val] = 0;
  }
  val_occurrences_map[val]++;

  etcpal_sem_post(&occ_map_lock);
}

static bool check_occurrences(unsigned num_expected_occurrences)
{
  bool true_if_success = true;
  etcpal_sem_wait(&occ_map_lock);
  for (std::pair<long long int, unsigned> val_occ_pair : val_occurrences_map)
  {
    if (val_occ_pair.second != num_expected_occurrences)
    {
      true_if_success = false;
      printf("\r\n\tERROR: The value %lld occurred %d times, expected %d occurrences", val_occ_pair.first,
             val_occ_pair.second, num_expected_occurrences);
    }
  }
  if (!true_if_success)
  {
    printf("\r\n");
    Sleep(10);

  }
  etcpal_sem_post(&occ_map_lock);
  return true_if_success;
}

static void reset_occurrence_counter()
{
  etcpal_sem_wait(&occ_map_lock);
  val_occurrences_map.clear();
  etcpal_sem_post(&occ_map_lock);
}

static void writer_func(void* queue)
{
  etcpal::Queue<long long int>* q = static_cast<etcpal::Queue<long long int>*>(queue);
  //printf("Writer Queue: %p\r\n", queue);
  for (unsigned i = 0; i < num_writes; i++)
  {
    if (!q->Send((i+1) * large_prime))
    {
      writer_failure = true;
      break;
    }
  }
  // Sometimes I don't know where
  // This dirty road is taking me
  // Sometimes I can't even see the reason why
  // I guess I keep a-gamblin'
  // Lots of booze and lots of ramblin'
  // It's easier than just waitin' around to die
}


static void reader_func(void* queue)
{
  etcpal::Queue<long long int>* q = static_cast<etcpal::Queue<long long int>*>(queue);
  //printf("Reader Queue: %p\r\n", queue);
  for (unsigned i = 0; i < num_reads; i++)
  {
    long long int val;
    if (q->Receive(val))
    {
      register_value_received(val);
      if (val == 0 || val % large_prime != 0)
      {
        reader_failure = true;
        break;
      }
    }
   
  }
  // Sometimes I don't know where
  // This dirty road is taking me
  // Sometimes I can't even see the reason why
  // I guess I keep a-gamblin'
  // Lots of booze and lots of ramblin'
  // It's easier than just waitin' around to die
}

typedef struct _thread
{
  etcpal_thread_t    thread;
  EtcPalThreadParams params;
  char               thread_name[ETCPAL_THREAD_NAME_MAX_LENGTH];
}thread_t;

static thread_t* list_of_writers;
static thread_t* list_of_readers;

static void create_writer_test(etcpal::Queue<long long int>* q, int num_writers)
{
  char fmt[] = "Writer %d";
  list_of_writers = (thread_t*)calloc(sizeof(thread_t), num_writers);
  if (list_of_writers)
  {
    for (int i = 0; i < num_writers; i++)
    {
      thread_t* thread = &list_of_writers[i];
    
      thread->params.priority = 1;
      thread->params.stack_size = 0x5000;
      snprintf(thread->thread_name, ETCPAL_THREAD_NAME_MAX_LENGTH, fmt, i);
      thread->params.thread_name = thread->thread_name;

      etcpal_thread_create(&thread->thread, &thread->params, writer_func, q);
    }
  }
}

static void create_reader_test(etcpal::Queue<long long int>* q, int num_readers)
{
  char fmt[] = "Reader %d";
  list_of_readers = (thread_t*)calloc(sizeof(thread_t), num_readers);
  if (list_of_readers)
  {
    for (int i = 0; i < num_readers; i++)
    {
      thread_t* thread = &list_of_readers[i];

      thread->params.priority = 1;
      thread->params.stack_size = 0x5000;
      snprintf(thread->thread_name, ETCPAL_THREAD_NAME_MAX_LENGTH, fmt, i);
      thread->params.thread_name = thread->thread_name;

      etcpal_thread_create(&thread->thread, &thread->params, reader_func, q);
    }
  }
}

static void wait_for_writer_complete(int num_threads)
{
  for (int i = 0; i < num_threads; i++)
  {

    etcpal_thread_join(&(list_of_writers[i].thread));
  }
}

static void wait_for_reader_complete(int num_threads)
{
  for (int i = 0; i < num_threads; i++)
  {
    etcpal_thread_join(&(list_of_readers[i].thread));
  }
}

static void cleanup_writers()
{
  free(list_of_writers);
}

static void cleanup_readers()
{
  free(list_of_readers);
}

bool concurrent_small_queue_few_writers()
{
  writer_failure = false;

  // Each of 8 thread writes 1 element, to fill a queue with 8 slots
  int                          num_threads = 8;
  etcpal::Queue<long long int> q(8);
  num_writes = 1;

  create_writer_test(&q, num_threads);
  wait_for_writer_complete(num_threads);
  cleanup_writers();
  return !writer_failure;
}

bool concurrent_large_queue_few_writers_many_items()
{
  writer_failure = false;

  // Each of 8 threads writes 10000 elements, to fill a queue with 80000 elements
  num_writes = 10000;
  int                          num_threads = 8;
  etcpal::Queue<long long int> q(80000);

  create_writer_test(&q, num_threads);
  wait_for_writer_complete(num_threads);
  cleanup_writers();
  return !writer_failure;
}

bool concurrent_large_queue_many_writers_few_items()
{
  writer_failure = false;

  // Each of 100 threads writes 800 elements, to fill a queue with 80000 elements
  num_writes = 800;
  int                          num_threads = 100;
  etcpal::Queue<long long int> q(80000);

  create_writer_test(&q, num_threads);
  wait_for_writer_complete(num_threads);
  cleanup_writers();
  return !writer_failure;
}

bool concurrent_small_queue_one_reader_few_writers_few_items()
{
  bool occurrence_fail = false;

  writer_failure = false;
  reader_failure = false;

  // Each of 8 writers writes 8 elements
  // Meanwhile, Each of 1 readers reads 64 elements
  // Queue only has 8 elements

  num_writes = 8;
  num_reads = 64;
  int                          num_writers = 8;
  int                          num_readers = 1;
  etcpal::Queue<long long int> q(8);

  create_writer_test(&q, num_writers);
  create_reader_test(&q, num_readers);

  wait_for_writer_complete(num_writers);
  wait_for_reader_complete(num_readers);

  occurrence_fail = !check_occurrences(num_writers);
  reset_occurrence_counter();

  cleanup_writers();
  cleanup_readers();

  return !(writer_failure || reader_failure || occurrence_fail);
}


bool concurrent_small_queue_few_readers_few_writers_few_items()
{
  bool occurrence_fail = false;

  writer_failure = false;
  reader_failure = false;

  // Each of 8 writers writes 8 elements
  // Meanwhile, Each of 8 readers reads 8 elements
  // Queue only has 8 elements

  num_writes = 8;
  num_reads = 8;
  int                          num_writers = 8;
  int                          num_readers = 8;
  etcpal::Queue<long long int> q(8);

  create_writer_test(&q, num_writers);
  create_reader_test(&q, num_readers);

  wait_for_writer_complete(num_writers);
  wait_for_reader_complete(num_readers);

  occurrence_fail = !check_occurrences(num_writers);
  reset_occurrence_counter();

  cleanup_writers();
  cleanup_readers();

  return !(writer_failure || reader_failure || occurrence_fail);
}

bool concurrent_small_queue_few_readers_many_writers_many_items()
{
  bool occurrence_fail = false;

  writer_failure = false;
  reader_failure = false;

  // Each of 100 writers writes 800 elements
  // Meanwhile, Each of 8 readers reads 10000 elements
  // Queue only has 8 elements

  num_writes = 800;
  num_reads = 10000;
  int                          num_writers = 100;
  int                          num_readers = 8;
  etcpal::Queue<long long int> q(8);

  create_writer_test(&q, num_writers);
  create_reader_test(&q, num_readers);

  wait_for_writer_complete(num_writers);
  wait_for_reader_complete(num_readers);

  occurrence_fail = !check_occurrences(num_writers);
  reset_occurrence_counter();

  cleanup_writers();
  cleanup_readers();

  return !(writer_failure || reader_failure || occurrence_fail);
}


bool concurrent_small_queue_many_readers_few_writers_many_items()
{
  bool occurrence_fail = false;

  writer_failure = false;
  reader_failure = false;

  // Each of 8 writers writes 10000 elements
  // Meanwhile, Each of 100 readers reads 800 elements
  // Queue only has 8 elements

  num_writes = 10000;
  num_reads = 800;
  int                          num_writers = 8;
  int                          num_readers = 100;
  etcpal::Queue<long long int> q(8);

  create_writer_test(&q, num_writers);
  create_reader_test(&q, num_readers);

  wait_for_writer_complete(num_writers);
  wait_for_reader_complete(num_readers);

  occurrence_fail = !check_occurrences(num_writers);
  reset_occurrence_counter();

  cleanup_writers();
  cleanup_readers();

  return !(writer_failure || reader_failure || occurrence_fail);
}
