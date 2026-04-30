#include <string.h>
#include "parcel.h"

static int MatchesFilter(Parcel *parcel, const char *date, const char *location) {
    int dateOk = IsBlank(date) || strcmp(date, "all") == 0 || strcmp(parcel->deliveryDate, date) == 0;
    int locationOk = IsBlank(location) || strcmp(location, "all") == 0 ||
                     strcmp(parcel->destinationOffice, location) == 0;

    return dateOk && locationOk;
}

static void PrintParcelArray(const char *date, const char *location) {
    Parcel *temp = head;
    int first = 1;
    int count = 0;

    printf("{\"success\":true,\"parcels\":[");
    while (temp != NULL) {
        if (MatchesFilter(temp, date, location)) {
            if (!first) {
                printf(",");
            }
            PrintParcelJson(temp);
            first = 0;
            count++;
        }
        temp = temp->next;
    }
    printf("],\"count\":%d}\n", count);
}

void listParcels(char date[], char location[]) {
    PrintParcelArray(date, location);
}

void filterByLocationValue(char location[]) {
    if (IsBlank(location)) {
        printf("{\"success\":false,\"error\":\"Location is required\"}\n");
        return;
    }

    PrintParcelArray("all", location);
}

void filterByDateValue(char date[]) {
    if (IsBlank(date)) {
        printf("{\"success\":false,\"error\":\"Date is required\"}\n");
        return;
    }

    PrintParcelArray(date, "all");
}

void printStats(void) {
    Parcel *temp = head;
    int total = 0;
    int delivered = 0;
    int active = 0;
    int failed = 0;
    int urgent = 0;
    int escalated = 0;

    while (temp != NULL) {
        total++;
        if (strcmp(temp->status, "Delivered") == 0) {
            delivered++;
        } else {
            active++;
        }

        if (strstr(temp->status, "Failed") != NULL ||
            strcmp(temp->status, "Office Hold") == 0 ||
            strcmp(temp->status, "Emergency Escalation") == 0) {
            failed++;
        }

        if (strcmp(temp->status, "Emergency Escalation") == 0) {
            escalated++;
        }

        if (temp->priority == 2) {
            urgent++;
        }

        temp = temp->next;
    }

    printf("{\"success\":true,\"stats\":{\"total\":%d,\"active\":%d,\"delivered\":%d,\"failed\":%d,\"urgent\":%d,\"emergencyEscalations\":%d}}\n",
           total, active, delivered, failed, urgent, escalated);
}

void DisplayParcels(void) {
    listParcels("all", "all");
}

void filterByLocation(void) {
    printf("{\"success\":false,\"error\":\"Use filter-location command\"}\n");
}

void filterByDate(void) {
    printf("{\"success\":false,\"error\":\"Use filter-date command\"}\n");
}

void filterByLocationAndDate(void) {
    printf("{\"success\":false,\"error\":\"Use list date location command\"}\n");
}
