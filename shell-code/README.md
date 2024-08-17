# Simple C-Shell

This repository contains a C-shell program written in C that mimics basic shell functionality. It handles command input, supports background and foreground process execution, and includes signal handling for interrupts.

## Features

- **Command Execution**: Executes user commands in foreground or background.
- **Background Processes**: Allows commands to run in the background with `&`.
- **Process Management**: Reaps zombie processes and manages process groups.
- **Signal Handling**: Handles SIGINT to allow interruption of commands.

## Getting Started

### Prerequisites

- A Unix-like operating system (Linux, macOS, etc.)
- A C compiler (e.g., `gcc`)

### Compilation

To compile the shell program, run the following command in your terminal:

```bash
gcc -o my_shell simple_shell.c
```

### Usage

Run the compiled shell program with:

```bash
./simple_shell
```

You will be prompted with a `$` symbol indicating the shell is ready for input. You can enter commands and use `&` to run them in the background.

#### Example Commands

- **Foreground Command**:
  ```bash
  ls -l
  ```

- **Background Command**:
  ```bash
  sleep 10 &
  ```

- **Change Directory**:
  ```bash
  cd /path/to/directory
  ```

- **Exit Shell**:
  ```bash
  exit
  ```

### Key Functions

- **`tokenize(char *line)`**: Splits the input string into tokens based on spaces, tabs, and newlines.
- **`sigint_handler(int signal_code)`**: Handles `SIGINT` (Ctrl+C) and prints the prompt again if no foreground process is running.
- **`main(int argc, char *argv[])`**: Main loop for reading input, managing processes, and executing commands.

## Handling Signals and Background Processes

- **SIGINT**: When `Ctrl+C` is pressed, the shell will print a new prompt or a new line if a foreground process is running.
- **Background Processes**: Commands ending with `&` are run in the background. The shell will print the process ID and wait for background processes to complete.

## Code Structure

- **`tokenize` Function**: Parses user input into tokens.
- **`sigint_handler` Function**: Manages the SIGINT signal.
- **`main` Function**: Main loop for command input, process creation, and execution.
