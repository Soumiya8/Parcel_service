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