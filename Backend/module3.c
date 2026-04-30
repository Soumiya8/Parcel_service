#include <stdio.h>
#include <string.h>
#include "parcel.h"

void notifySenderReceiver(struct Parcel *p, char message[]) {
    printf("\n========== NOTIFICATION ==========\n");
    printf("To Sender   [%s]: %s\n", p->senderContact, message);
    printf("To Receiver [%s]: %s\n", p->receiverContact, message);
    printf("=================================\n");
}

void handleFailedDelivery(int id) {
    struct Parcel *temp = head;

    while (temp != NULL) {
        if (temp->trackingID == id) {
            temp->failedAttempts++;

            if (temp->failedAttempts == 1) {
                strcpy(temp->status, "Delivery Failed - Rescheduled");
                notifySenderReceiver(temp, "Delivery failed once. Parcel will be reattempted.");
            }
            else if (temp->failedAttempts == 2) {
                strcpy(temp->status, "Urgent Attention Needed");
                temp->priority = 2;
                notifySenderReceiver(temp, "Second failed attempt. Parcel marked urgent.");
            }
            else {
                strcpy(temp->status, "Escalated to Office Manager");
                notifySenderReceiver(temp, "Multiple failed attempts. Office manager has been notified.");
                printf("\nOffice Manager Alert: Parcel %d needs manual intervention.\n", temp->trackingID);
            }

            printf("\nFailed delivery handled successfully.\n");
            return;
        }

        temp = temp->next;
    }

    printf("\nParcel not found.\n");
}

void updateParcelStatus() {
    int id, choice;
    struct Parcel *temp = head;

    printf("\nEnter Tracking ID: ");
    scanf("%d", &id);

    while (temp != NULL) {
        if (temp->trackingID == id) {
            printf("\nCurrent Status: %s\n", temp->status);

            printf("\nChoose New Status:");
            printf("\n1. Dispatched");
            printf("\n2. In Transit");
            printf("\n3. Reached Destination Office");
            printf("\n4. Out for Delivery");
            printf("\n5. Delivered");
            printf("\n6. Delivery Failed");
            printf("\nEnter choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    strcpy(temp->status, "Dispatched");
                    break;

                case 2:
                    strcpy(temp->status, "In Transit");
                    break;

                case 3:
                    strcpy(temp->status, "Reached Destination Office");
                    notifySenderReceiver(temp, "Parcel has reached the destination office.");
                    break;

                case 4:
                    strcpy(temp->status, "Out for Delivery");
                    notifySenderReceiver(temp, "Parcel is out for delivery today.");
                    break;

                case 5:
                    strcpy(temp->status, "Delivered");
                    notifySenderReceiver(temp, "Parcel delivered successfully.");
                    break;

                case 6:
                    handleFailedDelivery(id);
                    return;

                default:
                    printf("\nInvalid status choice.\n");
                    return;
            }

            printf("\nStatus updated successfully.\n");
            return;
        }

        temp = temp->next;
    }

    printf("\nParcel not found.\n");
}