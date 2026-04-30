#include <stdio.h>
#include <string.h>
#include "parcel.h"

void trackingMenu() {
    int id;

    printf("\nEnter Tracking ID: ");
    scanf("%d", &id);

    trackParcel(id);
}

void recordsMenu() {
    int choice;

    while (1) {
        printf("\n\n========== DELIVERY RECORDS ==========");
        printf("\n1. View All Parcels");
        printf("\n2. Filter by Location");
        printf("\n3. Filter by Date");
        printf("\n4. Filter by Location and Date");
        printf("\n5. Back");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                DisplayParcels();
                break;

            case 2:
                filterByLocation();
                break;

            case 3:
                filterByDate();
                break;

            case 4:
                filterByLocationAndDate();
                break;

            case 5:
                return;

            default:
                printf("\nInvalid choice.\n");
        }
    }
}

void employeeMenu() {
    int choice;

    while (1) {
        printf("\n\n========== EMPLOYEE PANEL ==========");
        printf("\n1. Create Parcel");
        printf("\n2. Track Parcel");
        printf("\n3. Update Status / Notify");
        printf("\n4. Delivery Records and Filters");
        printf("\n5. Save Data");
        printf("\n6. Logout");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                InsertParcel();
                break;

            case 2:
                trackingMenu();
                break;

            case 3:
                updateParcelStatus();
                break;

            case 4:
                recordsMenu();
                break;

            case 5:
                SaveToFile();
                break;

            case 6:
                printf("\nLogging out...\n");
                return;

            default:
                printf("\nInvalid choice.\n");
        }
    }
}

void loginSystem() {
    char username[20], password[20];
    int attempts = 3;

    while (attempts > 0) {
        printf("\nUsername: ");
        scanf("%s", username);

        printf("Password: ");
        scanf("%s", password);

        if (strcmp(username, "admin") == 0 &&
            strcmp(password, "1234") == 0) {
            printf("\nLogin successful.\n");
            employeeMenu();
            return;
        }

        attempts--;
        printf("\nInvalid login. Attempts left: %d\n", attempts);
    }

    printf("\nToo many failed attempts.\n");
}

int main() {
    int choice;

    LoadFromFile();

    while (1) {
        printf("\n\n========== PARCEL SERVICE SYSTEM ==========");
        printf("\n1. Employee Login");
        printf("\n2. Track Parcel");
        printf("\n3. Exit");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
<<<<<<< HEAD

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
=======
            case 1:
                loginSystem();
>>>>>>> 5b17440 (Project Update)
                break;

            case 2:
                trackingMenu();
                break;

            case 3:
                SaveToFile();
                printf("\nExiting system...\n");
                return 0;

            default:
                printf("\nInvalid choice.\n");
        }
    }
}