/*
 * ============================================================
 *  HOSPITAL EMERGENCY QUEUE SIMULATOR
 *  Advanced C Programming Project
 * ============================================================
 *  What this program does:
 *    - Patients arrive at an emergency room with a severity level.
 *    - The MOST SERIOUS patient is always treated first.
 *    - Patients with the SAME severity are treated in the order
 *      they arrived (first come, first served).
 *
 *  C concepts used (all beginner level):
 *    1. struct               -> to store patient details
 *    2. pointers + malloc    -> to create patients while program runs
 *    3. linked list          -> to keep the patients in a queue
 *    4. file handling        -> to save / load the queue
 *    5. functions, loops, switch, arrays
 * ============================================================
 */

#include <stdio.h>    /* printf, scanf, fopen, fgets ...        */
#include <stdlib.h>   /* malloc, free, rand, srand, atoi         */
#include <string.h>   /* strcpy, strlen, strcspn                 */
#include <time.h>     /* time() - used to seed random numbers    */

/* ---------- Severity levels (1 = most serious) ---------- */
#define CRITICAL 1
#define SERIOUS  2
#define MODERATE 3
#define MINOR    4

#define FILE_NAME "patients.txt"

/* ---------- Patient structure (one node of the linked list) ---------- */
struct Patient {
    int id;                 /* unique patient number                 */
    char name[50];          /* patient name                          */
    int age;                /* patient age                           */
    int severity;           /* 1 = Critical ... 4 = Minor            */
    struct Patient *next;   /* address of the next patient in queue  */
};

/* ---------- Global variables ---------- */
struct Patient *head = NULL;   /* first patient in the queue (NULL = empty) */
int nextId = 1;                /* id given to the next new patient          */
int totalArrived = 0;          /* how many patients have arrived            */
int totalTreated = 0;          /* how many patients have been treated       */
int treatedBySeverity[5];      /* treated count for each severity (1 to 4)  */

/* ============================================================
 *  HELPER FUNCTIONS
 * ============================================================ */

/* Returns the severity number as a word, e.g. 1 -> "Critical" */
const char *severityName(int severity)
{
    switch (severity) {
        case CRITICAL: return "Critical";
        case SERIOUS:  return "Serious";
        case MODERATE: return "Moderate";
        case MINOR:    return "Minor";
        default:       return "Unknown";
    }
}

/*
 * Reads a whole number from the user and keeps asking
 * until the number is between min and max.
 * (fgets + atoi is safer than scanf if the user types letters)
 */
int readInt(const char *message, int min, int max)
{
    char line[50];
    int value;

    while (1) {
        printf("%s", message);
        fgets(line, sizeof(line), stdin);   /* read one line of text   */
        value = atoi(line);                 /* convert text to number  */
        if (value >= min && value <= max) {
            return value;                   /* valid number, give back */
        }
        printf("  Invalid input! Enter a number from %d to %d.\n", min, max);
    }
}

/* Reads a name (with spaces) from the user */
void readName(char name[])
{
    printf("Enter patient name: ");
    fgets(name, 50, stdin);                 /* read the line           */
    name[strcspn(name, "\n")] = '\0';       /* remove the '\n' at end  */
    if (strlen(name) == 0) {
        strcpy(name, "Unknown");            /* empty name -> Unknown   */
    }
}

/* ============================================================
 *  QUEUE FUNCTIONS
 * ============================================================ */

/*
 * Puts an already-created patient into the correct place in the queue.
 * Rule: lower severity number = more serious = closer to the front.
 */
void insertInQueue(struct Patient *newPatient)
{
    struct Patient *current;

    /* Case 1: queue is empty, OR new patient is more serious than first one */
    if (head == NULL || newPatient->severity < head->severity) {
        newPatient->next = head;   /* new patient points to old first patient */
        head = newPatient;         /* new patient becomes the first           */
        return;
    }

    /* Case 2: find the last patient whose severity is the same or lower
       (so same-severity patients stay in arrival order) */
    current = head;
    while (current->next != NULL &&
           current->next->severity <= newPatient->severity) {
        current = current->next;   /* move to the next patient */
    }

    /* Put the new patient after 'current' */
    newPatient->next = current->next;
    current->next = newPatient;
}

