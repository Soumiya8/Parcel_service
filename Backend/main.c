#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parcel.h"

static void print_usage_error(void) {
    printf("{\"success\":false,\"error\":\"Invalid command or arguments\"}\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage_error();
        return 1;
    }

    LoadFromFile();

    if (strcmp(argv[1], "add") == 0) {
        if (argc != 12) {
            print_usage_error();
            FreeParcels();
            return 1;
        }

        addParcel(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7],
                  argv[8], argv[9], argv[10], atoi(argv[11]));
    } else if (strcmp(argv[1], "track") == 0) {
        if (argc != 3) {
            print_usage_error();
            FreeParcels();
            return 1;
        }

        searchParcel(argv[2]);
    } else if (strcmp(argv[1], "update") == 0) {
        if (argc != 4) {
            print_usage_error();
            FreeParcels();
            return 1;
        }

        updateStatus(argv[2], argv[3]);
    } else if (strcmp(argv[1], "failed") == 0) {
        if (argc != 3) {
            print_usage_error();
            FreeParcels();
            return 1;
        }

        handleFailedDelivery(atoi(argv[2]));
    } else if (strcmp(argv[1], "list") == 0) {
        char *date = argc >= 3 ? argv[2] : "all";
        char *location = argc >= 4 ? argv[3] : "all";
        listParcels(date, location);
    } else if (strcmp(argv[1], "filter-location") == 0) {
        if (argc != 3) {
            print_usage_error();
            FreeParcels();
            return 1;
        }

        filterByLocationValue(argv[2]);
    } else if (strcmp(argv[1], "filter-date") == 0) {
        if (argc != 3) {
            print_usage_error();
            FreeParcels();
            return 1;
        }

        filterByDateValue(argv[2]);
    } else if (strcmp(argv[1], "stats") == 0) {
        printStats();
    } else {
        print_usage_error();
        FreeParcels();
        return 1;
    }

    FreeParcels();
    return 0;
}
