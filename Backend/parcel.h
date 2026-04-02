#ifndef PARCEL_H
#define PARCEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// COMMON CONSTANTS

#define DATA_FILE "data.txt"
#define TEMP_FILE "temp.txt"

#define MAX_LINE 512
#define MAX_ID 20
#define MAX_NAME 50
#define MAX_ADDRESS 100
#define MAX_PHONE 15
#define MAX_STATUS 30
#define MAX_LOCATION 50
#define MAX_DATE 20
#define INITIAL_CAPACITY 5

/* -----------------------------------
   COMMON DATA FORMAT
----------------------------------- */
/*
Each parcel is stored as ONE LINE in data.txt

Format:
tracking_id|sender_name|sender_address|sender_phone|
receiver_name|receiver_address|receiver_phone|
status|location|date

Example:
CHN0001|Ali|Chennai|9876543210|Rahul|Bangalore|9123456780|Dispatched|Origin Office|01-04-2026
*/

/* -----------------------------------
   STRUCTURES
----------------------------------- */

/* Stores details of one person */
typedef struct {
    char name[MAX_NAME];
    char address[MAX_ADDRESS];
    char phone[MAX_PHONE];
} Person;

/* Stores complete parcel details */
typedef struct {
    char tracking_id[MAX_ID];
    Person sender;
    Person receiver;
    char status[MAX_STATUS];
    char location[MAX_LOCATION];
    char date[MAX_DATE];
} Parcel;

/* Used in Module 4 for storing multiple filtered results */
typedef struct {
    Parcel *items;
    int size;
    int capacity;
} ParcelList;

/* -----------------------------------
   MODULE 1 : CREATION & STORAGE
----------------------------------- */
void generateTrackingID(char *tracking_id);
void getCurrentDate(char *date);
int countRecords(void);
void addParcel(char *sname, char *saddr, char *sphone,
               char *rname, char *raddr, char *rphone);

/* -----------------------------------
   MODULE 2 : TRACKING
----------------------------------- */
void searchParcel(char *tracking_id);

/* -----------------------------------
   MODULE 3 : STATUS UPDATE
----------------------------------- */
int getStatusLevel(const char *status);
int validateStatusTransition(const char *oldStatus, const char *newStatus);
void updateLocationBasedOnStatus(char *location, const char *status);
void triggerNotification(const char *status);
void updateStatus(char *tracking_id, char *new_status);

/* -----------------------------------
   MODULE 4 : FILTERING & RECORDS
----------------------------------- */
void initParcelList(ParcelList *list);
void addToParcelList(ParcelList *list, Parcel p);
void freeParcelList(ParcelList *list);
void listParcels(char *date, char *location);

/* -----------------------------------
   COMMON UTILITY FUNCTIONS
----------------------------------- */
void trimNewline(char *str);
int isEmpty(const char *str);
void parseParcelLine(char *line, Parcel *p);
void formatParcelLine(const Parcel *p, char *line);

#endif