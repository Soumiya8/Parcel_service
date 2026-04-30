#include <stdlib.h>
#include <string.h>
#include "parcel.h"

void notifySenderReceiver(Parcel *parcel, char message[]) {
    printf("\"notification\":{\"senderContact\":");
    JsonEscapePrint(parcel->senderContact);
    printf(",\"receiverContact\":");
    JsonEscapePrint(parcel->receiverContact);
    printf(",\"message\":");
    JsonEscapePrint(message);
    printf("}");
}

void updateStatus(char trackingID[], char newStatus[]) {
    Parcel *parcel;
    int id;

    if (IsBlank(trackingID) || IsBlank(newStatus)) {
        printf("{\"success\":false,\"error\":\"Tracking ID and status are required\"}\n");
        return;
    }

    id = atoi(trackingID);
    parcel = FindParcel(id);

    if (parcel == NULL) {
        printf("{\"success\":false,\"error\":\"Parcel not found\"}\n");
        return;
    }

    if (strcmp(newStatus, "Delivery Failed") == 0) {
        handleFailedDelivery(id);
        return;
    }

    snprintf(parcel->status, sizeof(parcel->status), "%s", newStatus);
    CleanField(parcel->status);

    if (strcmp(parcel->status, "Delivered") == 0) {
        parcel->failedAttempts = 0;
    }

    SaveToFile();

    printf("{\"success\":true,\"message\":\"Status updated\",");
    notifySenderReceiver(parcel, "Parcel status updated.");
    printf(",\"parcel\":");
    PrintParcelJson(parcel);
    printf("}\n");
}

void handleFailedDelivery(int id) {
    Parcel *parcel = FindParcel(id);

    if (parcel == NULL) {
        printf("{\"success\":false,\"error\":\"Parcel not found\"}\n");
        return;
    }

    parcel->failedAttempts++;

    if (parcel->failedAttempts == 1) {
        strcpy(parcel->status, "Delivery Failed - Rescheduled");
    } else if (parcel->failedAttempts == 2) {
        strcpy(parcel->status, "Office Hold");
        parcel->priority = 2;
    } else {
        strcpy(parcel->status, "Emergency Escalation");
        parcel->priority = 2;
    }

    SaveToFile();

    printf("{\"success\":true,\"message\":\"Failed delivery recorded\",");
    notifySenderReceiver(parcel, "Failed delivery status updated.");
    printf(",\"parcel\":");
    PrintParcelJson(parcel);
    printf("}\n");
}

void updateParcelStatus(void) {
    printf("{\"success\":false,\"error\":\"Use the web form or CLI update command\"}\n");
}