/* Creates a new patient in memory, then inserts it into the queue */
void addPatient(char name[], int age, int severity)
{
    struct Patient *newPatient;

    newPatient = (struct Patient *) malloc(sizeof(struct Patient));
    if (newPatient == NULL) {              /* malloc fails if memory is full */
        printf("Error: not enough memory!\n");
        return;
    }

    newPatient->id = nextId;
    strcpy(newPatient->name, name);
    newPatient->age = age;
    newPatient->severity = severity;
    newPatient->next = NULL;

    nextId++;
    totalArrived++;

    insertInQueue(newPatient);

    printf("  Patient added -> ID: %d | %s | Severity: %s\n",
           newPatient->id, newPatient->name, severityName(severity));
}

/* Menu option 1: ask the user for details and add the patient */
void addPatientFromUser(void)
{
    char name[50];
    int age, severity;

    printf("\n--- ADD NEW PATIENT ---\n");
    readName(name);
    age = readInt("Enter age (1-120): ", 1, 120);
    printf("Severity levels: 1 = Critical, 2 = Serious, 3 = Moderate, 4 = Minor\n");
    severity = readInt("Enter severity (1-4): ", 1, 4);

    addPatient(name, age, severity);
}

/* Menu option 2: treat (remove) the patient at the front of the queue */
void treatNextPatient(void)
{
    struct Patient *treated;

    printf("\n--- TREAT NEXT PATIENT ---\n");

    if (head == NULL) {
        printf("  No patients are waiting.\n");
        return;
    }

    treated = head;        /* the first patient is the most urgent */
    head = head->next;     /* second patient becomes the first     */

    printf("  Now treating -> ID: %d | %s | Age: %d | Severity: %s\n",
           treated->id, treated->name, treated->age,
           severityName(treated->severity));

    totalTreated++;
    treatedBySeverity[treated->severity]++;

    free(treated);         /* give the memory back */
}

/* Menu option 3: print all waiting patients */
void viewQueue(void)
{
    struct Patient *current;
    int position = 1;

    printf("\n--- PATIENTS WAITING ---\n");

    if (head == NULL) {
        printf("  The queue is empty.\n");
        return;
    }

    printf("%-5s %-5s %-20s %-5s %-10s\n", "No.", "ID", "Name", "Age", "Severity");
    printf("-----------------------------------------------------\n");

    current = head;
    while (current != NULL) {      /* go through the list one by one */
        printf("%-5d %-5d %-20s %-5d %-10s\n",
               position, current->id, current->name,
               current->age, severityName(current->severity));
        current = current->next;
        position++;
    }
}

/* Menu option 4: search a patient by ID */
void searchPatient(void)
{
    struct Patient *current;
    int id, position = 1;

    printf("\n--- SEARCH PATIENT ---\n");
    id = readInt("Enter patient ID: ", 1, 100000);

    current = head;
    while (current != NULL) {
        if (current->id == id) {
            printf("  Found! Position in queue: %d\n", position);
            printf("  ID: %d | Name: %s | Age: %d | Severity: %s\n",
                   current->id, current->name, current->age,
                   severityName(current->severity));
            return;
        }
        current = current->next;
        position++;
    }
    printf("  Patient with ID %d is not in the queue.\n", id);
}

