# Parallel Password Cracking with Pthreads

## Overview
This project implements a multi-threaded password-cracking program using the pthreads library. It follows the Producer-Consumer paradigm to efficiently search for passwords by hashing potential candidates and comparing them against a given password hash.

## Features
- Parallel execution using pthreads.
- Producer-Consumer synchronization using mutexes and condition variables.
- Configurable number of producer and consumer threads.
- Uses SHA-256 hashing for password verification.
- Performance measurement and debugging with GDB and Valgrind.

## Compilation & Execution
### Build
```sh
make
```

### Run
```sh
./cracker <dictionary_file> <hash_file> <output_file> <num_producers> <num_consumers>
```
#### Examples
```sh
./cracker cain.txt hash.txt result.txt 4 8
```

## Implementation Details
The project is structured as follows:
- `main.c`: Initializes the system, creates threads, and manages execution flow.
- `global.h`: Defines shared global variables, including the global buffer.
- `producer.c`: Reads words from the dictionary and writes to the global buffer.
- `consumer.c`: Reads words from the buffer, generates password variations, and compares hashes.

### Producer-Consumer Strategy
- **Producers** read words from the dictionary file into an internal buffer and write batches of words to the shared global buffer.
- **Consumers** retrieve words from the buffer, generate variations, hash them, and compare against the target hash.
- Mutexes and condition variables ensure synchronization between producers and consumers.

### Synchronization
- Producers wait if the buffer is full and notify consumers when new data is available.
- Consumers wait for available data and notify producers when space is freed.
- A global flag signals all threads to terminate when the password is found.

### Timing Execution
The `gettimeofday` function is used to measure the execution time of the password-cracking process.

## Debugging
Use GDB and Valgrind to debug memory errors:
```sh
gdb ./cracker
valgrind --leak-check=full ./cracker cain.txt hash.txt result.txt 4 8
```

## Code Style & Best Practices
- No global variables outside `global.h`.
- Functions should ideally be ≤ 100 lines.
- Use `malloc()` for dynamic memory allocation.
- Handle file errors gracefully.
- Ensure proper synchronization to avoid race conditions.

## Additional Notes
- `CTRL+C` terminates an infinite loop.
- Validate command-line arguments before execution.
- Ensure proper cleanup of threads and dynamically allocated memory.
