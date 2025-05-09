/**
 * Incident Reporting System
 *
 * This application allows users to report and view incidents in a locality.
 * Features include:
 * - Creating new incident reports with area, type, and time information
 * - Viewing all incidents
 * - Filtering incidents by area or type
 * - Persistent data storage using files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_INCIDENTS 100
#define MAX_STRING_LENGTH 100
#define MAX_AREA_LENGTH 50
#define MAX_TYPE_LENGTH 50
#define MAX_TIME_LENGTH 20
#define DATA_FILE "incidents.txt"

// ANSI color codes for terminal output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Structure to represent an incident
struct Incident {
    char area[MAX_AREA_LENGTH];
    char type[MAX_TYPE_LENGTH];
    char time[MAX_TIME_LENGTH]; // Time when the incident occurred
    int id;
};

// Function declarations
void clearScreen();
void displayHeader(const char* title);
void displayMainMenu();
void displayViewMenu();
void addIncident();
void viewAllIncidents();
void viewIncidentsByArea();
void viewIncidentsByType();
int readIncidentsFromFile(struct Incident incidents[]);
void writeIncidentToFile(const struct Incident* incident);
void validateStringInput(char* input, int maxLength, const char* prompt);
void validateTimeInput(char* input, int maxLength);
int getNextIncidentId();
int isValidTimeFormat(const char* time);
int strContains(const char* str, const char* substr);

// Global array to store incidents
struct Incident incidents[MAX_INCIDENTS];
int incidentCount = 0;

int main() {
    int choice;

    // Load incidents from file
    incidentCount = readIncidentsFromFile(incidents);

    while (1) {
        clearScreen();
        displayHeader("INCIDENT REPORTING SYSTEM");
        displayMainMenu();

        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            // Clear input buffer if scanf fails
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            printf("Press Enter to continue...");
            getchar();
            continue;
        }

        // Clear input buffer
        while (getchar() != '\n');

        switch (choice) {
            case 1: // Report an incident
                clearScreen();
                displayHeader("REPORT NEW INCIDENT");
                addIncident();
                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 2: { // View incidents
                int viewChoice;
                int viewMenuActive = 1;

                while (viewMenuActive) {
                    clearScreen();
                    displayHeader("VIEW INCIDENTS");
                    displayViewMenu();

                    printf("Enter your choice: ");
                    if (scanf("%d", &viewChoice) != 1) {
                        // Clear input buffer if scanf fails
                        while (getchar() != '\n');
                        printf("Invalid input. Please enter a number.\n");
                        printf("Press Enter to continue...");
                        getchar();
                        continue;
                    }

                    // Clear input buffer
                    while (getchar() != '\n');

                    switch (viewChoice) {
                        case 1: // View all incidents
                            clearScreen();
                            displayHeader("ALL INCIDENTS");
                            viewAllIncidents();
                            printf("\nPress Enter to return to view menu...");
                            getchar();
                            break;

                        case 2: // Filter by area
                            clearScreen();
                            displayHeader("FILTER BY AREA");
                            viewIncidentsByArea();
                            printf("\nPress Enter to return to view menu...");
                            getchar();
                            break;

                        case 3: // Filter by type
                            clearScreen();
                            displayHeader("FILTER BY INCIDENT TYPE");
                            viewIncidentsByType();
                            printf("\nPress Enter to return to view menu...");
                            getchar();
                            break;

                        case 4: // Back to main menu
                            viewMenuActive = 0; // Set flag to exit the view menu loop
                            break;

                        default:
                            printf("Invalid choice. Please try again.\n");
                            printf("Press Enter to continue...");
                            getchar();
                    }
                }
                break;
            }

            case 3: // Exit
                clearScreen();
                printf("Thank you for using the Incident Reporting System!\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
                printf("Press Enter to continue...");
                getchar();
        }
    }

    return 0;
}

// Clear the console screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Display a formatted header
void displayHeader(const char* title) {
    printf(ANSI_COLOR_CYAN "==============================================\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW "           %s           \n" ANSI_COLOR_RESET, title);
    printf(ANSI_COLOR_CYAN "==============================================\n\n" ANSI_COLOR_RESET);
}

// Display the main menu options with incident count information
void displayMainMenu() {
    printf("1. Report a new incident\n");
    printf("2. View incidents " ANSI_COLOR_GREEN "(%d incident%s reported so far)" ANSI_COLOR_RESET "\n",
           incidentCount, (incidentCount == 1) ? "" : "s");
    printf("3. Exit\n\n");
}

// Display the view menu options
void displayViewMenu() {
    printf("1. View all incidents " ANSI_COLOR_GREEN "(%d incident%s)" ANSI_COLOR_RESET "\n",
           incidentCount, (incidentCount == 1) ? "" : "s");
    printf("2. Filter incidents by area\n");
    printf("3. Filter incidents by incident type\n");
    printf("4. Back to main menu\n\n");
}

// Add a new incident to the system
void addIncident() {
    if (incidentCount >= MAX_INCIDENTS) {
        printf(ANSI_COLOR_RED "Error: Maximum number of incidents reached.\n" ANSI_COLOR_RESET);
        return;
    }

    struct Incident newIncident;

    // Get area with validation
    validateStringInput(newIncident.area, MAX_AREA_LENGTH, "Enter the area where the incident occurred (e.g., Street name)");

    // Get incident type with validation
    validateStringInput(newIncident.type, MAX_TYPE_LENGTH, "Enter the type of incident (e.g., pothole, non-functional streetlight)");

    // Get time when the incident occurred with validation
    validateTimeInput(newIncident.time, MAX_TIME_LENGTH);

    // Assign ID
    newIncident.id = getNextIncidentId();

    // Add to array
    incidents[incidentCount] = newIncident;
    incidentCount++;

    // Write to file
    writeIncidentToFile(&newIncident);

    printf(ANSI_COLOR_GREEN "\nIncident reported successfully with ID: %d\n" ANSI_COLOR_RESET, newIncident.id);
}

// View all incidents
void viewAllIncidents() {
    if (incidentCount == 0) {
        printf("No incidents have been reported yet.\n");
        return;
    }

    printf("%-5s | %-30s | %-30s | %-20s\n", "ID", "Area", "Incident Type", "Time Occurred");
    printf("---------------------------------------------------------------------------------\n");

    for (int i = 0; i < incidentCount; i++) {
        printf("%-5d | " ANSI_COLOR_GREEN "%-30s" ANSI_COLOR_RESET " | "
               ANSI_COLOR_RED "%-30s" ANSI_COLOR_RESET " | "
               ANSI_COLOR_BLUE "%-20s" ANSI_COLOR_RESET "\n",
               incidents[i].id, incidents[i].area, incidents[i].type, incidents[i].time);
    }
}

// Helper function to check if a string contains a substring (case insensitive)
int strContains(const char* str, const char* substr) {
    char str_lower[MAX_STRING_LENGTH];
    char substr_lower[MAX_STRING_LENGTH];

    // Make lowercase copies
    size_t i;
    for (i = 0; i < strlen(str) && i < MAX_STRING_LENGTH - 1; i++) {
        str_lower[i] = tolower(str[i]);
    }
    str_lower[i] = '\0';

    for (i = 0; i < strlen(substr) && i < MAX_STRING_LENGTH - 1; i++) {
        substr_lower[i] = tolower(substr[i]);
    }
    substr_lower[i] = '\0';

    // Check if the lowercase substr appears in lowercase str
    return strstr(str_lower, substr_lower) != NULL;
}

// View incidents filtered by area
void viewIncidentsByArea() {
    if (incidentCount == 0) {
        printf("No incidents have been reported yet.\n");
        return;
    }

    char searchArea[MAX_AREA_LENGTH];
    validateStringInput(searchArea, MAX_AREA_LENGTH, "Enter area to filter by");

    printf("\nIncidents in area containing: %s\n", searchArea);
    printf("%-5s | %-30s | %-30s | %-20s\n", "ID", "Area", "Incident Type", "Time Occurred");
    printf("---------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < incidentCount; i++) {
        // Use partial matching instead of exact match
        if (strContains(incidents[i].area, searchArea)) {
            printf("%-5d | " ANSI_COLOR_GREEN "%-30s" ANSI_COLOR_RESET " | "
                   ANSI_COLOR_RED "%-30s" ANSI_COLOR_RESET " | "
                   ANSI_COLOR_BLUE "%-20s" ANSI_COLOR_RESET "\n",
                   incidents[i].id, incidents[i].area, incidents[i].type, incidents[i].time);
            found = 1;
        }
    }

    if (!found) {
        printf("No incidents found in this area.\n");
    }
}

// View incidents filtered by type
void viewIncidentsByType() {
    if (incidentCount == 0) {
        printf("No incidents have been reported yet.\n");
        return;
    }

    char searchType[MAX_TYPE_LENGTH];
    validateStringInput(searchType, MAX_TYPE_LENGTH, "Enter incident type to filter by");

    printf("\nIncidents of type containing: %s\n", searchType);
    printf("%-5s | %-30s | %-30s | %-20s\n", "ID", "Area", "Incident Type", "Time Occurred");
    printf("---------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < incidentCount; i++) {
        // Use partial matching instead of exact match
        if (strContains(incidents[i].type, searchType)) {
            printf("%-5d | " ANSI_COLOR_GREEN "%-30s" ANSI_COLOR_RESET " | "
                   ANSI_COLOR_RED "%-30s" ANSI_COLOR_RESET " | "
                   ANSI_COLOR_BLUE "%-20s" ANSI_COLOR_RESET "\n",
                   incidents[i].id, incidents[i].area, incidents[i].type, incidents[i].time);
            found = 1;
        }
    }

    if (!found) {
        printf("No incidents found of this type.\n");
    }
}

// Read incidents from file
int readIncidentsFromFile(struct Incident incidents[]) {
    FILE *file = fopen(DATA_FILE, "r");
    if (file == NULL) {
        // File doesn't exist yet, which is fine for a new system
        return 0;
    }

    int count = 0;
    char line[MAX_STRING_LENGTH * 3]; // Buffer to hold each line

    while (count < MAX_INCIDENTS && fgets(line, sizeof(line), file) != NULL) {
        // Remove newline character if present
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }

        // Parse the line using sscanf
        if (sscanf(line, "%d|%[^|]|%[^|]|%[^\n]",
                   &incidents[count].id,
                   incidents[count].area,
                   incidents[count].type,
                   incidents[count].time) == 4) {
            count++;
        }
    }

    fclose(file);
    return count;
}

// Write a new incident to file
void writeIncidentToFile(const struct Incident* incident) {
    FILE *file = fopen(DATA_FILE, "a");
    if (file == NULL) {
        printf(ANSI_COLOR_RED "Error: Could not open file for writing.\n" ANSI_COLOR_RESET);
        return;
    }

    fprintf(file, "%d|%s|%s|%s\n", incident->id, incident->area, incident->type, incident->time);
    fclose(file);
}

// Generate next incident ID
int getNextIncidentId() {
    int maxId = 0;
    for (int i = 0; i < incidentCount; i++) {
        if (incidents[i].id > maxId) {
            maxId = incidents[i].id;
        }
    }
    return maxId + 1;
}

// Validate string input with proper formatting and limits
void validateStringInput(char* input, int maxLength, const char* prompt) {
    int valid = 0;

    while (!valid) {
        printf("%s: ", prompt);
        if (fgets(input, maxLength, stdin) == NULL) {
            printf(ANSI_COLOR_RED "Error reading input. Please try again.\n" ANSI_COLOR_RESET);
            continue;
        }

        // Remove newline if present
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
            len--;
        }

        // Check if input is empty
        if (len == 0) {
            printf(ANSI_COLOR_RED "Input cannot be empty. Please try again.\n" ANSI_COLOR_RESET);
            continue;
        }

        // Check if input is too long
        if (len >= maxLength - 1) {
            printf(ANSI_COLOR_RED "Input too long (max %d characters). Please try again.\n" ANSI_COLOR_RESET, maxLength - 1);
            continue;
        }

        valid = 1;
    }
}

// Validate time input with proper formatting
void validateTimeInput(char* input, int maxLength) {
    int valid = 0;

    while (!valid) {
        printf("Enter the time when the incident occurred (HH:MM format, 24-hour clock): ");
        if (fgets(input, maxLength, stdin) == NULL) {
            printf(ANSI_COLOR_RED "Error reading input. Please try again.\n" ANSI_COLOR_RESET);
            continue;
        }

        // Remove newline if present
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
            len--;
        }

        // Check if input is empty
        if (len == 0) {
            printf(ANSI_COLOR_RED "Time cannot be empty. Please try again.\n" ANSI_COLOR_RESET);
            continue;
        }

        // Check time format
        if (!isValidTimeFormat(input)) {
            printf(ANSI_COLOR_RED "Invalid time format. Please use HH:MM (24-hour clock). Example: 14:30\n" ANSI_COLOR_RESET);
            continue;
        }

        valid = 1;
    }
}

// Check if the time format is valid (HH:MM)
int isValidTimeFormat(const char* time) {
    int hour, minute;
    char extra;

    // Check if the format is exactly HH:MM with no extra characters
    if (sscanf(time, "%d:%d%c", &hour, &minute, &extra) != 2) {
        return 0;
    }

    // Check if hour and minute are in valid ranges
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return 0;
    }

    return 1;
}