/* Menu option 5: remove a waiting patient (e.g. patient left the hospital) */
void removePatient(void)
{
    struct Patient *current, *previous;
    int id;

    printf("\n--- REMOVE PATIENT FROM QUEUE ---\n");
    id = readInt("Enter patient ID to remove: ", 1, 100000);

    current = head;
    previous = NULL;

    while (current != NULL) {
        if (current->id == id) {
            if (previous == NULL) {
                head = current->next;          /* removing the first patient */
            } else {
                previous->next = current->next; /* skip over this patient    */
            }
            printf("  Patient %s (ID %d) removed from the queue.\n",
                   current->name, current->id);
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
    printf("  Patient with ID %d is not in the queue.\n", id);
}

/* Menu option 6: show simple statistics */
void showStatistics(void)
{
    struct Patient *current;
    int waiting = 0;
    int waitingBySeverity[5] = {0, 0, 0, 0, 0};
    int i;

    /* count waiting patients */
    current = head;
    while (current != NULL) {
        waiting++;
        waitingBySeverity[current->severity]++;
        current = current->next;
    }

    printf("\n--- HOSPITAL STATISTICS ---\n");
    printf("Total patients arrived : %d\n", totalArrived);
    printf("Total patients treated : %d\n", totalTreated);
    printf("Patients waiting now   : %d\n", waiting);
    printf("\n%-10s %-10s %-10s\n", "Severity", "Waiting", "Treated");
    for (i = 1; i <= 4; i++) {
        printf("%-10s %-10d %-10d\n", severityName(i),
               waitingBySeverity[i], treatedBySeverity[i]);
    }
}

/* Menu option 7: add some random patients automatically (simulation) */
void simulateArrivals(void)
{
    /* a few sample names to pick from */
    char *names[] = {"Rahul", "Priya", "Amit", "Neha", "Karan",
                     "Simran", "Rohan", "Anjali", "Vikas", "Pooja"};
    int howMany, i;

    printf("\n--- SIMULATE ARRIVALS ---\n");
    howMany = readInt("How many patients arrive (1-10): ", 1, 10);

    for (i = 0; i < howMany; i++) {
        addPatient(names[rand() % 10],      /* random name         */
                   rand() % 90 + 1,         /* random age 1 to 90  */
                   rand() % 4 + 1);         /* random severity 1-4 */
    }
}

/* ============================================================
 *  FILE FUNCTIONS
 * ============================================================ */

/* Menu option 8: save all waiting patients into a text file */
void saveToFile(void)
{
    FILE *fp;
    struct Patient *current;
    int count = 0;

    fp = fopen(FILE_NAME, "w");            /* "w" = write (creates file) */
    if (fp == NULL) {
        printf("  Error: could not open file for writing.\n");
        return;
    }

    current = head;
    while (current != NULL) {
        /* one patient per line, parts separated by the | symbol */
        fprintf(fp, "%d|%s|%d|%d\n", current->id, current->name,
                current->age, current->severity);
        current = current->next;
        count++;
    }

    fclose(fp);                            /* always close the file */
    printf("  %d patient(s) saved to %s\n", count, FILE_NAME);
}

/* Menu option 9: load patients from the text file */
void loadFromFile(void)
{
    FILE *fp;
    struct Patient *newPatient;
    int id, age, severity, count = 0;
    char name[50];

    /* Loading into a non-empty queue would create duplicate patients */
    if (head != NULL) {
        printf("  Queue is not empty! Treat or remove all patients first.\n");
        return;
    }

    fp = fopen(FILE_NAME, "r");            /* "r" = read */
    if (fp == NULL) {
        printf("  No saved file found.\n");
        return;
    }

    /* read line by line until the file ends */
    while (fscanf(fp, "%d|%49[^|]|%d|%d\n", &id, name, &age, &severity) == 4) {
        newPatient = (struct Patient *) malloc(sizeof(struct Patient));
        if (newPatient == NULL) {
            printf("Error: not enough memory!\n");
            break;
        }
        newPatient->id = id;
        strcpy(newPatient->name, name);
        newPatient->age = age;
        newPatient->severity = severity;
        newPatient->next = NULL;

        insertInQueue(newPatient);

        if (id >= nextId) {
            nextId = id + 1;               /* keep ids unique */
        }
        totalArrived++;
        count++;
    }

    fclose(fp);
    printf("  %d patient(s) loaded from %s\n", count, FILE_NAME);
}

/* ============================================================
 *  CLEAN UP + MENU + MAIN
 * ============================================================ */

/* Frees every patient still in the queue (done before the program ends) */
void freeQueue(void)
{
    struct Patient *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void showMenu(void)
{
    printf("\n=========================================\n");
    printf("   HOSPITAL EMERGENCY QUEUE SIMULATOR\n");
    printf("=========================================\n");
    printf(" 1. Add new patient\n");
    printf(" 2. Treat next patient\n");
    printf(" 3. View waiting queue\n");
    printf(" 4. Search patient by ID\n");
    printf(" 5. Remove patient from queue\n");
    printf(" 6. Show statistics\n");
    printf(" 7. Simulate random arrivals\n");
    printf(" 8. Save queue to file\n");
    printf(" 9. Load queue from file\n");
    printf(" 0. Exit\n");
    printf("=========================================\n");
}

int main(void)
{
    int choice;

    srand((unsigned int) time(NULL));   /* so random numbers differ each run */

    do {
        showMenu();
        choice = readInt("Enter your choice (0-9): ", 0, 9);

        switch (choice) {
            case 1: addPatientFromUser(); break;
            case 2: treatNextPatient();   break;
            case 3: viewQueue();          break;
            case 4: searchPatient();      break;
            case 5: removePatient();      break;
            case 6: showStatistics();     break;
            case 7: simulateArrivals();   break;
            case 8: saveToFile();         break;
            case 9: loadFromFile();       break;
            case 0: printf("\nExiting... Stay safe!\n"); break;
        }
    } while (choice != 0);

    freeQueue();    /* release memory before closing */
    return 0;
}
