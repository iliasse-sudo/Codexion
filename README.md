*This project has been created as part of the 42 curriculum by ibaya.*

# Codexion

## Description

Codexion is a multi-threaded simulation of coders working in a circular co-working hub,
competing for limited USB dongles to compile their quantum code. Each coder cycles through
three states — **compiling**, **debugging**, and **refactoring** — and requires **two
adjacent dongles** simultaneously to compile. The simulation models resource contention,
deadlock prevention, fair scheduling, and precise burnout detection using POSIX threads,
mutexes, and condition variables.

**Core challenges addressed:**
- Deadlock-free resource acquisition (dining philosophers variant)
- Fair arbitration via FIFO or Earliest Deadline First (EDF) scheduling
- Sub-10ms burnout detection via a dedicated monitor thread
- Serialized, non-interleaved logging under concurrency
- Zero memory leaks with a custom allocation tracker

## Instructions

### Compilation

```bash
make          # Compiles the "codexion" binary with -Wall -Wextra -Werror -pthread
make clean    # Removes object files (*.o)
make fclean   # Removes object files and the binary
make re       # Full rebuild (fclean + all)
```

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Type | Description |
| :--- | :--- | :--- |
| `number_of_coders` | Positive integer | Number of coders (and dongles). Must be ≥ 1. |
| `time_to_burnout` | Positive integer (ms) | Max time without starting a compile before burnout. |
| `time_to_compile` | Positive integer (ms) | Time spent compiling (holding both dongles). |
| `time_to_debug` | Positive integer (ms) | Time spent debugging after releasing dongles. |
| `time_to_refactor` | Positive integer (ms) | Time spent refactoring before retrying to compile. |
| `number_of_compiles_required` | Non-negative integer | Simulation stops when all coders reach this count. |
| `dongle_cooldown` | Non-negative integer (ms) | Time a released dongle stays unavailable. |
| `scheduler` | `fifo` or `edf` | Arbitration policy for contested dongles. |

**Example:**
```bash
./codexion 5 800 200 200 200 5 50 edf
```

### Input Validation

- Exactly 8 arguments required (argc = 9)
- All numeric arguments must be valid non-negative integers
- `number_of_coders` must be ≥ 1
- `scheduler` must be exactly `fifo` or `edf`
- Invalid input prints `Error: Invalid arguments` to stderr and exits with code 1

## Resources

### References & Documentation

- **POSIX Threads Programming** (LLNL tutorial) — pthreads, mutexes, condition variables
- **man pages**: `pthread_create`, `pthread_mutex_lock`, `pthread_cond_wait`,
  `pthread_cond_timedwait`, `gettimeofday`, `usleep`
