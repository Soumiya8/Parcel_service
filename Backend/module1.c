#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "parcel.h"

Parcel *head = NULL;
int currentID = 1000;

static void CopyField(char *dest, size_t size, const char *src) {
    if (src == NULL) {
        src = "";
    }
    snprintf(dest, size, "%s", src);
    CleanField(dest);
}

int IsBlank(const char *value) {
    if (value == NULL) {
        return 1;
    }

    while (*value) {
        if (!isspace((unsigned char)*value)) {
            return 0;
        }
        value++;
    }

    return 1;
}

void CleanField(char *value) {
    int i;

    if (value == NULL) {
        return;
    }

    for (i = 0; value[i] != '\0'; i++) {
        if (value[i] == '|' || value[i] == '\n' || value[i] == '\r') {
            value[i] = ' ';
        }
    }
}

void JsonEscapePrint(const char *value) {
    const unsigned char *p = (const unsigned char *)(value ? value : "");

    putchar('"');
    while (*p) {
        if (*p == '"' || *p == '\\') {
            putchar('\\');
            putchar(*p);
        } else if (*p == '\n') {
            printf("\\n");
        } else if (*p == '\r') {
            printf("\\r");
        } else if (*p == '\t') {
            printf("\\t");
        } else if (*p >= 32) {
            putchar(*p);
        }
        p++;
    }
    putchar('"');
}

static void EnsureDataFiles(void) {
    const char *files[] = { DATA_FILE, USERS_FILE, HISTORY_FILE, NOTIFICATIONS_FILE };
    int i;

    for (i = 0; i < 4; i++) {
        FILE *fp = fopen(files[i], "a");
        if (fp != NULL) {
            fclose(fp);
        }
    }
}

static void CurrentDateTime(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    if (local == NULL) {
        snprintf(buffer, size, "Unknown");
        return;
    }

    strftime(buffer, size, "%Y-%m-%d %H:%M", local);
}

int IsValidIndianPhone(const char *phone) {
    int i;

    if (phone == NULL || strlen(phone) != 13) {
        return 0;
    }

    if (phone[0] != '+' || phone[1] != '9' || phone[2] != '1') {
        return 0;
    }

    for (i = 3; i < 13; i++) {
        if (!isdigit((unsigned char)phone[i])) {
            return 0;
        }
    }

    return 1;
}

void AppendHistory(int trackingID, const char *status, const char *remarks) {
    FILE *fp = fopen(HISTORY_FILE, "a");
    char date[32];
    char cleanStatus[60];
    char cleanRemarks[160];

    if (fp == NULL) {
        return;
    }

    CurrentDateTime(date, sizeof(date));
    CopyField(cleanStatus, sizeof(cleanStatus), status);
    CopyField(cleanRemarks, sizeof(cleanRemarks), remarks);
    fprintf(fp, "%d|%s|%s|%s\n", trackingID, cleanStatus, date, cleanRemarks);
    fclose(fp);
}

void AppendNotification(Parcel *parcel, const char *message) {
    FILE *fp = fopen(NOTIFICATIONS_FILE, "a");
    char date[32];
    char cleanMessage[180];

    if (fp == NULL || parcel == NULL) {
        return;
    }

    CurrentDateTime(date, sizeof(date));
    CopyField(cleanMessage, sizeof(cleanMessage), message);
    fprintf(fp, "%d|%s|%s|%s|%s\n",
            parcel->trackingID,
            parcel->receiverContact,
            parcel->senderContact,
            cleanMessage,
            date);
    fclose(fp);
}

void PrintParcelHistoryJson(int trackingID) {
    FILE *fp = fopen(HISTORY_FILE, "r");
    char line[500];
    int first = 1;

    printf("[");
    if (fp == NULL) {
        printf("]");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        int id = 0;
        char status[60] = "";
        char date[32] = "";
        char remarks[180] = "";
        int result = sscanf(line, "%d|%59[^|]|%31[^|]|%179[^\n]", &id, status, date, remarks);

        if (result >= 3 && id == trackingID) {
            if (!first) {
                printf(",");
            }
            printf("{\"status\":");
            JsonEscapePrint(status);
            printf(",\"date\":");
            JsonEscapePrint(date);
            printf(",\"remarks\":");
            JsonEscapePrint(result == 4 ? remarks : "");
            printf("}");
            first = 0;
        }
    }

    fclose(fp);
    printf("]");
}

