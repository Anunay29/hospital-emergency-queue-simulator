# Hospital Emergency Queue Simulator

A console-based C program that simulates how an emergency room treats patients.
The **most serious patient is always treated first**. Patients with the same
severity are treated in the order they arrived.

## Features
- Add a patient (name, age, severity 1-4)
- Treat the next patient (highest priority first)
- View the waiting queue
- Search a patient by ID
- Remove a patient from the queue
- Statistics (arrived, treated, waiting, per severity)
- Simulate random arrivals
- Save / load the queue to `patients.txt`

## Severity Levels
| Level | Meaning  |
|-------|----------|
| 1     | Critical |
| 2     | Serious  |
| 3     | Moderate |
| 4     | Minor    |

## C Concepts Used
- Structures (`struct Patient`)
- Pointers and dynamic memory (`malloc`, `free`)
- Singly linked list (priority queue)
- File handling (`fopen`, `fprintf`, `fscanf`, `fclose`)
- Functions, loops, `switch`, arrays

## How to Compile and Run
```
gcc hospital.c -o hospital
./hospital          (Windows: hospital.exe)
```

## Author
Your Name - Advanced C Programming Project
