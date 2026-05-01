#include <string.h>
#include <stdlib.h>
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

RouteNode *createRouteNode(const char *name) {
    RouteNode *node = (RouteNode *)calloc(1, sizeof(RouteNode));

    if (node == NULL) {
        return NULL;
    }

    snprintf(node->name, sizeof(node->name), "%s", name ? name : "");
    CleanField(node->name);
    return node;
}

void addChild(RouteNode *parent, RouteNode *child) {
    RouteNode *temp;

    if (parent == NULL || child == NULL) {
        return;
    }

    if (parent->firstChild == NULL) {
        parent->firstChild = child;
        return;
    }

    temp = parent->firstChild;
    while (temp->nextSibling != NULL) {
        temp = temp->nextSibling;
    }
    temp->nextSibling = child;
}

RouteNode *buildRouteTree(void) {
    RouteNode *india = createRouteNode("India");
    RouteNode *tamilNadu = createRouteNode("Tamil Nadu");
    RouteNode *chennai = createRouteNode("Chennai");
    RouteNode *coimbatore = createRouteNode("Coimbatore");
    RouteNode *madurai = createRouteNode("Madurai");
    RouteNode *trichy = createRouteNode("Trichy");

    addChild(india, tamilNadu);
    addChild(tamilNadu, chennai);
    addChild(tamilNadu, coimbatore);
    addChild(tamilNadu, madurai);
    addChild(tamilNadu, trichy);

    addChild(chennai, createRouteNode("OMR Road"));
    addChild(chennai, createRouteNode("ECR Road"));
    addChild(chennai, createRouteNode("GST Road"));
    addChild(coimbatore, createRouteNode("Avinashi Road"));
    addChild(madurai, createRouteNode("Ring Road"));
    addChild(trichy, createRouteNode("Bypass Road"));

    return india;
}

RouteNode *searchRoute(RouteNode *root, const char *name) {
    RouteNode *found;

    if (root == NULL || name == NULL) {
        return NULL;
    }

    if (_stricmp(root->name, name) == 0) {
        return root;
    }

    found = searchRoute(root->firstChild, name);
    if (found != NULL) {
        return found;
    }

    return searchRoute(root->nextSibling, name);
}

int isRouteValidForOffice(const char *office, const char *routeName) {
    RouteNode *root;
    RouteNode *officeNode;
    RouteNode *route;
    int valid = 0;

    if (IsBlank(office) || IsBlank(routeName)) {
        return 0;
    }

    root = buildRouteTree();
    officeNode = searchRoute(root, office);

    if (officeNode != NULL) {
        route = officeNode->firstChild;
        while (route != NULL) {
            if (_stricmp(route->name, routeName) == 0) {
                valid = 1;
                break;
            }
            route = route->nextSibling;
        }
    }

    freeRouteTree(root);
    return valid;
}

void suggestRoutesByOffice(char office[]) {
    RouteNode *root;
    RouteNode *officeNode;
    RouteNode *route;
    int first = 1;

    if (IsBlank(office)) {
        printf("{\"success\":false,\"error\":\"Office is required\"}\n");
        return;
    }

    root = buildRouteTree();
    officeNode = searchRoute(root, office);

    printf("{\"success\":true,\"office\":");
    JsonEscapePrint(office);
    printf(",\"routes\":[");

    if (officeNode != NULL) {
        route = officeNode->firstChild;
        while (route != NULL) {
            if (!first) {
                printf(",");
            }
            JsonEscapePrint(route->name);
            first = 0;
            route = route->nextSibling;
        }
    }

    printf("]}\n");
    freeRouteTree(root);
}

void freeRouteTree(RouteNode *root) {
    if (root == NULL) {
        return;
    }

    freeRouteTree(root->firstChild);
    freeRouteTree(root->nextSibling);
    free(root);
}
