# 🐚 Mini Shell in C

A command-line based **Mini Shell** developed in C as part of my Linux Internals learning journey. This project implements a minimal shell that can execute commands and demonstrates important Linux system programming concepts such as process creation, signal handling, process synchronization, command parsing, job control, and pipes.

---

## 🚀 Project Overview

The Mini Shell provides a custom command prompt where users can enter commands and execute them.

The shell identifies whether a command is a built-in command or an external command and executes it accordingly.

The project also implements process control features such as handling stopped processes, foreground and background execution, signal handling, and multiple commands connected using pipes.

---

## ✅ Key Features

- Custom shell prompt
- Execute external Linux commands
- Built-in command support
- Command-line argument parsing
- Process creation using `fork()`
- Program execution using `execvp()`
- Process synchronization using `waitpid()`
- Signal handling
- Foreground and background process control
- Job management using a linked list
- Support for multiple pipes
- Special shell variables
- Exit status handling

---

## 🔍 Built-in Commands

The shell supports commands such as:

- `echo`
- `cd`
- `pwd`
- `clear`
- `jobs`
- `fg`
- `bg`
- `exit`

It also handles special variables such as:

- `$$` – Process ID
- `$?` – Exit status
- `$SHELL` – Shell environment variable

---

## 🔗 Pipe Functionality

The Mini Shell supports connecting multiple commands using pipes.

Example:

```bash
ls | grep .c
