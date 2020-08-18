/******************************************************************************
 * Copyright 2020 ETC Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

#include "etcpal/cpp/lock.h"
#include "unity_fixture.h"

extern "C" {
TEST_GROUP(etcpal_cpp_lock);

TEST_SETUP(etcpal_cpp_lock)
{
}

TEST_TEAR_DOWN(etcpal_cpp_lock)
{
}

TEST(etcpal_cpp_lock, mutex_create_and_destroy_works)
{
  etcpal::Mutex mutex;

  // Take ownership
  TEST_ASSERT_TRUE(mutex.Lock());

  TEST_ASSERT_FALSE(mutex.TryLock());

  mutex.Unlock();
}

TEST(etcpal_cpp_lock, mutex_guard_works)
{
  etcpal::Mutex mutex;

  {
    // Take ownership via a mutex guard
    etcpal::MutexGuard guard(mutex);

    TEST_ASSERT_FALSE(mutex.TryLock());
  }

  // Lock should now be unlocked
  TEST_ASSERT_TRUE(mutex.TryLock());
  mutex.Unlock();
}

TEST(etcpal_cpp_lock, signal_create_and_destroy_works)
{
  etcpal::Signal sig;

  // Signals shouldn't be created in the signaled state.
  TEST_ASSERT_FALSE(sig.TryWait());

  sig.Notify();
  TEST_ASSERT_TRUE(sig.Wait());
}

TEST(etcpal_cpp_lock, rwlock_create_and_destroy_works)
{
  etcpal::RwLock rwlock;

  // We should be able to get a number of read locks
  for (size_t i = 0; i < 100; ++i)
    TEST_ASSERT_TRUE(rwlock.ReadLock());

  // Write lock should fail if there are readers
  TEST_ASSERT_FALSE(rwlock.TryWriteLock());

  // When there are no more readers, write lock should succeed
  for (size_t i = 0; i < 100; ++i)
    rwlock.ReadUnlock();
  TEST_ASSERT_TRUE(rwlock.WriteLock());

  TEST_ASSERT_FALSE(rwlock.TryWriteLock());
  TEST_ASSERT_FALSE(rwlock.TryReadLock());

  rwlock.WriteUnlock();
}

TEST(etcpal_cpp_lock, read_guard_works)
{
  etcpal::RwLock rwlock;

  {
    // Take a read lock via guard
    etcpal::ReadGuard read(rwlock);

    // Another read lock should be OK
    TEST_ASSERT_TRUE(rwlock.TryReadLock());
    rwlock.ReadUnlock();

    // But a write lock should not
    TEST_ASSERT_FALSE(rwlock.TryWriteLock());
  }

  // Now the read lock should be released
  TEST_ASSERT_TRUE(rwlock.TryWriteLock());
  rwlock.WriteUnlock();
}

TEST(etcpal_cpp_lock, write_guard_works)
{
  etcpal::RwLock rwlock;

  {
    // Take a write lock via guard
    etcpal::WriteGuard write(rwlock);

    TEST_ASSERT_FALSE(rwlock.TryReadLock());
    TEST_ASSERT_FALSE(rwlock.TryWriteLock());
  }

  // Lock should now be unlocked
  TEST_ASSERT_TRUE(rwlock.TryWriteLock());
  rwlock.WriteUnlock();
}

TEST(etcpal_cpp_lock, sem_create_and_destroy_works)
{
  // The semaphore has an initial count of 1
  etcpal::Semaphore sem(1);

  TEST_ASSERT_TRUE(sem.Wait());

  // The count should now be zero - wait should fail
  TEST_ASSERT_FALSE(sem.TryWait());

  TEST_ASSERT_TRUE(sem.Post());
  // The count should be 1 again. Wait should succeed.
  TEST_ASSERT_TRUE(sem.TryWait());

  TEST_ASSERT_TRUE(sem.Post());
}

TEST_GROUP_RUNNER(etcpal_cpp_lock)
{
  RUN_TEST_CASE(etcpal_cpp_lock, mutex_create_and_destroy_works);
  RUN_TEST_CASE(etcpal_cpp_lock, mutex_guard_works);
  RUN_TEST_CASE(etcpal_cpp_lock, signal_create_and_destroy_works);
  RUN_TEST_CASE(etcpal_cpp_lock, rwlock_create_and_destroy_works);
  RUN_TEST_CASE(etcpal_cpp_lock, read_guard_works);
  RUN_TEST_CASE(etcpal_cpp_lock, write_guard_works);
  RUN_TEST_CASE(etcpal_cpp_lock, sem_create_and_destroy_works);
}
}
