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
    char destinationOffice[50];
    char deliveryDate[20];
    char roadRoute[50];
    char driverName[50];

    int priority;
    int failedAttempts;

    struct Parcel *next;
};

extern struct Parcel *head;
extern int currentID;

void InsertParcel();
void SaveToFile();
void LoadFromFile();

void trackParcel(int id);

void updateParcelStatus();
void handleFailedDelivery(int id);
void assignDriverByRoute(struct Parcel *p);
void notifySenderReceiver(struct Parcel *p, char message[]);

void DisplayParcels();
void filterByLocation();
void filterByDate();
void filterByLocationAndDate();

#endif