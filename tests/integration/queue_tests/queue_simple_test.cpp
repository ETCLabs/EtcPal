#include "queue_tests.h"

bool create_queue()
{
  etcpal::Queue<int> q(1);
  return true;
}

bool simple_send_receive()
{
  etcpal::Queue<int> q(1);
  q.Send(420);

  int val = -1;
  q.Receive(val);

  if (val == 420)
  {
    return true;
  }
  return false;
}

bool simple_fill_and_empty()
{
  static const unsigned        queue_size  = 128;
  static const unsigned        large_prime = 115249;
  etcpal::Queue<long long int> q(queue_size);

  for (int i = 0; i < queue_size; i++)
  {
    if (!q.Send(115249 * i))
    {
      return false;
    }
  }

  for (int i = 0; i < queue_size; i++)
  {
    long long int val;
    if (!q.Receive(val))
    {
      if (val / large_prime != i)
      {
        return false;
      }
    }
  }

  return true;
}
