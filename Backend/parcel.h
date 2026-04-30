#ifndef PARCEL_H
#define PARCEL_H

#include <stdio.h>

#define DATA_FILE "data.txt"
#define MAX_FIELD 100

typedef struct Parcel {
    int trackingID;
    char senderName[50];
    char senderAddress[100];
    char senderContact[20];
    char receiverName[50];
    char receiverAddress[100];
    char receiverContact[20];
    char status[60];
    char destinationOffice[50];
    char deliveryDate[20];
    char roadRoute[50];
    char driverName[50];
    int priority;
    int failedAttempts;
    struct Parcel *next;
} Parcel;

extern Parcel *head;
extern int currentID;

void LoadFromFile(void);
void SaveToFile(void);
void FreeParcels(void);
int IsBlank(const char *value);
void CleanField(char *value);
void JsonEscapePrint(const char *value);
void PrintParcelJson(Parcel *parcel);
Parcel *FindParcel(int id);
void assignDriverByRoute(Parcel *parcel);

void addParcel(char senderName[], char senderAddress[], char senderContact[],
               char receiverName[], char receiverAddress[], char receiverContact[],
               char destinationOffice[], char deliveryDate[], char roadRoute[],
               int priority);

void searchParcel(char trackingID[]);
void updateStatus(char trackingID[], char newStatus[]);
void handleFailedDelivery(int id);
void notifySenderReceiver(Parcel *parcel, char message[]);
void listParcels(char date[], char location[]);
void filterByLocationValue(char location[]);
void filterByDateValue(char date[]);
void printStats(void);

/* Simple interactive names kept for beginner C menu compatibility. */
void InsertParcel(void);
void DisplayParcels(void);
void trackParcel(int id);
void updateParcelStatus(void);
void filterByLocation(void);
void filterByDate(void);
void filterByLocationAndDate(void);
void loginSystem(void);

#endif
