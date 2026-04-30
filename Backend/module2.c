#include <stdio.h>
#include <string.h>
#include "parcel.h"

void trackParcel(int id) {
    struct Parcel *temp = head;

    while (temp != NULL) {
        if (temp->trackingID == id) {
            printf("\n========== PARCEL TRACKING DETAILS ==========\n");

            printf("\nTracking ID       : %d", temp->trackingID);
            printf("\nStatus            : %s", temp->status);
            printf("\nDestination Office: %s", temp->destinationOffice);
            printf("\nDelivery Date     : %s", temp->deliveryDate);
            printf("\nRoute             : %s", temp->roadRoute);
            printf("\nAssigned Driver   : %s", temp->driverName);
            printf("\nPriority          : %s", temp->priority == 2 ? "Urgent" : "Normal");
            printf("\nFailed Attempts   : %d", temp->failedAttempts);

            printf("\n\n--- Sender Details ---");
            printf("\nName    : %s", temp->senderName);
            printf("\nAddress : %s", temp->senderAddress);
            printf("\nContact : %s", temp->senderContact);

            printf("\n\n--- Receiver Details ---");
            printf("\nName    : %s", temp->receiverName);
            printf("\nAddress : %s", temp->receiverAddress);
            printf("\nContact : %s", temp->receiverContact);

            printf("\n\n============================================\n");
            return;
        }

        temp = temp->next;
    }

    printf("\nParcel not found with Tracking ID: %d\n", id);
}