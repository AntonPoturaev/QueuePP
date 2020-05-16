//
// Created by antonp on 16.08.17.
//

#include <iostream>
#include <memory>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <deque>
#include <functional>
#include <cassert>
#include <unordered_set>
#include <algorithm>
#include <vector>

namespace Comodo { namespace Concurrency {

  /// @brief public interface
  /// @{
  using tTask = std::function<void()>;

  struct IQueue {
    virtual ~IQueue() = default;

    virtual void Start() = 0;
    virtual bool IsStarted() const noexcept = 0;
    virtual void Stop() = 0;

    virtual void Push(tTask const&) = 0;
    virtual void Push(tTask&&) = 0;

    virtual std::thread::id GetWorkThreadId() const noexcept = 0;
    virtual std::string const& GetName() const noexcept = 0;
  };

  using tIQueuePtr = std::shared_ptr<IQueue>;

  struct IExecutePolicy {
    virtual ~IExecutePolicy() = default;

    virtual void Execute(tTask&& task) = 0;
  };

  using tIExecutePolicyPtr = std::unique_ptr<IExecutePolicy>;

  struct DefaultExecutePolicy final
    : public IExecutePolicy
  {
    virtual void Execute(tTask&& task) override;
  };

  struct Queues {
    using tQueueNamesStore = std::vector<std::string>;

   public:
    Queues() = delete;

    static std::string const& GetMainThreadName();

    /// @brief factory methods (creators)
    static tIQueuePtr Create(std::string const& name, tIExecutePolicyPtr&& execPolicy = std::make_unique<DefaultExecutePolicy>());
    static tIQueuePtr CreateMain(tIExecutePolicyPtr&& execPolicy = std::make_unique<DefaultExecutePolicy>());

    /// @brief simple assessors
    static tIQueuePtr CreateOrGet(std::string const& name, tIExecutePolicyPtr&& execPolicy = std::make_unique<DefaultExecutePolicy>());
    static tIQueuePtr CreateOrGetMain(tIExecutePolicyPtr&& execPolicy = std::make_unique<DefaultExecutePolicy>());

    /// @brief Getters
    static tIQueuePtr Get(std::string const& name);
    static tIQueuePtr GetMain();

    /// @brief Enumerator
    static tQueueNamesStore GetAllNames();
    static bool IsExists(std::string const& name);

    /// @brief destroyers (remove from internal store...)
    static void Forget(std::string const& name);
    static void Forget(tIQueuePtr const& instance);
  };

  class SequenceHolder final {
    using tIQueueWeak = std::weak_ptr<IQueue>;
    using tStore = std::deque<tIQueueWeak>;
   public:
    SequenceHolder() = default;
    ~SequenceHolder() {
      Clear();
    }

    void Add(tIQueuePtr const& queue) {
      _store.emplace_back(queue);
    }

    void Clear() {
      if (!_store.empty()) {
        for (auto const& queueWeak : _store) {
          if (auto queue = queueWeak.lock()) {
            Queues::Forget(queue);
            /// call queue->Stop() at dtor of queue ptr object
          }
        }

        _store.clear();
      }
    }

   private:
    tStore _store;
  };
  /// @}

  /// @brife implementation details
  /// @{
  void DefaultExecutePolicy::Execute(tTask&& task) {
    try {
      if (task) {
        task();
      }
    } catch (...) {
      /// TODO: add trace here if needed
    }
  }

  struct ControlPolicy {

    virtual ~ControlPolicy() = default;

    virtual void Start(tTask&& routine) = 0;
    virtual void Stop() = 0;
  };

  typedef std::unique_ptr<ControlPolicy> tControlPolicyPtr;

  struct MainThreadControlPolicy final
    : public ControlPolicy
  {
    virtual void Start(tTask&& routine) override;
    virtual void Stop() override;
  };

