# process-mgmt.


# Process Management Utility

This utility is designed to manage and interact with Linux processes. It provides various functionalities such as checking if a process is a zombie, resuming stopped processes, retrieving descendants of a process, and more.

## Features

1. **Check Zombie Status**: Determines if a given process is a zombie.
2. **Resume Stopped Processes**: Sends a `SIGCONT` signal to stopped processes.
3. **Retrieve Grandchildren**: Lists all grandchildren of a process.
4. **Find All Defunct Descendants**: Lists all defunct (zombie) descendants of a process.
5. **List Non-Direct Descendants**: Lists all non-direct descendants of a process.
6. **Immediate Children**: Lists immediate children of a process.
7. **Terminate Processes**: Terminates a process or the root process.
8. **Stop Processes**: Stops (pauses) a process.
9. **List Siblings**: Lists siblings of a process.
10. **Process Tree Membership**: Checks if a process belongs to a specified process tree.

## Usage

### Command Syntax

```
a2prc <processID> <rootProcess> [option]
```

### Arguments

1. `processID`: The ID of the process to be managed.
2. `rootProcess`: The ID of the root process.
3. `option`: (Optional) The action to perform. Valid options are:
   - `-rp`: Kill the specified process.
   - `-pr`: Kill the root process.
   - `-xn`: List non-direct descendants.
   - `-xd`: List immediate children.
   - `-xs`: List siblings.
   - `-xt`: Stop (pause) the specified process.
   - `-xc`: Resume all stopped processes.
   - `-xz`: List all defunct (zombie) descendants.
   - `-xg`: List all grandchildren.
   - `-zs`: Check if the process is defunct (zombie).

### Examples

- **Check Zombie Status**:
  ```sh
  a2prc 1234 1 -zs
  ```

- **Resume All Stopped Processes**:
  ```sh
  a2prc 1234 1 -xc
  ```

- **List Immediate Children**:
  ```sh
  a2prc 1234 1 -xd
  ```

## Functions

### 1. `int zombie(int processId)`

- Checks if a process is a zombie.
- **Arguments**: `processId` - ID of the process.
- **Returns**: 1 if the process is a zombie, -1 otherwise.

### 2. `int sigcont()`

- Resumes all stopped processes by sending the `SIGCONT` signal.
- **Returns**: The number of processes resumed.

### 3. `void grandchild(int processId, int level, int dupli[])`

- Lists all grandchildren of a process.
- **Arguments**:
  - `processId` - ID of the process.
  - `level` - Depth of the process in the hierarchy.
  - `dupli[]` - Array to track visited processes.

### 4. `void allchildzombie(int processId, int dupli[])`

- Lists all defunct (zombie) descendants of a process.
- **Arguments**:
  - `processId` - ID of the process.
  - `dupli[]` - Array to track visited processes.

### 5. `void nddescendents(int processId, int level, int parentId, int dupli[])`

- Lists all non-direct descendants of a process.
- **Arguments**:
  - `processId` - ID of the process.
  - `level` - Depth of the process in the hierarchy.
  - `parentId` - ID of the parent process.
  - `dupli[]` - Array to track visited processes.

### 6. `int child(int processId, int pid)`

- Lists immediate children of a process.
- **Arguments**:
  - `processId` - ID of the process.
  - `pid` - ID of the current process.

### 7. `void printPID(int pid, int ppid, char* option)`

- Manages and interacts with processes based on the provided option.
- **Arguments**:
  - `pid` - ID of the process.
  - `ppid` - ID of the parent process.
  - `option` - Action to perform.

## Compilation and Execution

1. **Compile the program**:
   ```sh
   gcc -o a2prc a2prc.c
   ```

2. **Run the program**:
   ```sh
   ./a2prc <processID> <rootProcess> [option]
   ```

## Error Handling

- The program checks for valid arguments and prints appropriate error messages for invalid inputs or options.
- Proper error handling is implemented for file operations and memory allocations.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.