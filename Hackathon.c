#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>  
#define MAX_STATIONS 100 
#define MAX_LINES 10    
#define INF 1000000     



typedef struct Edge {
    int destination;     // Index of the destination station for this edge, in the stations array
    int cost;            // fare b/w two station
    struct Edge *next;   // pointer to the next edge in the ll
} Edge;


typedef struct Station { // this struct defines one station in the metro network
    char name[50];       // station name
    Edge *edges;         // pointer to a linked list of edges representing all connected stations from this station
    int lineCount;       // a counter for no of lines that pass through this station
    char lines[MAX_LINES][50];  // array of name of lines
} Station;


typedef struct Line {
    char name[50];       // Name of the line
    int startStation;    // Index of the start station for this line
    int endStation;      // Index of the end station for this line
} Line;

// Define a struct for the entire metro network
typedef struct MetroNetwork {
    Station stations[MAX_STATIONS];  // Array of all stations in the network
    int stationCount;                // Current count of stations in the network
    Line lines[MAX_LINES];           // Array of all metro lines in the network
    int lineCount;                   // Current count of metro lines in the network
} MetroNetwork;


// Function to get station index by name
int getStationIndex(MetroNetwork *network, const char *name) {
    for (int i = 0; i < network->stationCount; i++) {
        if (strcmp(network->stations[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}


void addStation(MetroNetwork *network, const char *name) {
    if (getStationIndex(network, name) == -1) {
        strcpy(network->stations[network->stationCount++].name, name);
    }
}

// Function to add an edge (route with fare) between stations
void addEdge(MetroNetwork *network, const char *src, const char *dest, int cost) {
    int srcIndex = getStationIndex(network, src);
    int destIndex = getStationIndex(network, dest);

    if (srcIndex == -1 || destIndex == -1) {
        printf("Invalid stations.\n");
        return;
    }

    Edge *newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->destination = destIndex;
    newEdge->cost = cost;
    newEdge->next = network->stations[srcIndex].edges;
    network->stations[srcIndex].edges = newEdge;

    newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->destination = srcIndex;
    newEdge->cost = cost;
    newEdge->next = network->stations[destIndex].edges;
    network->stations[destIndex].edges = newEdge;
}

// Function to add a metro line and its stations
void addLine(MetroNetwork *network, const char *lineName, int numStations, char stationNames[][50]) {
    strcpy(network->lines[network->lineCount].name, lineName);
    network->lines[network->lineCount].startStation = getStationIndex(network, stationNames[0]);
    network->lines[network->lineCount].endStation = getStationIndex(network, stationNames[numStations - 1]);

    for (int i = 0; i < numStations; i++) {
        int stationIndex = getStationIndex(network, stationNames[i]);
        strcpy(network->stations[stationIndex].lines[network->stations[stationIndex].lineCount++], lineName);
    }
    network->lineCount++;
}


void displayRoute(MetroNetwork *network, int path[], int pathIndex, int totalCost) {
    for (int i = 0; i < pathIndex; i++) {       //loop that iterates over the stations in the path array (from 0 to pathIndex - 1).
        printf("%s", network->stations[path[i]].name);
        if (i < pathIndex - 1) printf(" -> ");
    }
    printf(", Cost: %d\n", totalCost);  //prints the total cost of the route
}


void findRoutes(MetroNetwork *network, int src, int dest, int visited[], int path[], int pathIndex, int cost) {
    visited[src] = 1;   //ensures that a station is not revisited in the same path.
    path[pathIndex++] = src;    //Adds the current station (src) to the path array at the current 
    //pathIndex position and then increments pathIndex to prepare for adding the next station.
    if (src == dest) {
        displayRoute(network, path, pathIndex, cost);   //print the current path and its total cost if src=dest
    } else {
        Edge *edge = network->stations[src].edges;  //If the current station is not the destination, it proceeds to explore neighboring stations.
        while (edge) {
            if (!visited[edge->destination]) {  //Checks if the destination of the current edge has not been visited yet. 
                findRoutes(network, edge->destination, dest, visited, path, pathIndex, cost + edge->cost);
            }
            edge = edge->next;  //Moves to the next edge in the list of edges for the current station (src).
        }
    }

    pathIndex--;    //line decrements pathIndex because the current station is no longer part of the active path (it backtracks).
    visited[src] = 0;   //Marks the current station (src) as unvisited in the visited array, allowing it to be visited again if a different route uses it.
}

// Function to display station information
void stationInfo(MetroNetwork *network, const char *stationName) {
    int stationIndex = getStationIndex(network, stationName);
    if (stationIndex == -1) {
        printf("Station not found.\n");
        return;  //Checks if stationIndex is -1, indicating that the station was not found. If so, prints "Station not found." and exits the function early.
    }

    printf("Station Name: %s\n", network->stations[stationIndex].name);
    printf("Lines: ");
    for (int i = 0; i < network->stations[stationIndex].lineCount; i++) {  //Looping occurs here
        printf("%s", network->stations[stationIndex].lines[i]);
        if (i < network->stations[stationIndex].lineCount - 1) printf(", "); 
        printf("\nAdjacent Stations: ");
        
        // Iterate through all edges and print adjacent stations
        Edge *edge = network->stations[stationIndex].edges;
        int first = 1;  // Flag to handle commas between adjacent stations
        while (edge) {
            if (!first) {
                printf(", ");
            }
            printf("%s (%d)", network->stations[edge->destination].name, edge->cost);
            edge = edge->next; //Moves to the next edge in the list of edges
            first = 0;
        }

        printf("\n");
    }
}


void displayIntersections(MetroNetwork *network) {
    for (int i = 0; i < network->stationCount; i++) {  
        if (network->stations[i].lineCount > 1) {
            printf("%s, %d\n", network->stations[i].name, network->stations[i].lineCount);
        } // iterates through all station, if each station has more than one line going through it , it is an intersection
    }
}


// Function to display terminal stations for each line
void displayTerminalStations(MetroNetwork *network) { //pointer to structure network 
    for (int i = 0; i < network->lineCount; i++) { // loops through lines of network
        printf("%s: %s, %s\n", network->lines[i].name,
               network->stations[network->lines[i].startStation].name,//retrieves the name of metro network
               network->stations[network->lines[i].endStation].name);
    }// getting index of current line , uses list to print station array and retrieve name of end station
} 


// func to find the nearest intersection for a given station
void findNearestIntersection(MetroNetwork *network, int stationIndex) {
    int queue[MAX_STATIONS], front = 0, rear = 0; // first we create a queue to store the stations to enable bfs
    int visited[MAX_STATIONS] = {0}; // we initial all elementes of the visited array to 0 to show that none are visited
    int cost[MAX_STATIONS];
    int parent[MAX_STATIONS]; // making an array of parent stations to enable backtracking
    
    for (int i = 0; i < MAX_STATIONS; i++) cost[i] = INF; // inits all stations to infity except for stating station, signifies that all stations are unreachable when first starting

    queue[rear++] = stationIndex; // add the starting station to the queue 
    // setting rear as 1 as we have added one station to the queue
    visited[stationIndex] = 1; // visited is set to non zero 
    cost[stationIndex] = 0; // cost is set to 0 as we are already at the station
    parent[stationIndex] = -1; // set to -1 as there is no parent for initial station

    while (front < rear) {
        int current = queue[front++]; // hence int current = stationIndex for first iteration

        if (network->stations[current].lineCount > 1 && current != stationIndex) {
            printf("Path to nearest intersection: "); // we found intersection
            // now we start backtracking to find the path to the intersection
            // so we can display it
            
            int path[MAX_STATIONS]; // we initialize path to store station indices along the path
            int pathLength = 0; // we initialize pathLength to store the length of the path
            int temp = current;
            // using the below while loop, we backtrack to find path from intersection to the station
            while (temp != -1) {
                path[pathLength++] = temp; // we store the station index in the path array
                temp = parent[temp];
            }
            // we print the path in reverse order to get the correct path
            for (int i = pathLength - 1; i >= 0; i--) {
                printf("%s", network->stations[path[i]].name); // printing statoin name
                if (i > 0) printf(" -> "); // beautification
            }
            printf(", Cost: %d\n", cost[current]);
            return;
        }

        Edge *edge = network->stations[current].edges;
        // process all stations connected to current station and add them to queue as is procedure in bsf
        while (edge) {
            int next = edge->destination;
            // below if checks if the station is visited or not, or if shorter route is found
            // which means current cost+edge cost < 
            if (!visited[next] || cost[next] > cost[current] + edge->cost) {
                queue[rear++] = next;
                visited[next] = 1;
                parent[next] = current;
                cost[next] = cost[current] + edge->cost;
            }
            edge = edge->next;
        }
    }
    
    printf("No intersection found.\n");
}

int main() {
    MetroNetwork network = { .stationCount = 0, .lineCount = 0 }; // init metronetwork
    int n, m; 
    char src[50], dest[50], lineName[50]; // assuming name wont be longer than 50
    int fare, numStations;

    printf("Enter the number of edges: "); // taking user input
    // the rest of the main function is self explanatory
    // simply taking input and putting them in the respectively appropriate locations
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        printf("Enter source station, destination station, and fare: ");
        scanf("%s %s %d", src, dest, &fare);
        addStation(&network, src);
        addStation(&network, dest);
        addEdge(&network, src, dest, fare);
    }

    printf("Enter the number of lines: ");
    scanf("%d", &m);
    for (int i = 0; i < m; i++) {
        printf("Enter line name: ");
        scanf("%s", lineName);
        printf("Enter the number of stations on this line: ");
        scanf("%d", &numStations);
        char stationNames[numStations][50];
        for (int j = 0; j < numStations; j++) {
            printf("Enter station %d name: ", j + 1);
            scanf("%s", stationNames[j]);
            addStation(&network, stationNames[j]);
        }
        addLine(&network, lineName, numStations, stationNames);
    }

    char command[50];
    // while loop to let user use the functionality of the code 
    while (1) {
        printf("\nEnter command (find_routes, station_info, display_intersections, display_terminal_stations, find_path_to_nearest_intersection, or exit): ");
        scanf("%s", command);
        
        if (strcmp(command, "find_routes") == 0) {
            printf("Enter source and destination stations: ");
            scanf("%s %s", src, dest);
            int srcIndex = getStationIndex(&network, src);
            int destIndex = getStationIndex(&network, dest);
            if (srcIndex == -1 || destIndex == -1) {
                printf("One or both stations not found in the network.\n");
            } 
            else {
                int visited[MAX_STATIONS] = {0}; // init visited to zero for the findRoutes function
                int path[MAX_STATIONS];
                printf("Possible routes:\n");
                findRoutes(&network, srcIndex, destIndex, visited, path, 0, 0);
            }
        } 
        
        else if (strcmp(command, "station_info") == 0) {
            printf("Enter station name: ");
            scanf("%s", src);
            stationInfo(&network, src);
        } 
        
        else if (strcmp(command, "display_intersections") == 0) {
            displayIntersections(&network);
        } 
        
        else if (strcmp(command, "display_terminal_stations") == 0) {
            displayTerminalStations(&network);
        } 
        
        else if (strcmp(command, "find_path_to_nearest_intersection") == 0) {
            printf("Enter station name: ");
            scanf("%s", src);
            int stationIndex = getStationIndex(&network, src);
            if (stationIndex == -1) {
                printf("Station not found.\n");
            } 
            
            else {
                findNearestIntersection(&network, stationIndex);
            }
        } 
        
        else if (strcmp(command, "exit") == 0) {
            break;
        } 
        
        
        
        else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}