  void MainThreadControlPolicy::Start(tTask&& routine) {
    assert(routine && "Bad data!");
    if (routine) {
      routine();
    }
  }

  void MainThreadControlPolicy::Stop() {}

  class BackgroundThreadControlPolicy final
    : public ControlPolicy
  {
   public:
    virtual ~BackgroundThreadControlPolicy();

    virtual void Start(tTask&& routine) override;
    virtual void Stop() override;

   private:
    void _Stop();

   private:
    std::thread _worker;
  };

  BackgroundThreadControlPolicy::~BackgroundThreadControlPolicy() {
    _Stop();
  }

  void BackgroundThreadControlPolicy::Start(tTask&& routine) {
    assert(!_worker.joinable() && "Bad logic!");
    assert(routine && "Bad data!");
    if (routine) {
      _worker = std::thread(std::forward<tTask>(routine));
    }
  }

  void BackgroundThreadControlPolicy::Stop() {
    _Stop();
  }

  void BackgroundThreadControlPolicy::_Stop() {
    if (_worker.joinable()) {
      _worker.join();
    }

    _worker = std::thread();
  }

  class _Queue
    : public IQueue
  {
   public:
    _Queue(std::string const& name)
      : _name(name)
    {
    }

    virtual std::string const& GetName() const noexcept override final;

   protected:
    std::string const _name;
  };

  std::string const& _Queue::GetName() const noexcept {
    return _name;
  }

  class Queue final
    : public _Queue
  {
    friend class Queues;

    using tAccess = std::mutex;
    using tLock = std::unique_lock<tAccess>;
    using tTaskStore = std::deque<tTask>;

   public:
    virtual ~Queue() override;
    Queue(std::string const& name, tControlPolicyPtr&& ctrlPolicy, tIExecutePolicyPtr&& execPolicy);

    virtual void Start() override;
    virtual bool IsStarted() const noexcept override;
    virtual void Stop() override;

    virtual void Push(tTask const&) override;
    virtual void Push(tTask&& task) override;

    virtual std::thread::id GetWorkThreadId() const noexcept override;

   private:
    void _Routine();
    bool _WaitForNext();
    tTask _Next();
    void _Execute(tTask&& task);
    void _Stop();
    void _DoStop();

   private:
    tControlPolicyPtr _ctrl;
    tIExecutePolicyPtr _exec;
    std::atomic_bool _stop;
    std::atomic_bool mutable _started;
    tTaskStore _store;
    tAccess mutable _access;
    std::condition_variable _cond;
    std::thread::id _id;
  };

  namespace {
    class _ScopedBool final
    {
     public:
      _ScopedBool(std::atomic_bool& value) noexcept
        : _value(value)
      {
        _Set(true);
      }

      ~_ScopedBool() noexcept {
        _Set(false);
      }

     private:
      void _Set(bool value) noexcept {
        _value.store(value, std::memory_order_relaxed);
      }

     private:
      std::atomic_bool& _value;
    };
  } /// end unnamed namespace

  Queue::~Queue() {
    _Stop();
#if 1
    std::cout << "Queue [" << _name << "] is destroyed." << std::endl;
#endif
  }

  Queue::Queue(std::string const& name, tControlPolicyPtr&& ctrlPolicy, tIExecutePolicyPtr&& execPolicy)
    : _Queue(name)
    , _ctrl(std::forward<tControlPolicyPtr>(ctrlPolicy))
    , _exec(std::forward<tIExecutePolicyPtr>(execPolicy))
    , _stop(false)
    , _started(false)
  {
    assert(_ctrl && _exec && "Bad data!");
#if 1
    std::cout << "Queue [" << _name << "] is created." << std::endl;
#endif
  }

  void Queue::Start() {
    assert(_ctrl && "Bad data!");
    _ctrl->Start([this](){ _Routine(); });
  }

  bool Queue::IsStarted() const noexcept {
    return _started;
  }

  void Queue::Stop() {
    _Stop();
  }

