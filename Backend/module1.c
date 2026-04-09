#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parcel.h"

struct Parcel* head = NULL;
int currentID = 1000; // starting tracking number

void InsertParcel() {
    struct Parcel* temp = (struct Parcel*)malloc(sizeof(struct Parcel));

    temp->trackingID = currentID++;

    printf("Enter Sender Name: ");
    scanf(" %[^\n]", temp->senderName);

    printf("Enter Sender Address: ");
    scanf(" %[^\n]", temp->senderAddress);

    printf("Enter Sender Contact: ");
    scanf(" %[^\n]", temp->senderContact);

    printf("Enter Receiver Name: ");
    scanf(" %[^\n]", temp->receiverName);

    printf("Enter Receiver Address: ");
    scanf(" %[^\n]", temp->receiverAddress);


    printf("Enter Receiver Contact: ");
    scanf(" %[^\n]", temp->receiverContact);

    strcpy(temp->status, "Dispatched");

    temp->next = head;
    head = temp;

    printf("\nParcel Created! Tracking ID: %d\n", temp->trackingID);
}
void trackParcel(int id) {
    struct Parcel* temp = head;

    while (temp != NULL) {
        if (temp->trackingID == id) {

            printf("\n\n      PARCEL DETAILS         \n\n");


            printf("\nTracking ID: %d", temp->trackingID);

            printf("\n\n--- Sender Details ---");
            printf("\nName: %s", temp->senderName);
            printf("\nAddress: %s", temp->senderAddress);
            printf("\nContact: %s", temp->senderContact);

            printf("\n\n--- Receiver Details ---");
            printf("\nName: %s", temp->receiverName);
            printf("\nAddress: %s", temp->receiverAddress);
            printf("\nContact: %s", temp->receiverContact);

            printf("\n\nStatus: %s\n", temp->status);

            return;
        }

        temp = temp->next;
    }

    printf("\n Parcel not found with Tracking ID: %d\n", id);
}

void DisplayParcels() {
    struct Parcel* temp = head;

    while (temp != NULL) {
        printf("\nTracking ID: %d", temp->trackingID);
        printf("\nSender: %s", temp->senderName);
        printf("\nReceiver: %s", temp->receiverName);
        printf("\nStatus: %s\n", temp->status);

        temp = temp->next;
    }
}

void SaveToFile() {
    FILE *fp = fopen("data.txt", "w");

    struct Parcel* temp = head;

    while (temp != NULL) {
        fprintf(fp, "%d|%s|%s|%s|%s|%s|%s|%s\n",
            temp->trackingID,
            temp->senderName,
            temp->senderAddress,
            temp->senderContact,
            temp->receiverName,
            temp->receiverAddress,
            temp->receiverContact,
            temp->status
        );

        temp = temp->next;
    }

    fclose(fp);
    printf("\nData saved to file.\n");
}