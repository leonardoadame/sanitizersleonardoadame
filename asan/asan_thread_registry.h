//===-- asan_thread_registry.h ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of AddressSanitizer, an address sanity checker.
//
// ASan-private header for asan_thread_registry.cc
//===----------------------------------------------------------------------===//

#ifndef ASAN_THREAD_REGISTRY_H
#define ASAN_THREAD_REGISTRY_H

#include "asan_lock.h"
#include "asan_stack.h"
#include "asan_thread.h"

namespace __asan {

// Stores summaries of all created threads, returns current thread,
// thread by tid, thread by stack address. There is a single instance
// of AsanThreadRegistry for the whole program.
// AsanThreadRegistry is thread-safe.
class AsanThreadRegistry {
 public:
  explicit AsanThreadRegistry(LinkerInitialized);
  void Init();
  void RegisterThread(AsanThread *thread, int parent_tid,
                      AsanStackTrace *stack);
  void UnregisterThread(AsanThread *thread);

  AsanThread *GetMain();
  // Get the current thread. May return NULL.
  AsanThread *GetCurrent();
  void SetCurrent(AsanThread *t);

  AsanThreadSummary *FindByTid(int tid);
  AsanThread *FindThreadByStackAddress(uintptr_t addr);

 private:
  static const int kMaxNumberOfThreads = (1 << 22);  // 4M
  AsanThreadSummary *thread_summaries_[kMaxNumberOfThreads];
  AsanThread main_thread_;
  AsanThreadSummary main_thread_summary_;
  int n_threads_;
  AsanLock mu_;
  // For each thread tls_key_ stores the pointer to the corresponding
  // AsanThread.
  pthread_key_t tls_key_;
  // This flag is updated only once at program startup, and then read
  // by concurrent threads.
  bool tls_key_created_;
};

// Returns a single instance of registry.
AsanThreadRegistry &asanThreadRegistry();

}  // namespace __asan

#endif  // ASAN_THREAD_REGISTRY_H