  void Queue::Push(tTask const& task) {
    tLock const lock(_access);
    _store.push_back(task);
    _cond.notify_one();
  }

  void Queue::Push(tTask&& task) {
    tLock const lock(_access);
    _store.emplace_back(std::forward<tTask>(task));
    _cond.notify_one();
  }

  std::thread::id Queue::GetWorkThreadId() const noexcept {
    assert(IsStarted() && "Bad logic!");
    return _id;
  }

  void Queue::_Routine() {
    _id = std::this_thread::get_id();
    _ScopedBool const startScope(_started);

    while (!_stop) {
      if (_WaitForNext()) {
        _Execute(_Next());
      } else {
        return;
      }
    }
  }

  bool Queue::_WaitForNext() {
    tLock lock(_access);
    while (_store.empty()) {
      if (_stop) {
        return false;
      }

      _cond.wait(lock);
    }

    return true;
  }

  tTask Queue::_Next() {
    //tLock const lock(_access);
    auto task = std::move(_store.front());
    _store.pop_front();
    return task;
  }

  void Queue::_Execute(tTask&& task) {
    assert(_exec && task && "Bad data!");
    if (_exec) {
      _exec->Execute(std::forward<tTask>(task));
    }
  }

  void Queue::_Stop() {
    if (!_stop) {
      _DoStop();

      assert(_ctrl && "Bad data!");
      _ctrl->Stop();
    }
  }

  void Queue::_DoStop() {
    tLock const lock(_access);

    _store.clear();
    _stop = true;
    _cond.notify_one();
  }

  namespace {
    class _FakeQueue final
      : public _Queue
    {
     public:
      _FakeQueue(std::string const& name)
        : _Queue(name)
      {
      }

     private:
      virtual void Start() override {}
      virtual bool IsStarted() const noexcept override { return {}; }
      virtual void Stop() override {}
      virtual void Push(tTask const& task) override {}
      virtual void Push(tTask&& task) override {}
      virtual std::thread::id GetWorkThreadId() const noexcept override { return {}; }
    };

    struct _Hasher final
      : public std::hash<tIQueuePtr>
    {
      std::size_t operator()(tIQueuePtr const& q) const noexcept{
        assert(q && "Bad data!");
        static auto const hasher = std::hash<std::string>();
        return hasher(q->GetName());
      }
    };

    struct _Equal final {
      bool operator()(tIQueuePtr const& l, tIQueuePtr const& r) const {
        assert(l && r && "Bad data!");
        return l->GetName() == r->GetName();
      }
    };

    class _QueuesStore final {
      using tStore = std::unordered_set<tIQueuePtr, _Hasher, _Equal>;
      using tAccess = std::mutex;
      using tLock = std::lock_guard<tAccess> const;

     public:
      _QueuesStore() = default;

      ~_QueuesStore() {
        tLock lock(_access);
        _store.clear();
      }

      static _QueuesStore& Instance() {
        static _QueuesStore instance;
        return instance;
      }

      tIQueuePtr Create(std::string const& name, tIExecutePolicyPtr&& execPolicy) {
        tLock lock(_access);
        return _CreateAndGet(name, std::make_unique<BackgroundThreadControlPolicy>(), std::forward<tIExecutePolicyPtr>(execPolicy));
      }

      tIQueuePtr CreateMain(tIExecutePolicyPtr&& execPolicy) {
        tLock lock(_access);
        return _CreateAndGet(Queues::GetMainThreadName()
          , std::make_unique<MainThreadControlPolicy>(), std::forward<tIExecutePolicyPtr>(execPolicy));
      }

      tIQueuePtr CreateOrGet(std::string const& name, tIExecutePolicyPtr&& execPolicy) {
        tLock lock(_access);
        tIQueuePtr queue = _GetByName(name);
        return queue ? queue : _CreateAndGet(name
          , std::make_unique<BackgroundThreadControlPolicy>(), std::forward<tIExecutePolicyPtr>(execPolicy));
      }

