#pragma once

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <cstddef>
#include <limits>

namespace moodycamel {

struct ConcurrentQueueDefaultTraits {
  typedef std::size_t size_t;
  typedef std::size_t index_t;
  static const size_t BLOCK_SIZE                                               = 32;
  static const size_t EXPLICIT_BLOCK_EMPTY_COUNTER_THRESHOLD                   = 32;
  static const size_t EXPLICIT_INITIAL_INDEX_SIZE                              = 32;
  static const size_t IMPLICIT_INITIAL_INDEX_SIZE                              = 32;
  static const size_t INITIAL_IMPLICIT_PRODUCER_HASH_SIZE                      = 32;
  static const std::uint32_t EXPLICIT_CONSUMER_CONSUMPTION_QUOTA_BEFORE_ROTATE = 256;
  static const size_t MAX_SUBQUEUE_SIZE                                        = SIZE_MAX;
  static inline void *malloc(size_t size) { return std::malloc(size); }
  static inline void free(void *ptr) { return std::free(ptr); }
};
struct ProducerToken;
struct ConsumerToken;
namespace details {
struct ConcurrentQueueProducerTypelessBase {
  ConcurrentQueueProducerTypelessBase *next;
  std::atomic<bool> inactive;
  ProducerToken *token;

  ConcurrentQueueProducerTypelessBase();
};
struct ThreadExitListener {
  typedef void (*callback_t)(void *);
  callback_t callback;
  void *userData;
  ThreadExitListener *next;
};
typedef std::uint32_t thread_id_t;
static const thread_id_t invalid_thread_id  = 0;
static const thread_id_t invalid_thread_id2 = 0xFFFFFFFFU;
static inline thread_id_t thread_id();

} // namespace details
template <typename T, typename Traits = ConcurrentQueueDefaultTraits> class ConcurrentQueue {
  typedef ::moodycamel::ProducerToken producer_token_t;
  typedef ::moodycamel::ConsumerToken consumer_token_t;
  typedef typename Traits::index_t index_t;
  typedef typename Traits::size_t size_t;
  static const size_t BLOCK_SIZE                                               = static_cast<size_t>(Traits::BLOCK_SIZE);
  static const size_t EXPLICIT_BLOCK_EMPTY_COUNTER_THRESHOLD                   = static_cast<size_t>(Traits::EXPLICIT_BLOCK_EMPTY_COUNTER_THRESHOLD);
  static const size_t EXPLICIT_INITIAL_INDEX_SIZE                              = static_cast<size_t>(Traits::EXPLICIT_INITIAL_INDEX_SIZE);
  static const size_t IMPLICIT_INITIAL_INDEX_SIZE                              = static_cast<size_t>(Traits::IMPLICIT_INITIAL_INDEX_SIZE);
  static const size_t INITIAL_IMPLICIT_PRODUCER_HASH_SIZE                      = static_cast<size_t>(Traits::INITIAL_IMPLICIT_PRODUCER_HASH_SIZE);
  static const std::uint32_t EXPLICIT_CONSUMER_CONSUMPTION_QUOTA_BEFORE_ROTATE = static_cast<std::uint32_t>(Traits::EXPLICIT_CONSUMER_CONSUMPTION_QUOTA_BEFORE_ROTATE);
  static const size_t MAX_SUBQUEUE_SIZE                                        = (std::numeric_limits<size_t>::max - static_cast<size_t>(Traits::MAX_SUBQUEUE_SIZE) < BLOCK_SIZE) ?
                                              std::numeric_limits<size_t>::max :
                                              ((static_cast<size_t>(Traits::MAX_SUBQUEUE_SIZE) + (BLOCK_SIZE - 1)) / BLOCK_SIZE * BLOCK_SIZE);
  explicit ConcurrentQueue(size_t capacity = 6 * BLOCK_SIZE);
  ConcurrentQueue(size_t minCapacity, size_t maxExplicitProducers, size_t maxImplicitProducers);
  ~ConcurrentQueue();
  ConcurrentQueue(ConcurrentQueue const &) = delete;
  ConcurrentQueue &operator=(ConcurrentQueue const &) = delete;
  ConcurrentQueue(ConcurrentQueue &&other) noexcept;
  inline ConcurrentQueue &operator=(ConcurrentQueue &&other) noexcept;
  inline void swap(ConcurrentQueue &other) noexcept;
  ConcurrentQueue &swap_internal(ConcurrentQueue &other);
  inline bool enqueue(T const &item);
  inline bool enqueue(T &&item);
  inline bool enqueue(producer_token_t const &token, T const &item);
  inline bool enqueue(producer_token_t const &token, T &&item);
  template <typename It> bool enqueue_bulk(It itemFirst, size_t count);
  template <typename It> bool enqueue_bulk(producer_token_t const &token, It itemFirst, size_t count);
  inline bool try_enqueue(T const &item);
  inline bool try_enqueue(T &&item);
  inline bool try_enqueue(producer_token_t const &token, T const &item);
  inline bool try_enqueue(producer_token_t const &token, T &&item);
  template <typename It> bool try_enqueue_bulk(It itemFirst, size_t count);
  template <typename It> bool try_enqueue_bulk(producer_token_t const &token, It itemFirst, size_t count);
  template <typename U> bool try_dequeue(U &item);
  template <typename U> bool try_dequeue_non_interleaved(U &item);
  template <typename U> bool try_dequeue(consumer_token_t &token, U &item);
  template <typename It> size_t try_dequeue_bulk(It itemFirst, size_t max);
  template <typename It> size_t try_dequeue_bulk(consumer_token_t &token, It itemFirst, size_t max);
  template <typename U> inline bool try_dequeue_from_producer(producer_token_t const &producer, U &item);
  template <typename It> inline size_t try_dequeue_bulk_from_producer(producer_token_t const &producer, It itemFirst, size_t max);
  size_t size_approx() const;
  static bool is_lock_free();
  enum AllocationMode { CanAlloc, CannotAlloc };
  template <AllocationMode canAlloc, typename U> inline bool inner_enqueue(producer_token_t const &token, U &&element);
  template <AllocationMode canAlloc, typename U> inline bool inner_enqueue(U &&element);
  template <AllocationMode canAlloc, typename It> inline bool inner_enqueue_bulk(producer_token_t const &token, It itemFirst, size_t count);
  template <AllocationMode canAlloc, typename It> inline bool inner_enqueue_bulk(It itemFirst, size_t count);
  inline bool update_current_producer_after_rotation(consumer_token_t &token);
  template <typename N> struct FreeListNode {
    FreeListNode();
    std::atomic<std::uint32_t> freeListRefs;
    std::atomic<N *> freeListNext;
  };
  template <typename N> struct FreeList {
    FreeList();
    FreeList(FreeList &&other);
    void swap(FreeList &other);
    FreeList(FreeList const &) noexcept;
    FreeList &operator=(FreeList const &) noexcept;
    inline void add(N *node);
    inline N *try_get();
    N *head_unsafe() const;
    inline void add_knowing_refcount_is_zero(N *node);
    std::atomic<N *> freeListHead;
    static const std::uint32_t REFS_MASK             = 0x7FFFFFFF;
    static const std::uint32_t SHOULD_BE_ON_FREELIST = 0x80000000;
  };
  enum InnerQueueContext { implicit_context = 0, explicit_context = 1 };
  struct Block {
    Block();
    template <InnerQueueContext context> inline bool is_empty() const;
    template <InnerQueueContext context> inline bool set_empty(index_t i);
    template <InnerQueueContext context> inline bool set_many_empty(index_t i, size_t count);
    template <InnerQueueContext context> inline void set_all_empty();
    template <InnerQueueContext context> inline void reset_empty();
    inline T *operator[](index_t idx) noexcept;
    inline T const *operator[](index_t idx) const;
    union {
      char elements[sizeof(T) * BLOCK_SIZE];
      std::max_align_t dummy;
    };
    Block *next;
    std::atomic<size_t> elementsCompletelyDequeued;
    std::atomic<bool> emptyFlags[BLOCK_SIZE];
    std::atomic<std::uint32_t> freeListRefs;
    std::atomic<Block *> freeListNext;
    std::atomic<bool> shouldBeOnFreeList;
    bool dynamicallyAllocated;
  };
  struct MemStats;
  struct ProducerBase : details::ConcurrentQueueProducerTypelessBase {
    ProducerBase(ConcurrentQueue *parent, bool isExplicit);
    virtual ~ProducerBase();
    template <typename U> inline bool dequeue(U &element);
    template <typename It> inline size_t dequeue_bulk(It &itemFirst, size_t max);
    inline ProducerBase *next_prod() const;
    inline size_t size_approx() const;
    inline index_t getTail() const;
    std::atomic<index_t> tailIndex;
    std::atomic<index_t> headIndex;
    std::atomic<index_t> dequeueOptimisticCount;
    std::atomic<index_t> dequeueOvercommit;
    Block *tailBlock;
    bool isExplicit;
    ConcurrentQueue *parent;
  };
  struct ExplicitProducer : public ProducerBase {
    explicit ExplicitProducer(ConcurrentQueue *parent);
    ~ExplicitProducer();
    template <AllocationMode allocMode, typename U> inline bool enqueue(U &&element);
    template <typename U> bool dequeue(U &element);
    template <AllocationMode allocMode, typename It> bool enqueue_bulk(It itemFirst, size_t count);
    template <typename It> size_t dequeue_bulk(It &itemFirst, size_t max);
    struct BlockIndexEntry {
      index_t base;
      Block *block;
    };
    struct BlockIndexHeader {
      size_t size;
      std::atomic<size_t> front; // Current slot (not next, like pr_blockIndexFront)
      BlockIndexEntry *entries;
      void *prev;
    };
    bool new_block_index(size_t numberOfFilledSlotsToExpose);
    std::atomic<BlockIndexHeader *> blockIndex;
    size_t pr_blockIndexSlotsUsed;
    size_t pr_blockIndexSize;
    size_t pr_blockIndexFront;
    BlockIndexEntry *pr_blockIndexEntries;
    void *pr_blockIndexRaw;
  };
  struct ImplicitProducer : public ProducerBase {
    ImplicitProducer(ConcurrentQueue *parent);
    ~ImplicitProducer();
    template <AllocationMode allocMode, typename U> inline bool enqueue(U &&element);
    template <typename U> bool dequeue(U &element);
    template <AllocationMode allocMode, typename It> bool enqueue_bulk(It itemFirst, size_t count);
    template <typename It> size_t dequeue_bulk(It &itemFirst, size_t max);
    static const index_t INVALID_BLOCK_BASE = 1;
    struct BlockIndexEntry {
      std::atomic<index_t> key;
      std::atomic<Block *> value;
    };

    struct BlockIndexHeader {
      size_t capacity;
      std::atomic<size_t> tail;
      BlockIndexEntry *entries;
      BlockIndexEntry **index;
      BlockIndexHeader *prev;
    };
    template <AllocationMode allocMode> inline bool insert_block_index_entry(BlockIndexEntry *&idxEntry, index_t blockStartIndex);
    inline void rewind_block_index_tail();
    inline BlockIndexEntry *get_block_index_entry_for_index(index_t index) const;
    inline size_t get_block_index_index_for_index(index_t index, BlockIndexHeader *&localBlockIndex) const;
    bool new_block_index();
    size_t nextBlockIndexCapacity;
    std::atomic<BlockIndexHeader *> blockIndex;
    details::ThreadExitListener threadExitListener;
  };
  void populate_initial_block_list(size_t blockCount);
  inline Block *try_get_block_from_initial_pool();
  inline void add_block_to_free_list(Block *block);
  inline void add_blocks_to_free_list(Block *block);
  inline Block *try_get_block_from_free_list();
  template <AllocationMode canAlloc> Block *requisition_block();
  ProducerBase *recycle_or_create_producer(bool isExplicit);
  ProducerBase *recycle_or_create_producer(bool isExplicit, bool &recycled);
  ProducerBase *add_producer(ProducerBase *producer);
  void reown_producers();
  struct ImplicitProducerKVP {
    std::atomic<details::thread_id_t> key;
    ImplicitProducer *value;
    ImplicitProducerKVP();
    ImplicitProducerKVP(ImplicitProducerKVP &&other) noexcept;
    inline ImplicitProducerKVP &operator=(ImplicitProducerKVP &&other) noexcept;
    inline void swap(ImplicitProducerKVP &other) noexcept;
  };
  struct ImplicitProducerHash {
    size_t capacity;
    ImplicitProducerKVP *entries;
    ImplicitProducerHash *prev;
  };
  inline void populate_initial_implicit_producer_hash();
  void swap_implicit_producer_hashes(ConcurrentQueue &other);
  ImplicitProducer *get_or_add_implicit_producer();
  void implicit_producer_thread_exited(ImplicitProducer *producer);
  static void implicit_producer_thread_exited_callback(void *userData);
  template <typename U> static inline U *create_array(size_t count);
  template <typename U> static inline void destroy_array(U *p, size_t count);
  template <typename U> static inline U *create();
  template <typename U, typename A1> static inline U *create(A1 &&a1);
  template <typename U> static inline void destroy(U *p);
  std::atomic<ProducerBase *> producerListTail;
  std::atomic<std::uint32_t> producerCount;
  std::atomic<size_t> initialBlockPoolIndex;
  Block *initialBlockPool;
  size_t initialBlockPoolSize;
  FreeList<Block> freeList;
  std::atomic<ImplicitProducerHash *> implicitProducerHash;
  std::atomic<size_t> implicitProducerHashCount;
  ImplicitProducerHash initialImplicitProducerHash;
  std::array<ImplicitProducerKVP, INITIAL_IMPLICIT_PRODUCER_HASH_SIZE> initialImplicitProducerHashEntries;
  std::atomic_flag implicitProducerHashResizeInProgress;
  std::atomic<std::uint32_t> nextExplicitConsumerId;
  std::atomic<std::uint32_t> globalExplicitConsumerOffset;
};

} // namespace moodycamel

template <typename T> struct MPMCQueue {
  moodycamel::ConcurrentQueue<T> queue;
  MPMCQueue();
  bool push(T &&);
  bool pop(T &);
  ~MPMCQueue();
};

namespace mpmc {
template <typename T> struct Receiver {
  std::shared_ptr<MPMCQueue<T>> ptr;
  Receiver();
  Receiver(std::shared_ptr<MPMCQueue<T>> ptr);
  Receiver &operator=(Receiver &&);
  bool pop(T &);
  ~Receiver();
};
template <typename T> struct Sender {
  std::shared_ptr<MPMCQueue<T>> ptr;
  Sender();
  Sender(std::shared_ptr<MPMCQueue<T>> ptr);
  Sender(Sender &&);
  Sender(Sender const &);
  Sender &operator=(Sender &&);
  bool push(T &&);
  ~Sender();
};
template <typename T> void makeQueue(Sender<T> &, Receiver<T> &);
} // namespace mpmc