#include <stdio.h>
#include <string.h>
#include "parcel.h"

void userMenu() {
    int id;

    printf("\n--- Parcel Tracking ---\n");
    printf("Enter Tracking ID: ");
    scanf("%d", &id);

    trackParcel(id);   
}

void employeeMenu() {
    int choice;

    while (1) {
        printf("\n\n        EMPLOYEE PANEL        \n\n");
        printf("\n1. Add Parcel");
        printf("\n2. View All Parcels");
        printf("\n3. Save Data");
        printf("\n4. Logout");
        printf("\nEnter choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                InsertParcel();
                break;

            case 2:
                DisplayParcels();
                break;

            case 3:
                SaveToFile();
                break;

            case 4:
                printf("\nLogging out...\n");
                return;

            default:
                printf("\nInvalid choice! Try again.\n");
        }
    }
}


void loginSystem() {
    int choice;
    char username[20], password[20];

    while (1) {
        printf("\n\n    PARCEL SERVICE SYSTEM      \n\n");
        printf("\n1. Employee Login");
        printf("\n2. Track Parcel");
        printf("\n3. Exit");
        printf("\nEnter choice: ");

        scanf("%d", &choice);

        switch (choice) {

            case 1: {
                int attempts = 3;

                while (attempts--) {
                    printf("\nUsername: ");
                    scanf("%s", username);

                    printf("Password: ");
                    scanf("%s", password);

                    if (strcmp(username, "admin") == 0 &&
                        strcmp(password, "1234") == 0) {

                        printf("\nLogin successful! Welcome, %s\n", username);
                        employeeMenu();
                        break;
                    } else {
                        printf("\nInvalid credentials! Attempts left: %d\n", attempts);
                    }
                }

                if (attempts < 0) {
                    printf("\nToo many failed attempts!\n");
                }
                break;
            }

            case 2:
                userMenu();
                break;

            case 3:
                printf("\nExiting system...\n");
                return;

            default:
                printf("\nInvalid choice! Try again.\n");
        }
    }
}


int main() {
    loginSystem();
    LoadFromFile();
    return 0;
}