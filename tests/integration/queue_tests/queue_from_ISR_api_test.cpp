#include "queue_tests.h"

bool send_to_empty_from_isr()
{
  bool                         true_if_success = false;
  long long int                val = 0;

  etcpal::Queue<long long int> q(3);

  q.SendFromIsr(420);
  q.Receive(val);

  true_if_success = (val == 420);

  return true_if_success;
}

bool send_to_full_from_isr()
{
  bool true_if_success = false;

  etcpal::Queue<long long int> q(3);

  q.Send(4);
  q.Send(20);
  q.Send(1996);

  // Should fail
  true_if_success = !q.SendFromIsr(6969);

  return true_if_success;
}

bool receive_from_empty_from_isr()
{
  long long int                val = 0;
  bool                         true_if_success = false;
  etcpal::Queue<long long int> q(3);

  // Receive should fail
  true_if_success = !q.ReceiveFromIsr(val);

  return true_if_success;
}

bool receive_from_full_from_isr()
{
  int val = 0;
  bool          true_if_success = false;

  etcpal::Queue<int> q(3);

  q.Send(4);
  q.Send(20);
  q.Send(1996);

  q.ReceiveFromIsr(val);

  true_if_success = (val == 4);

  return true_if_success;
}