- **Dining Philosophers Problem & Coffman's Conditions** (classic OS literature)
  - [Wikipedia: Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- **Earliest Deadline First (EDF) Scheduling** — real-time systems literature
- **Pthreads Condition Variables for Signaling Between Threads**
  - [CircuitLabs article](https://circuitlabs.net/pthreads-condition-variables-for-signaling-between-threads/)

### AI Usage

AI was used to assist with:
- Structuring and writing this README.md
- Understanding low-level threading concepts: mutex lock/unlock internals, thread sleep behavior
  (`usleep`/`nanosleep`), futex queue mechanics when a thread attempts to lock an already-locked
  mutex, condition variable wait/signal semantics, and spurious wakeups
- Reviewing the binary min-heap implementation (array indexing, sift-up/sift-down algorithms)
- Norminette compliance guidance (function splitting, line limits, variable limits)

All AI-generated content was reviewed, tested, and fully understood before inclusion.

## Blocking Cases Handled

### Deadlock Prevention (Coffman's Four Conditions)

| Condition | Status | Implementation |
| :--- | :--- | :--- |
| **Mutual Exclusion** | Required | Each dongle protected by its own `pthread_mutex_t` |
| **Hold and Wait** | **Eliminated** | `try_acquire_both()` atomically validates both dongles under ordered lock acquisition (by dongle ID) — a coder never holds one dongle while waiting for the other |
| **No Preemption** | Required | Dongles voluntarily released after compile phase |
| **Circular Wait** | **Eliminated** | Fixed lock ordering (lower dongle ID first) + initial odd-coder stagger |

### Starvation Prevention

Each dongle maintains a **min-heap priority queue** of waiting coders:
- **FIFO**: Ordered by `arrival_time` (then `coder_id` tiebreaker)
- **EDF**: Ordered by `deadline = last_compile_start + time_to_burnout` (then `arrival_time`, then `coder_id`)
- Deterministic tiebreakers guarantee fully deterministic scheduling even with equal deadlines

### Cooldown Handling

- Each dongle tracks `available_at` timestamp (release time + `dongle_cooldown`)
- Waiting coders distinguish two cases:
  - **Cooldown active**: `ft_usleep()` until `available_at` (timed wait)
  - **Blocked by queue**: `pthread_cond_wait()` on coder's condition variable (signaled on release)

### Precise Burnout Detection

- Dedicated **monitor thread** polls every **500 μs**
- Reads each coder's `last_compile_start` under the coder's mutex
- If `now - last_compile_start > time_to_burnout`, logs burnout via `announce_burnout()` within **10 ms**
- Sets global `sim_stop` flag under `state_mutex` and broadcasts wakeups to all coders

### Log Serialization

- Global `write_mutex` protects all `printf` calls — no interleaved output lines
- `state_mutex` ensures atomic check of `sim_stop` flag alongside timestamp capture
- Output format: `timestamp_in_ms X has taken a dongle / is compiling / is debugging / is refactoring / burned out`

## Thread Synchronization Mechanisms

| Primitive | Scope | Protects |
| :--- | :--- | :--- |
| `pthread_mutex_t` | Per-dongle | `is_held`, `available_at`, heap queue |
| `pthread_mutex_t` | Per-coder | `last_compile_start`, `compiles_done` |
| `pthread_cond_t` | Per-coder | Blocks coder until dongle available (signaled on release) |
| `pthread_mutex_t` | Global (`write_mutex`) | Serializes all log output |
| `pthread_mutex_t` | Global (`state_mutex`) | Protects `sim_stop` simulation flag |

### Coordination Flow

1. **Coder requests dongles** → pushes request to both adjacent dongles' heaps (under each dongle's mutex)
2. **Atomic acquisition check** → `try_acquire_both()` locks both dongles in ID order, validates readiness, pops both heaps
3. **Compile phase** → coder holds both dongles, sleeps `time_to_compile`
4. **Release** → sets `available_at = now + cooldown`, signals next waiter via `pthread_cond_signal`
5. **Monitor** → periodically reads `last_compile_start` under coder's lock; on burnout, locks `state_mutex`, sets `sim_stop`, logs under `write_mutex`, broadcasts to all coders

### Race Condition Prevention

- All shared state accessed only while holding the appropriate mutex
- Strict lock ordering (by dongle ID) eliminates circular wait
- Monitor reads coder state under coder's lock — never observes torn/partial updates
- `sim_stop` flag checked atomically with `state_mutex` in all threads

## Custom Allocation Tracker / Garbage Collector

A centralized memory management system (`t_allocs_tracker`) tracks **every** dynamic allocation
and guarantees leak-free cleanup:

### Three Linked Lists

| List | Tracks | Cleanup Action |
| :--- | :--- | :--- |
| `t_alloc` | All `malloc`'d memory pointers | `free(ptr)` |
| `t_mutex_alloc` | All `pthread_mutex_t*` | `pthread_mutex_destroy()` + `free()` |
| `t_cond_alloc` | All `pthread_cond_t*` | `pthread_cond_destroy()` + `free()` |

### API

- `init_alloc_saver()` — initializes empty tracker
- `ft_malloc(tracker, size)` — allocates memory + registers in tracker
- `create_mutex(tracker)` — initializes mutex + registers in tracker
- `create_cond(tracker)` — initializes condition variable + registers in tracker
- `ft_free(tracker)` — destroys all mutexes, cond vars, and frees all memory in reverse order

**Benefits:** Zero memory leaks, zero mutex/cond var leaks, single cleanup call in `main.c`.

## Technical Highlights

### Single-Coder Edge Case
When `number_of_coders == 1`, both `left_dongle` and `right_dongle` point to the same dongle.
The code detects `left_dongle == right_dongle` in `grab_dongles()` and avoids self-deadlock by
sleeping until burnout — matching the subject specification exactly.

### Custom Binary Min-Heap
- Array-based binary heap with `O(log K)` push/pop
- Capacity = 2 (maximum 2 requests per dongle: left and right neighbor)
- No standard library priority queue used (requirement-compliant)

### Atomic Two-Dongle Acquisition
`try_acquire_both()` locks both dongles in fixed ID order, checks both are free and off cooldown
and that the coder is at the head of both heaps — then pops both heaps atomically. No hold-and-wait.

### Odd-Coder Stagger Trick
Odd-numbered coders sleep `time_to_compile / 2` at startup. This breaks symmetry in the circular
arrangement, ensuring the maximum number of non-conflicting coders (⌊N/2⌋) start compiling
immediately without contention.

### Monitor Polling Interval (500 μs)
Chosen to guarantee **sub-10ms burnout detection** (requirement) while keeping CPU overhead
minimal. At 500 μs, worst-case detection latency is 500 μs + scheduling jitter, well within
the 10 ms bound.

### Timing
- `gettimeofday()` for millisecond-precision timestamps
- `ft_usleep()` with simulation-stop checking — wakes early if `sim_stop` is set