      tIQueuePtr CreateOrGetMain(tIExecutePolicyPtr&& execPolicy) {
        tLock lock(_access);

        tIQueuePtr queue = _GetByName(Queues::GetMainThreadName());
        return queue ? queue : _CreateAndGet(Queues::GetMainThreadName()
          , std::make_unique<MainThreadControlPolicy>(), std::forward<tIExecutePolicyPtr>(execPolicy));
      }

      tIQueuePtr Get(std::string const& name) {
        tLock lock(_access);
        return _GetByName(name);
      }

      tIQueuePtr GetMain() {
        tLock lock(_access);
        return _GetByName(Queues::GetMainThreadName());
      }

      Queues::tQueueNamesStore GetAllNames() const {
        tLock lock(_access);

        Queues::tQueueNamesStore store;
        store.reserve(_store.size());

        for (auto const& queue : _store) {
          assert(queue && "Bad data!");
#if 0
          store.push_back(queue->GetName());
#else
          if (queue) {
            if (queue->GetName() == Queues::GetMainThreadName()) {
              store.push_back(queue->GetName());
            } else {
              store.insert(std::prev(store.end()), queue->GetName());
            }
          }
#endif
        }

        return store;
      }

      bool IsExists(std::string const& name) const {
        tLock lock(_access);
        return _store.find(std::make_shared<_FakeQueue>(name)) != _store.end();
      }

      void Forget(tIQueuePtr const& queue) {
        tLock lock(_access);
        assert(queue && "Bad data!");
        _store.erase(queue);
      }

      void Forget(std::string const& name) {
        tLock lock(_access);
        _store.erase(std::make_shared<_FakeQueue>(name));
      }

     private:
      tIQueuePtr _GetByName(std::string const& name) {
        auto const found = _store.find(std::make_shared<_FakeQueue>(name));
        return found != _store.end() ? *found : nullptr;
      }

      tIQueuePtr _CreateAndGet(std::string const& name, tControlPolicyPtr&& ctrlPolicy, tIExecutePolicyPtr&& execPolicy) {
        tIQueuePtr queue = std::make_shared<Queue>(name
          , std::forward<tControlPolicyPtr>(ctrlPolicy), std::forward<tIExecutePolicyPtr>(execPolicy));

        if (!_Add(queue)) {
          queue.reset();
          /// TODO: add trace here if needed
        }

        return queue;
      }

      bool _Add(tIQueuePtr const& queue) {
        assert(queue && "Bad data!");
        return _store.insert(queue).second;
      }

     private:
      tStore _store;
      tAccess mutable _access;
    };

  } /// end unnamed namespace

  std::string const& Queues::GetMainThreadName() {
    static std::string const name("___MAIN_THREAD_QUEUE___");
    return name;
  }

  tIQueuePtr Queues::Create(std::string const& name, tIExecutePolicyPtr&& execPolicy) {
    return _QueuesStore::Instance().Create(name, std::forward<tIExecutePolicyPtr>(execPolicy));
  }

  tIQueuePtr Queues::CreateMain(tIExecutePolicyPtr&& execPolicy) {
    return _QueuesStore::Instance().CreateMain(std::forward<tIExecutePolicyPtr>(execPolicy));
  }


  tIQueuePtr Queues::CreateOrGet(std::string const& name, tIExecutePolicyPtr&& execPolicy) {
    return _QueuesStore::Instance().CreateOrGet(name, std::forward<tIExecutePolicyPtr>(execPolicy));
  }

  tIQueuePtr Queues::CreateOrGetMain(tIExecutePolicyPtr&& execPolicy) {
    return _QueuesStore::Instance().CreateOrGetMain(std::forward<tIExecutePolicyPtr>(execPolicy));
  }

  tIQueuePtr Queues::Get(std::string const& name) {
    return _QueuesStore::Instance().Get(name);
  }

