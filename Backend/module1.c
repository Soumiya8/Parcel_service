#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parcel.h"

struct Parcel *head = NULL;
int currentID = 1000;

void assignDriverByRoute(struct Parcel *p) {
    struct Parcel *temp = head;

    while (temp != NULL) {
        if (strcmp(temp->roadRoute, p->roadRoute) == 0 &&
            strcmp(temp->deliveryDate, p->deliveryDate) == 0 &&
            strcmp(temp->status, "Delivered") != 0 &&
            strlen(temp->driverName) > 0) {

            strcpy(p->driverName, temp->driverName);
            return;
        }

        temp = temp->next;
    }

    if (strcmp(p->roadRoute, "OMR") == 0)
        strcpy(p->driverName, "Driver Arun");
    else if (strcmp(p->roadRoute, "ECR") == 0)
        strcpy(p->driverName, "Driver Bala");
    else if (strcmp(p->roadRoute, "GST") == 0)
        strcpy(p->driverName, "Driver Karthik");
    else if (strcmp(p->roadRoute, "PORUR") == 0)
        strcpy(p->driverName, "Driver Naveen");
    else
        strcpy(p->driverName, "Driver Common Route");
}

void InsertParcel() {
    struct Parcel *temp = malloc(sizeof(struct Parcel));

    if (temp == NULL) {
        printf("\nMemory allocation failed.\n");
        return;
    }

    temp->trackingID = currentID++;

    printf("\nEnter Sender Name: ");
    scanf(" %[^\n]", temp->senderName);

    printf("Enter Sender Address: ");
    scanf(" %[^\n]", temp->senderAddress);

    printf("Enter Sender Contact: ");
    scanf(" %[^\n]", temp->senderContact);

    printf("\nEnter Receiver Name: ");
    scanf(" %[^\n]", temp->receiverName);

    printf("Enter Receiver Address: ");
    scanf(" %[^\n]", temp->receiverAddress);

    printf("Enter Receiver Contact: ");
    scanf(" %[^\n]", temp->receiverContact);

    printf("Enter Destination Office: ");
    scanf(" %[^\n]", temp->destinationOffice);

    printf("Enter Delivery Date (DD-MM-YYYY): ");
    scanf(" %[^\n]", temp->deliveryDate);

    printf("Enter Road Route / Area Code (OMR/ECR/GST/PORUR/etc): ");
    scanf(" %[^\n]", temp->roadRoute);

    printf("Priority? 1 = Normal, 2 = Urgent: ");
    scanf("%d", &temp->priority);

    strcpy(temp->status, "Dispatched");
    temp->failedAttempts = 0;

    assignDriverByRoute(temp);

    temp->next = head;
    head = temp;

    printf("\nParcel Created Successfully.");
    printf("\nTracking ID     : %d", temp->trackingID);
    printf("\nAssigned Driver : %s", temp->driverName);
    printf("\nRoute Group     : %s", temp->roadRoute);
    printf("\nDelivery Date   : %s\n", temp->deliveryDate);
}

void SaveToFile() {
    FILE *fp = fopen("data.txt", "w");

    if (fp == NULL) {
        printf("\nUnable to open data.txt\n");
        return;
    }

    struct Parcel *temp = head;

    while (temp != NULL) {
        fprintf(fp, "%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d\n",
                temp->trackingID,
                temp->senderName,
                temp->senderAddress,
                temp->senderContact,
                temp->receiverName,
                temp->receiverAddress,
                temp->receiverContact,
                temp->status,
                temp->destinationOffice,
                temp->deliveryDate,
                temp->roadRoute,
                temp->driverName,
                temp->priority,
                temp->failedAttempts);

        temp = temp->next;
    }

    fclose(fp);
    printf("\nData saved successfully to data.txt\n");
}

void LoadFromFile() {
    FILE *fp = fopen("data.txt", "r");

    if (fp == NULL) {
        printf("\nNo existing data file found.\n");
        return;
    }

    char line[600];

    while (fgets(line, sizeof(line), fp)) {
        struct Parcel *temp = malloc(sizeof(struct Parcel));

        if (temp == NULL) {
            printf("\nMemory allocation failed.\n");
            fclose(fp);
            return;
        }

        strcpy(temp->destinationOffice, "Not Updated");
        strcpy(temp->deliveryDate, "Not Updated");
        strcpy(temp->roadRoute, "General");
        strcpy(temp->driverName, "Not Assigned");

        temp->priority = 1;
        temp->failedAttempts = 0;
        temp->next = NULL;

        int result = sscanf(line,
            "%d|%49[^|]|%99[^|]|%14[^|]|%49[^|]|%99[^|]|%14[^|]|%49[^|]|%49[^|]|%19[^|]|%49[^|]|%49[^|]|%d|%d",
            &temp->trackingID,
            temp->senderName,
            temp->senderAddress,
            temp->senderContact,
            temp->receiverName,
            temp->receiverAddress,
            temp->receiverContact,
            temp->status,
            temp->destinationOffice,
            temp->deliveryDate,
            temp->roadRoute,
            temp->driverName,
            &temp->priority,
            &temp->failedAttempts
        );

        if (result < 8) {
            free(temp);
            continue;
        }

        temp->next = head;
        head = temp;

        if (temp->trackingID >= currentID) {
            currentID = temp->trackingID + 1;
        }
    }

    fclose(fp);
    printf("\nData loaded successfully.\n");
}