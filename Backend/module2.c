#include <stdlib.h>
#include "parcel.h"

void searchParcel(char trackingID[]) {
    Parcel *parcel;
    int id;

    if (IsBlank(trackingID)) {
        printf("{\"success\":false,\"error\":\"Tracking ID is required\"}\n");
        return;
    }

    id = atoi(trackingID);
    parcel = FindParcel(id);

    if (parcel == NULL) {
        printf("{\"success\":false,\"error\":\"Parcel not found\"}\n");
        return;
    }

    printf("{\"success\":true,\"parcel\":");
    PrintParcelJson(parcel);
    printf("}\n");
}

void trackParcel(int id) {
    char idText[20];
    snprintf(idText, sizeof(idText), "%d", id);
    searchParcel(idText);
}