  tIQueuePtr Queues::GetMain() {
    return _QueuesStore::Instance().GetMain();
  }

  Queues::tQueueNamesStore Queues::GetAllNames() {
    return _QueuesStore::Instance().GetAllNames();
  }

  bool Queues::IsExists(std::string const& name) {
    return _QueuesStore::Instance().IsExists(name);
  }

  void Queues::Forget(std::string const& name) {
    _QueuesStore::Instance().Forget(name);
  }

  void Queues::Forget(tIQueuePtr const& instance) {
    _QueuesStore::Instance().Forget(instance);
  }

  /// @}

}} /// end namespace Comodo::Concurrency

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>

namespace {
  namespace CC = ::Comodo::Concurrency;
  using namespace std::chrono_literals;

  void _PrintAllQueuesNames() {
    std::cout << "-------------------" << std::endl;
    for (auto const& name : CC::Queues::GetAllNames()) {
      std::cout << "Queue: [" << name << ']' << std::endl;
    }
    std::cout << "-------------------" << std::endl;
  }

  void _CtrlCHandler(int) {
    std::cout << "\nCtrl-C handler..." << std::endl;

    for (auto const& name : CC::Queues::GetAllNames()) {
      std::cout << "Stopping of queue: [" << name << ']' << std::endl;
      auto queue = CC::Queues::Get(name);
      assert(queue && "Bad data!");
      CC::Queues::Forget(queue);
      //queue->Stop();
      queue.reset();
    }

    std::cout << "Exit" << std::endl;
    //::exit(EXIT_SUCCESS);
  }
} /// end unnamed namespace

