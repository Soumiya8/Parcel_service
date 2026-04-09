#ifndef PARCEL_H
#define PARCEL_H

struct Parcel {
    int trackingID;
    char senderName[50];
    char senderAddress[100];
    char senderContact[15];

    char receiverName[50];
    char receiverAddress[100];
    char receiverContact[15];

    char status[50];  
    struct Parcel* next;
};

extern struct Parcel* head;

void InsertParcel();
void DisplayParcels();
void SaveToFile();
void trackParcel(int id);

#endif