void PrintParcelJson(Parcel *parcel) {
    printf("{\"trackingID\":%d,\"senderName\":", parcel->trackingID);
    JsonEscapePrint(parcel->senderName);
    printf(",\"senderAddress\":");
    JsonEscapePrint(parcel->senderAddress);
    printf(",\"senderContact\":");
    JsonEscapePrint(parcel->senderContact);
    printf(",\"receiverName\":");
    JsonEscapePrint(parcel->receiverName);
    printf(",\"receiverAddress\":");
    JsonEscapePrint(parcel->receiverAddress);
    printf(",\"receiverContact\":");
    JsonEscapePrint(parcel->receiverContact);
    printf(",\"status\":");
    JsonEscapePrint(parcel->status);
    printf(",\"destinationOffice\":");
    JsonEscapePrint(parcel->destinationOffice);
    printf(",\"deliveryDate\":");
    JsonEscapePrint(parcel->deliveryDate);
    printf(",\"roadRoute\":");
    JsonEscapePrint(parcel->roadRoute);
    printf(",\"driverName\":");
    JsonEscapePrint(parcel->driverName);
    printf(",\"priority\":%d,\"failedAttempts\":%d}", parcel->priority, parcel->failedAttempts);
}

Parcel *FindParcel(int id) {
    Parcel *temp = head;

    while (temp != NULL) {
        if (temp->trackingID == id) {
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}

void assignDriverByRoute(Parcel *parcel) {
    Parcel *temp = head;
    char routeUpper[50];
    int i;

    while (temp != NULL) {
        if (strcmp(temp->roadRoute, parcel->roadRoute) == 0 &&
            strcmp(temp->status, "Delivered") != 0 &&
            !IsBlank(temp->driverName)) {
            CopyField(parcel->driverName, sizeof(parcel->driverName), temp->driverName);
            return;
        }
        temp = temp->next;
    }

    CopyField(routeUpper, sizeof(routeUpper), parcel->roadRoute);
    for (i = 0; routeUpper[i] != '\0'; i++) {
        routeUpper[i] = (char)toupper((unsigned char)routeUpper[i]);
    }

    if (strcmp(routeUpper, "OMR") == 0 || strcmp(routeUpper, "OMR ROAD") == 0 || strcmp(routeUpper, "OMR CHENNAI") == 0) {
        CopyField(parcel->driverName, sizeof(parcel->driverName), "Driver Arun");
    } else if (strcmp(routeUpper, "ECR") == 0 || strcmp(routeUpper, "ECR ROAD") == 0) {
        CopyField(parcel->driverName, sizeof(parcel->driverName), "Driver Bala");
    } else if (strcmp(routeUpper, "GST") == 0 || strcmp(routeUpper, "GST ROAD") == 0) {
        CopyField(parcel->driverName, sizeof(parcel->driverName), "Driver Karthik");
    } else if (strcmp(routeUpper, "AVINASHI ROAD") == 0) {
        CopyField(parcel->driverName, sizeof(parcel->driverName), "Driver Naveen");
    } else if (strcmp(routeUpper, "RING ROAD") == 0) {
        CopyField(parcel->driverName, sizeof(parcel->driverName), "Driver Meena");
    } else if (strcmp(routeUpper, "BYPASS ROAD") == 0) {
        CopyField(parcel->driverName, sizeof(parcel->driverName), "Driver Naveen");
    } else {
        CopyField(parcel->driverName, sizeof(parcel->driverName), "Driver Common Route");
    }
}

void LoadFromFile(void) {
    FILE *fp = fopen(DATA_FILE, "r");
    char line[700];

    EnsureDataFiles();

    if (fp == NULL) {
        FILE *legacy = fopen(LEGACY_DATA_FILE, "r");

        if (legacy != NULL) {
            fp = fopen(DATA_FILE, "w");
            if (fp != NULL) {
                while (fgets(line, sizeof(line), legacy) != NULL) {
                    fputs(line, fp);
                }
                fclose(fp);
            }
            fclose(legacy);
        }
        fp = fopen(DATA_FILE, "r");
    }

    if (fp == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        Parcel *temp = (Parcel *)calloc(1, sizeof(Parcel));
        int result;

        if (temp == NULL) {
            fclose(fp);
            return;
        }

        result = sscanf(line,
            "%d|%49[^|]|%99[^|]|%19[^|]|%49[^|]|%99[^|]|%19[^|]|%59[^|]|%49[^|]|%19[^|]|%49[^|]|%49[^|]|%d|%d",
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
            &temp->failedAttempts);

        if (result == 8) {
            CopyField(temp->destinationOffice, sizeof(temp->destinationOffice), "Not Updated");
            CopyField(temp->deliveryDate, sizeof(temp->deliveryDate), "Not Updated");
            CopyField(temp->roadRoute, sizeof(temp->roadRoute), "General");
            CopyField(temp->driverName, sizeof(temp->driverName), "Driver Common Route");
            temp->priority = 1;
            temp->failedAttempts = 0;
        } else if (result != 14) {
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
}

void SaveToFile(void) {
    FILE *fp = fopen(DATA_FILE, "w");
    Parcel *temp = head;

    if (fp == NULL) {
        return;
    }

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
}

void FreeParcels(void) {
    Parcel *temp = head;

    while (temp != NULL) {
        Parcel *next = temp->next;
        free(temp);
        temp = next;
    }

    head = NULL;
}

void addParcel(char senderName[], char senderAddress[], char senderContact[],
               char receiverName[], char receiverAddress[], char receiverContact[],
               char destinationOffice[], char deliveryDate[], char roadRoute[],
               int priority) {
    Parcel *parcel;

    if (IsBlank(senderName) || IsBlank(senderAddress) || IsBlank(senderContact) ||
        IsBlank(receiverName) || IsBlank(receiverAddress) || IsBlank(receiverContact) ||
        IsBlank(destinationOffice) || IsBlank(deliveryDate) || IsBlank(roadRoute)) {
        printf("{\"success\":false,\"error\":\"All parcel fields are required\"}\n");
        return;
    }

    if (!IsValidIndianPhone(senderContact) || !IsValidIndianPhone(receiverContact)) {
        printf("{\"success\":false,\"error\":\"Phone number must be +91 followed by 10 digits\"}\n");
        return;
    }

    if (!isRouteValidForOffice(destinationOffice, roadRoute)) {
        printf("{\"success\":false,\"error\":\"Selected route is not valid for the destination office\"}\n");
        return;
    }

    parcel = (Parcel *)calloc(1, sizeof(Parcel));
    if (parcel == NULL) {
        printf("{\"success\":false,\"error\":\"Memory allocation failed\"}\n");
        return;
    }

    parcel->trackingID = currentID++;
    CopyField(parcel->senderName, sizeof(parcel->senderName), senderName);
    CopyField(parcel->senderAddress, sizeof(parcel->senderAddress), senderAddress);
    CopyField(parcel->senderContact, sizeof(parcel->senderContact), senderContact);
    CopyField(parcel->receiverName, sizeof(parcel->receiverName), receiverName);
    CopyField(parcel->receiverAddress, sizeof(parcel->receiverAddress), receiverAddress);
    CopyField(parcel->receiverContact, sizeof(parcel->receiverContact), receiverContact);
    CopyField(parcel->status, sizeof(parcel->status), "Dispatched");
    CopyField(parcel->destinationOffice, sizeof(parcel->destinationOffice), destinationOffice);
    CopyField(parcel->deliveryDate, sizeof(parcel->deliveryDate), deliveryDate);
    CopyField(parcel->roadRoute, sizeof(parcel->roadRoute), roadRoute);
    parcel->priority = priority == 2 ? 2 : 1;
    parcel->failedAttempts = 0;

    assignDriverByRoute(parcel);
    parcel->next = head;
    head = parcel;
    SaveToFile();
    AppendHistory(parcel->trackingID, parcel->status, "Parcel created");

    printf("{\"success\":true,\"message\":\"Parcel created\",\"parcel\":");
    PrintParcelJson(parcel);
    printf("}\n");
}

void InsertParcel(void) {
    printf("{\"success\":false,\"error\":\"Use the web form or CLI add command\"}\n");
}

void loginSystem(void) {
    printf("{\"success\":true,\"message\":\"Login simulation skipped for API mode\"}\n");
}