int main() {
  std::cout << "Hello from Queues draft test application." << std::endl;

  struct sigaction sigIntHandler = { 0 };
  sigIntHandler.sa_handler = _CtrlCHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  signal(SIGINT, _CtrlCHandler);

/// @brief uncomment next line for run test under debuger
//#define WITH_AUTO_STOP

#if defined(WITH_AUTO_STOP)
  static constexpr auto const g_stopTimeout(5s);
#endif /// WITH_AUTO_STOP


#define TEST_CASE 2

#if TEST_CASE == 1 /// @brief Test-case for recursive task

  std::cout << "Test-case for recursive task." << std::endl;

  static std::string const g_toMainSenderQueue("toMainSenderQueue");

  struct _RecursiveTask
    : public std::enable_shared_from_this<_RecursiveTask>
  {
    ~_RecursiveTask() {
      std::cout << "_RecursiveTask is destroyed." << std::endl;
    }

    void Run() {
      CC::Queues::CreateOrGetMain()->Push([]() {
        std::cout << "[HB] main thread." << std::endl;
      });

      std::this_thread::sleep_for(0.5s);

      CC::Queues::CreateOrGet(g_toMainSenderQueue)->Push(std::bind(&_RecursiveTask::Run, shared_from_this()));
    }
  };

  auto toMainSenderQueue = CC::Queues::CreateOrGet(g_toMainSenderQueue);
  //g_QueuesRegistry.push_back(toMainSenderQueue);

  toMainSenderQueue->Push(std::bind(&_RecursiveTask::Run, std::make_shared<_RecursiveTask>()));

  auto mainQueue = CC::Queues::CreateOrGetMain();
  //g_QueuesRegistry.push_back(mainQueue);

  mainQueue->Push([](){
    assert(!CC::Queues::CreateOrGet(g_toMainSenderQueue)->IsStarted());
    CC::Queues::CreateOrGet(g_toMainSenderQueue)->Start();
  });

#if defined(WITH_AUTO_STOP)
  std::thread([](){
    std::this_thread::sleep_for(g_stopTimeout);
    std::cout << "Send the kill signal for self..." << std::endl;
    kill (getpid(), SIGINT);
  }).detach();
#endif /// WITH_AUTO_STOP

  mainQueue->Start();

#elif TEST_CASE == 2 /// @brief Test-case for composite task

  std::cout << "Test-case for composite task." << std::endl;

  static std::string const g_compositeTaskQueue("compositeTaskQueue");
  static std::string const g_taskQueue("taskQueue");

  CC::Queues::Create(g_compositeTaskQueue)->Start();
  CC::Queues::Create(g_taskQueue)->Start();
  std::this_thread::sleep_for(0.1s);

  auto const compositeTask = []() {
    if (!CC::Queues::Get(g_compositeTaskQueue)->IsStarted()) {
      std::cerr << "Bad logic: [compositeTaskQueue] is not started!\n";
      return;
    }

    if (auto compositeQueue = CC::Queues::Get(g_compositeTaskQueue)) {
      compositeQueue->Push([](){
        std::cout << "--> CompositeTask" << std::endl;

        auto const task = []() {
          if (!CC::Queues::Get(g_taskQueue)->IsStarted()) {
            std::cerr << "Bad logic: [taskQueue] is not started!\n";
            return;
          }

          std::promise<void> promise;
          std::future<void> future = promise.get_future();

          auto const shortAsyncProc = [&promise]() {
            std::this_thread::sleep_for(0.5s);
            std::cout << "---- ShortTask ----" << std::endl;
            promise.set_value();
          };

          if (auto taskQueue = CC::Queues::Get(g_taskQueue)) {
            taskQueue->Push(shortAsyncProc);
          } else {
            std::cerr << "Queue [" << g_taskQueue << "] is wrong.\n";
          }

          future.get();
        };

        static constexpr size_t const callsCount(4);
        for (size_t i = 0; i < callsCount; ++i) {
          task();
        }

        std::cout << "<-- CompositeTask" << std::endl;
      });
    } else {
      std::cerr << "Queue [" << g_compositeTaskQueue << "] is wrong.\n";
    }
  };

  auto mainQueue = CC::Queues::CreateMain();

  mainQueue->Push([&compositeTask, mainQueue]() {
    mainQueue->Push(compositeTask);
    mainQueue->Push(compositeTask);
    mainQueue->Push(compositeTask);
#if !defined(WITH_AUTO_STOP)
    mainQueue->Push([](){ _PrintAllQueuesNames(); });
#if 0
    mainQueue->Push([](){ _CtrlCHandler(0); });
#else
    mainQueue->Push([](){
      auto queue = CC::Queues::Get(g_compositeTaskQueue);
      CC::Queues::Forget(queue);
      queue->Stop();
      queue = CC::Queues::Get(g_taskQueue);
      CC::Queues::Forget(queue);
      queue->Stop();
      queue = CC::Queues::GetMain();
      CC::Queues::Forget(queue);
      queue->Stop();
    });
#endif
#endif /// !WITH_AUTO_STOP
    /*mainQueue->Push(compositeTask);
    mainQueue->Push(compositeTask);
    mainQueue->Push(compositeTask);*/
  });

#if defined(WITH_AUTO_STOP)

  std::thread([](){
    std::this_thread::sleep_for(g_stopTimeout);
    std::cout << "Send the kill signal for self..." << std::endl;
    kill (getpid(), SIGINT);
  }).detach();

#endif /// #if defined(WITH_AUTO_STOP)

  mainQueue->Start();

#elif TEST_CASE == 3 ///

  auto main = CC::Queues::CreateMain();
  auto ghost = CC::Queues::Create("ghost");

  ghost->Push([main]() {
    std::this_thread::sleep_for(1s);
    main->Stop();
  });

  main->Push([]() {
    std::this_thread::sleep_for(5s);
  });

  ghost->Start();
  main->Start();

  _PrintAllQueuesNames();
  _CtrlCHandler(0);

#else
# error Unknown id of test case
#endif /// #if TEST_CASE

  std::cout << "End of main." << std::endl;
  return 0;
}
