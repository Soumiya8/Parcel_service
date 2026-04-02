#include "parcel.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: No command provided\n");
        return 1;
    }

    /* Module 1: Add Parcel */
    if (strcmp(argv[1], "add") == 0) {
        if (argc != 8) {
            printf("Error: Invalid arguments for add\n");
            return 1;
        }

        addParcel(
            argv[2],  /* sender name */
            argv[3],  /* sender address */
            argv[4],  /* sender phone */
            argv[5],  /* receiver name */
            argv[6],  /* receiver address */
            argv[7]   /* receiver phone */
        );
    }

    /* Module 2: Track Parcel */
    else if (strcmp(argv[1], "track") == 0) {
        if (argc != 3) {
            printf("Error: Invalid arguments for track\n");
            return 1;
        }

        searchParcel(argv[2]);   /* tracking id */
    }

    /* Module 3: Update Status */
    else if (strcmp(argv[1], "update") == 0) {
        if (argc != 4) {
            printf("Error: Invalid arguments for update\n");
            return 1;
        }

        updateStatus(
            argv[2],   /* tracking id */
            argv[3]    /* new status */
        );
    }

    /* Module 4: List Parcels */
    else if (strcmp(argv[1], "list") == 0) {
        if (argc != 4) {
            printf("Error: Invalid arguments for list\n");
            return 1;
        }

        listParcels(
            argv[2],   /* date */
            argv[3]    /* location */
        );
    }

    else {
        printf("Error: Unknown command\n");
        return 1;
    }

    return 0;
}