#include <stdio.h>
#include <string.h>
#include "parcel.h"

void DisplayParcels() {
    struct Parcel *temp = head;

    if (temp == NULL) {
        printf("\nNo parcel records available.\n");
        return;
    }

    printf("\n========== ALL PARCEL RECORDS ==========\n");

    while (temp != NULL) {
        printf("\nTracking ID : %d", temp->trackingID);
        printf("\nSender      : %s", temp->senderName);
        printf("\nReceiver    : %s", temp->receiverName);
        printf("\nLocation    : %s", temp->destinationOffice);
        printf("\nDate        : %s", temp->deliveryDate);
        printf("\nRoute       : %s", temp->roadRoute);
        printf("\nDriver      : %s", temp->driverName);
        printf("\nStatus      : %s", temp->status);
        printf("\n--------------------------------------");

        temp = temp->next;
    }

    printf("\n");
}

void filterByLocation() {
    char location[50];
    int found = 0;

    printf("\nEnter destination office/location: ");
    scanf(" %[^\n]", location);

    struct Parcel *temp = head;

    while (temp != NULL) {
        if (strcmp(temp->destinationOffice, location) == 0) {
            printf("\nID: %d | Receiver: %s | Date: %s | Status: %s | Driver: %s",
                   temp->trackingID,
                   temp->receiverName,
                   temp->deliveryDate,
                   temp->status,
                   temp->driverName);
            found = 1;
        }

        temp = temp->next;
    }

    if (!found)
        printf("\nNo records found for location: %s\n", location);
}

void filterByDate() {
    char date[20];
    int found = 0;

    printf("\nEnter delivery date DD-MM-YYYY: ");
    scanf(" %[^\n]", date);

    struct Parcel *temp = head;

    while (temp != NULL) {
        if (strcmp(temp->deliveryDate, date) == 0) {
            printf("\nID: %d | Location: %s | Receiver: %s | Status: %s | Driver: %s",
                   temp->trackingID,
                   temp->destinationOffice,
                   temp->receiverName,
                   temp->status,
                   temp->driverName);
            found = 1;
        }

        temp = temp->next;
    }

    if (!found)
        printf("\nNo records found for date: %s\n", date);
}

void filterByLocationAndDate() {
    char location[50], date[20];
    int found = 0;

    printf("\nEnter destination office/location: ");
    scanf(" %[^\n]", location);

    printf("Enter delivery date DD-MM-YYYY: ");
    scanf(" %[^\n]", date);

    struct Parcel *temp = head;

    while (temp != NULL) {
        if (strcmp(temp->destinationOffice, location) == 0 &&
            strcmp(temp->deliveryDate, date) == 0) {

            printf("\nID: %d | Receiver: %s | Status: %s | Route: %s | Driver: %s",
                   temp->trackingID,
                   temp->receiverName,
                   temp->status,
                   temp->roadRoute,
                   temp->driverName);

            found = 1;
        }

        temp = temp->next;
    }

    if (!found)
        printf("\nNo matching records found.\n");
}