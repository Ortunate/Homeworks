#include <bits/stdc++.h>
using namespace std;

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define MAX_VERTEX_NUM 20

typedef int status;
typedef int KeyType;
typedef enum
{
    DG,  // Directed Graph
    DN,  // Directed Network
    UDG, // Undirected Graph
    UDN  // Undirected Network
} GraphKind;

typedef struct
{
    KeyType key;
    char others[20];
} VertexType; // Vertex data type definition

typedef struct ArcNode
{                            // Adjacency list node structure
    int adjvex;              // Adjacent vertex index
    struct ArcNode *nextarc; // Pointer to next arc node
} ArcNode;

typedef struct VNode
{                      // Vertex node structure
    VertexType data;   // Vertex data
    ArcNode *firstarc; // Pointer to first adjacent arc
} VNode, AdjList[MAX_VERTEX_NUM];

typedef struct
{                       // Adjacency list graph structure
    AdjList vertices;   // Array of vertex nodes
    int vexnum, arcnum; // Number of vertices and arcs
    GraphKind kind;     // Graph type
} ALGraph;

static int duplicated(VertexType V[], int &n)
// Checks for duplicate vertex keys and counts vertices
{
    unordered_set<KeyType> keys;
    for (int i = 0; V[i].key != -1; i++)
    {
        if (keys.find(V[i].key) != keys.end())
            return TRUE; // Duplicate found
        keys.insert(V[i].key);
    }
    n = keys.size();
    return FALSE; // No duplicates
}

status CreateCraph(ALGraph &G, VertexType V[], KeyType VR[][2])
// Creates graph from vertex array V and edge array VR
{
    if (V[0].key == -1 || (V[1].key == -1 && VR[0][0] != -1) || duplicated(V, G.vexnum))
        return ERROR; // Invalid input
    if (G.vexnum > MAX_VERTEX_NUM)
        return ERROR;

    G.arcnum = 0;
    G.kind = UDG;
    unordered_map<KeyType, int> dic; // Key to index mapping

    // Initialize vertices
    for (int i = 0; i < G.vexnum; i++)
    {
        dic[V[i].key] = i;
        G.vertices[i].data = V[i];
        G.vertices[i].firstarc = NULL;
    }

    // Count edges
    while (VR[G.arcnum][0] != -1)
        G.arcnum++;

    // Create adjacency lists for undirected graph
    for (int i = 0; i < G.arcnum; i++)
    {
        if (VR[i][0] == VR[i][1]) // Skip self-loops
            continue;
        KeyType v1 = VR[i][0], v2 = VR[i][1];
        int j, k, flg = 0;
        if (dic.find(v1) == dic.end() || dic.find(v2) == dic.end())
            return ERROR; // Invalid edge vertices
        j = dic[v1], k = dic[v2];

        // Check for duplicate edges
        for (ArcNode *p = G.vertices[j].firstarc; p != NULL; p = p->nextarc)
            if (p->adjvex == k)
            {
                flg = 1; // Duplicate edge found
                break;
            }
        if (flg)
            continue;

        // Add edge in both directions (undirected)
        ArcNode *p1 = new ArcNode, *p2 = new ArcNode;
        p1->adjvex = k;
        p1->nextarc = G.vertices[j].firstarc;
        G.vertices[j].firstarc = p1;
        p2->adjvex = j;
        p2->nextarc = G.vertices[k].firstarc;
        G.vertices[k].firstarc = p2;
    }
    return OK;
}

status DestroyGraph(ALGraph &G)
// Destroys graph and frees all memory
{
    for (int i = 0; i < G.vexnum; i++)
    {
        ArcNode *p = G.vertices[i].firstarc;
        while (p != NULL)
        {
            ArcNode *q = p;
            p = p->nextarc;
            delete q;
        }
        G.vertices[i].firstarc = NULL;
    }
    G.vexnum = 0;
    G.arcnum = 0;
    return OK;
}

int LocateVex(ALGraph G, KeyType u)
// Finds vertex position by key, returns index or -1 if not found
{
    for (int i = 0; i < G.vexnum; i++)
        if (G.vertices[i].data.key == u)
            return i;
    return -1;
}

status PutVex(ALGraph &G, KeyType u, VertexType value)
// Updates vertex data, checks for key conflicts
{
    int i = LocateVex(G, u);
    if (i == -1)
        return ERROR;
    int j = LocateVex(G, value.key);
    if (j != -1 && j != i) // Key already exists in another vertex
        return ERROR;
    G.vertices[i].data = value;
    return OK;
}

int FirstAdjVex(ALGraph G, KeyType u)
// Returns index of first adjacent vertex, -1 if none exists
{
    int i = LocateVex(G, u);
    if (i == -1 || G.vertices[i].firstarc == NULL)
        return -1;
    return G.vertices[i].firstarc->adjvex;
}

int NextAdjVex(ALGraph G, KeyType v, KeyType w)
// Returns next adjacent vertex after w for vertex v
{
    int i = LocateVex(G, v), j = LocateVex(G, w);
    if (i == -1 || j == -1)
        return -1;
    ArcNode *p = G.vertices[i].firstarc;
    while (p != NULL && p->adjvex != j)
        p = p->nextarc;
    if (p == NULL || p->nextarc == NULL)
        return -1; // w is the last adjacent vertex
    return p->nextarc->adjvex;
}

status InsertVex(ALGraph &G, VertexType v)
// Inserts new vertex into graph
{
    if (G.vexnum == MAX_VERTEX_NUM || LocateVex(G, v.key) != -1)
        return ERROR;
    G.vertices[G.vexnum].data = v;
    G.vertices[G.vexnum++].firstarc = NULL;
    return OK;
}

status DeleteVex(ALGraph &G, KeyType v)
// Deletes vertex and all related edges
{
    int i = LocateVex(G, v);
    if (i == -1 || G.vexnum <= 1)
        return ERROR;

    // Delete all edges from this vertex
    ArcNode *p = G.vertices[i].firstarc;
    while (p != NULL)
    {
        ArcNode *q = p;
        p = p->nextarc;
        delete q;
        G.arcnum--;
    }

    // Shift vertices array to fill gap
    for (int j = i; j < G.vexnum - 1; j++)
        G.vertices[j] = G.vertices[j + 1];
    G.vexnum--;

    // Update adjacency lists to remove references to deleted vertex
    for (int j = 0; j < G.vexnum; j++)
    {
        ArcNode *p = G.vertices[j].firstarc, *q = NULL;
        // Remove edges pointing to deleted vertex at head
        while (p && p->adjvex == i)
        {
            ArcNode *temp = p;
            p = p->nextarc;
            delete temp;
        }
        G.vertices[j].firstarc = p;
        q = p;
        // Remove edges pointing to deleted vertex in middle/end
        while (p != NULL)
        {
            if (p->adjvex == i)
            {
                q->nextarc = p->nextarc;
                ArcNode *temp = p;
                p = p->nextarc;
                delete temp;
            }
            else if (p->adjvex > i) // Adjust indices after deletion
            {
                p->adjvex--;
                q = p;
                p = p->nextarc;
            }
            else
            {
                q = p;
                p = p->nextarc;
            }
        }
    }
    return OK;
}

status InsertArc(ALGraph &G, KeyType v, KeyType w)
// Inserts edge between vertices v and w
{
    int i = LocateVex(G, v), j = LocateVex(G, w);
    if (i == -1 || j == -1)
        return ERROR;

    // Check if edge already exists
    ArcNode *p = G.vertices[i].firstarc;
    while (p != NULL)
    {
        if (p->adjvex == j)
            return ERROR; // Duplicate edge
        p = p->nextarc;
    }

    // Add edge in both directions (undirected graph)
    ArcNode *newArc = new ArcNode;
    newArc->adjvex = j;
    newArc->nextarc = G.vertices[i].firstarc;
    G.vertices[i].firstarc = newArc;

    newArc = new ArcNode;
    newArc->adjvex = i;
    newArc->nextarc = G.vertices[j].firstarc;
    G.vertices[j].firstarc = newArc;
    G.arcnum++;
    return OK;
}

static int da(ArcNode **p, int i)
// Helper function to delete arc node with given index
{
    if ((*p)->adjvex == i)
    {
        ArcNode *temp = *p;
        *p = (*p)->nextarc;
        delete temp;
        return 1;
    }
    ArcNode *q = *p, *r = (*p)->nextarc;
    while (r != NULL)
    {
        if (r->adjvex == i)
        {
            q->nextarc = r->nextarc;
            delete r;
            return 1;
        }
        q = r;
        r = r->nextarc;
    }
    return 0; // Edge not found
}

status DeleteArc(ALGraph &G, KeyType v, KeyType w)
// Deletes edge between vertices v and w
{
    int i = LocateVex(G, v), j = LocateVex(G, w);
    if (i == -1 || j == -1)
        return ERROR;
    if (da(&G.vertices[i].firstarc, j) == 0)
        return ERROR;               // Edge not found
    da(&G.vertices[j].firstarc, i); // Remove reverse edge
    G.arcnum--;
    return OK;
}

status DFSTraverse(ALGraph &G, void (*visit)(VertexType))
// Performs depth-first search traversal using iterative approach
{
    if (G.vexnum == 0)
        return ERROR;
    unordered_set<int> visited;

    // Visit all connected components
    for (int j = 0; j < G.vexnum; j++)
    {
        if (visited.find(j) != visited.end())
            continue;
        stack<int> s;
        s.push(j);
        while (!s.empty())
        {
            int i = s.top();
            s.pop();
            if (visited.find(i) == visited.end())
            {
                visit(G.vertices[i].data);
                visited.insert(i);
                // Add adjacent vertices to stack in reverse order
                stack<int> s1;
                for (ArcNode *p = G.vertices[i].firstarc; p != NULL; p = p->nextarc)
                    if (visited.find(p->adjvex) == visited.end())
                        s1.push(p->adjvex);
                while (!s1.empty())
                {
                    s.push(s1.top());
                    s1.pop();
                }
            }
        }
    }
    return OK;
}

status BFSTraverse(ALGraph &G, void (*visit)(VertexType))
// Performs breadth-first search traversal
{
    if (G.vexnum == 0)
        return ERROR;
    unordered_set<int> visited;

    // Visit all connected components
    for (int j = 0; j < G.vexnum; j++)
    {
        if (visited.find(j) != visited.end())
            continue;
        queue<int> q;
        q.push(j);
        while (!q.empty())
        {
            int i = q.front();
            q.pop();
            if (visited.find(i) == visited.end())
            {
                visit(G.vertices[i].data);
                visited.insert(i);
                // Add all unvisited adjacent vertices to queue
                for (ArcNode *p = G.vertices[i].firstarc; p != NULL; p = p->nextarc)
                    if (visited.find(p->adjvex) == visited.end())
                        q.push(p->adjvex);
            }
        }
    }
    return OK;
}

status SaveGraph(ALGraph G, char FileName[])
// Saves graph data to file in adjacency list format
{
    FILE *fp = fopen(FileName, "w");
    if (fp == NULL)
        return ERROR;
    fprintf(fp, "%d %d\n", G.vexnum, G.arcnum);
    for (int i = 0; i < G.vexnum; i++)
    {
        fprintf(fp, "%d %s ", G.vertices[i].data.key, G.vertices[i].data.others);
        ArcNode *p = G.vertices[i].firstarc;
        while (p != NULL)
        {
            fprintf(fp, "%d ", p->adjvex);
            p = p->nextarc;
        }
        fprintf(fp, "-1\n"); // End marker for adjacency list
    }
    fclose(fp);
    return OK;
}

status LoadGraph(ALGraph &G, char FileName[])
// Loads graph from file and creates adjacency list
{
    FILE *fp = fopen(FileName, "r");
    if (fp == NULL)
        return ERROR;
    G.kind = UDG;
    fscanf(fp, "%d %d", &G.vexnum, &G.arcnum);

    for (int i = 0; i < G.vexnum; i++)
    {
        fscanf(fp, "%d %s", &G.vertices[i].data.key, G.vertices[i].data.others);
        G.vertices[i].firstarc = NULL;
        int tmp;
        fscanf(fp, "%d", &tmp);
        if (tmp == -1) // No adjacent vertices
            continue;
        else
        {
            // Create first arc node
            ArcNode *p = new ArcNode;
            p->adjvex = tmp;
            p->nextarc = NULL;
            G.vertices[i].firstarc = p;

            // Read remaining adjacent vertices
            while (fscanf(fp, "%d", &tmp) && tmp != -1)
            {
                ArcNode *q = new ArcNode;
                q->adjvex = tmp;
                q->nextarc = NULL;
                p->nextarc = q;
                p = q;
            }
        }
    }
    fclose(fp);
    return OK;
}

int ConnectedComponent(ALGraph G)
// Counts number of connected components using DFS
{
    if (G.vexnum <= 1)
        return G.vexnum;
    unordered_set<int> visited;
    int count = 0;

    for (int j = 0; j < G.vexnum; j++)
    {
        if (visited.find(j) != visited.end())
            continue;
        count++;
        stack<int> s;
        s.push(j);
        while (!s.empty())
        {
            int i = s.top();
            s.pop();
            if (visited.find(i) == visited.end())
            {
                visited.insert(i);
                for (ArcNode *p = G.vertices[i].firstarc; p != NULL; p = p->nextarc)
                    if (visited.find(p->adjvex) == visited.end())
                        s.push(p->adjvex);
            }
        }
    }
    return count;
}

int **distance(ALGraph G)
// Computes all-pairs shortest distances using Floyd-Warshall algorithm
{
    int **dist = new int *[G.vexnum];
    for (int i = 0; i < G.vexnum; i++)
        dist[i] = new int[G.vexnum];

    // Initialize distance matrix
    for (int i = 0; i < G.vexnum; i++)
        for (int j = 0; j < G.vexnum; j++)
            dist[i][j] = INT_MAX;

    // Set direct distances
    for (int i = 0; i < G.vexnum; i++)
    {
        dist[i][i] = 0; // Distance to self is 0
        ArcNode *p = G.vertices[i].firstarc;
        while (p != NULL)
        {
            dist[i][p->adjvex] = 1; // All edge weights are 1
            p = p->nextarc;
        }
    }

    // Floyd-Warshall algorithm
    for (int k = 0; k < G.vexnum; k++)
        for (int i = 0; i < G.vexnum; i++)
            for (int j = 0; j < G.vexnum; j++)
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
    return dist;
}

VertexType *VerticesSetLessThanK1(ALGraph &G, VertexType v, int k)
// Returns vertices within distance k using Floyd-Warshall
{
    int i = LocateVex(G, v.key);
    if (i == -1)
        return NULL;
    if (G.vertices[i].firstarc == NULL)
        return NULL;
    int **dist = distance(G);
    VertexType *result = new VertexType[G.vexnum];
    int count = 0;
    for (int j = 0; j < G.vexnum; j++)
        if (dist[i][j] <= k)
            result[count++] = G.vertices[j].data;
    result[count].key = -1; // End marker

    // Clean up distance matrix
    for (int j = 0; j < G.vexnum; j++)
        delete[] dist[j];
    delete[] dist;
    return result;
}

VertexType *VerticesSetLessThanK(ALGraph &G, VertexType v, int k)
// Returns vertices within distance k using optimized Dijkstra
{
    int i = LocateVex(G, v.key);
    if (i == -1)
        return NULL;
    VertexType *result = new VertexType[G.vexnum + 1];
    int count = 0;
    vector<int> dist(G.vexnum, INT_MAX);
    vector<bool> visited(G.vexnum, false);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    dist[i] = 0;
    pq.push({0, i});

    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();
        if (visited[u])
            continue;
        visited[u] = true;
        if (dist[u] > k)
            break;

        // Relax adjacent vertices
        for (ArcNode *p = G.vertices[u].firstarc; p != NULL; p = p->nextarc)
        {
            int v_adj = p->adjvex;
            int newDist = dist[u] + 1;

            if (newDist < dist[v_adj])
            {
                dist[v_adj] = newDist;
                if (newDist <= k)
                    pq.push({newDist, v_adj});
            }
        }
    }

    // Collect vertices within distance k
    for (int j = 0; j < G.vexnum; j++)
        if (dist[j] <= k)
            result[count++] = G.vertices[j].data;

    result[count].key = -1; // End marker
    return result;
}

int ShortestPathLen(ALGraph G, VertexType v, VertexType w)
// Computes shortest path length between two vertices
{
    int i = LocateVex(G, v.key);
    int j = LocateVex(G, w.key);
    if (i == -1 || j == -1 || G.vertices[i].firstarc == NULL)
        return ERROR;
    int **dist = distance(G);
    int result = dist[i][j];

    // Clean up distance matrix
    for (int k = 0; k < G.vexnum; k++)
        delete[] dist[k];
    delete[] dist;
    return result;
}

int main(void)
// Main function providing interactive menu for graph operations
{
    vector<pair<string, ALGraph>> graphs(1, {"Default", {}});
    int op = 1;
    int currentGraph = 0; // Index of currently active graph
    VertexType V[MAX_VERTEX_NUM];
    KeyType VR[MAX_VERTEX_NUM * MAX_VERTEX_NUM][2];

    // Lambda function for vertex visiting
    auto visit = [](VertexType vertex)
    {
        printf("%d %s | ", vertex.key, vertex.others);
    };

    while (op)
    {
        // Display menu
        printf("      Menu for Graph Management System \n");
        printf("-----------------------Basic---------------------\n");
        printf("    1. CreateGraph           2. DestroyGraph\n");
        printf("    3. LocateVex             4. PutVex\n");
        printf("    5. FirstAdjVex           6. NextAdjVex\n");
        printf("    7. InsertVex             8. DeleteVex\n");
        printf("    9. InsertArc            10. DeleteArc\n");
        printf("   11. DFSTraverse          12. BFSTraverse\n");
        printf("----------------------Advanced--------------------\n");
        printf("   13. VerticesSetLessThanK  14. ShortestPathLen\n");
        printf("   15. ConnectedComponent    16. SaveGraph\n");
        printf("   17. LoadGraph             18. CreateNewGraph\n");
        printf("   19. RemoveGraph           20. GraphsInfo\n");
        printf("   21. SwitchGraph           22. InputGraphDefinition\n");
        printf("-----------------------Exit----------------------\n");
        printf("    0. Exit\n");
        printf("Current graph: '%s' (index %d)\n", graphs[currentGraph].first.c_str(), currentGraph);
        printf("Please select your operation [0~22]:\n");

        scanf("%d", &op);
        getchar();

        switch (op)
        {
        case 1: // Create graph from definition
        {
            if (V[0].key == -1)
            {
                printf("No graph definition available! Use option 22 to input one first.\n");
                break;
            }

            if (CreateCraph(graphs[currentGraph].second, V, VR) == OK)
            {
                printf("Graph '%s' created successfully!\n", graphs[currentGraph].first.c_str());
                // Reset definition arrays
                memset(V, -1, sizeof(V));
                memset(VR, -1, sizeof(VR));
            }
            else
                printf("Error creating graph! Check for duplicate keys or invalid data.\n");
            break;
        }

        case 2: // Destroy graph
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist or is already empty!\n", graphs[currentGraph].first.c_str());
                break;
            }

            if (DestroyGraph(graphs[currentGraph].second) == OK)
                printf("Graph '%s' destroyed successfully!\n", graphs[currentGraph].first.c_str());
            else
                printf("Failed to destroy graph!\n");
            break;
        }

        case 3: // Locate vertex
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            KeyType key;
            printf("Enter vertex key to locate: ");
            scanf("%d", &key);
            getchar();

            int loc = LocateVex(graphs[currentGraph].second, key);
            if (loc != -1)
                printf("Vertex: %d %s found at position %d!\n", key,
                       graphs[currentGraph].second.vertices[loc].data.others, loc);
            else
                printf("Vertex with key %d not found in graph '%s'!\n", key, graphs[currentGraph].first.c_str());
            break;
        }

        case 4: // Update vertex data
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            KeyType key;
            VertexType value;
            printf("Enter vertex key to update: ");
            scanf("%d", &key);
            printf("Enter new key for the vertex: ");
            scanf("%d", &value.key);
            printf("Enter new data for the vertex: ");
            scanf("%s", value.others);
            getchar();

            if (PutVex(graphs[currentGraph].second, key, value) == OK)
                printf("Vertex with key %d updated to key %d in graph '%s'!\n",
                       key, value.key, graphs[currentGraph].first.c_str());
            else
                printf("Failed to update vertex! Check if key exists or conflicts with another vertex.\n");
            break;
        }

        case 5: // Get first adjacent vertex
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            KeyType key;
            printf("Enter vertex key: ");
            scanf("%d", &key);
            getchar();

            int res = FirstAdjVex(graphs[currentGraph].second, key);
            if (res != -1)
                printf("First adjacent vertex of %d is at position %d with key %d!\n",
                       key, res, graphs[currentGraph].second.vertices[res].data.key);
            else
                printf("Vertex with key %d has no adjacent vertices or doesn't exist!\n", key);
            break;
        }

        case 6: // Get next adjacent vertex
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            KeyType v, w;
            printf("Enter first vertex key: ");
            scanf("%d", &v);
            printf("Enter adjacent vertex key: ");
            scanf("%d", &w);
            getchar();

            int res = NextAdjVex(graphs[currentGraph].second, v, w);
            if (res != -1)
                printf("Next adjacent vertex after %d for vertex %d is at position %d with key %d!\n",
                       w, v, res, graphs[currentGraph].second.vertices[res].data.key);
            else
                printf("No next adjacent vertex found or vertices don't exist!\n");
            break;
        }

        case 7: // Insert vertex
        {
            if (graphs[currentGraph].second.vexnum == MAX_VERTEX_NUM)
            {
                printf("Cannot insert vertex: Graph '%s' is full (max %d vertices)!\n",
                       graphs[currentGraph].first.c_str(), MAX_VERTEX_NUM);
                break;
            }

            VertexType vertex;
            printf("Enter new vertex key: ");
            scanf("%d", &vertex.key);
            printf("Enter new vertex data: ");
            scanf("%s", vertex.others);
            getchar();

            if (InsertVex(graphs[currentGraph].second, vertex) == OK)
                printf("Vertex with key %d inserted successfully into graph '%s'!\n",
                       vertex.key, graphs[currentGraph].first.c_str());
            else
                printf("Failed to insert vertex! Check if key already exists.\n");
            break;
        }

        case 8: // Delete vertex
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            KeyType key;
            printf("Enter key of vertex to delete: ");
            scanf("%d", &key);
            getchar();

            if (DeleteVex(graphs[currentGraph].second, key) == OK)
                printf("Vertex with key %d deleted successfully from graph '%s'!\n",
                       key, graphs[currentGraph].first.c_str());
            else
                printf("Failed to delete vertex! Check if key exists or if it's the only vertex.\n");
            break;
        }

        case 9: // Insert edge
        {
            if (graphs[currentGraph].second.vexnum < 2)
            {
                printf("Graph '%s' needs at least 2 vertices to insert an arc!\n",
                       graphs[currentGraph].first.c_str());
                break;
            }

            KeyType v, w;
            printf("Enter first vertex key: ");
            scanf("%d", &v);
            printf("Enter second vertex key: ");
            scanf("%d", &w);
            getchar();

            if (InsertArc(graphs[currentGraph].second, v, w) == OK)
                printf("Arc between vertices %d and %d inserted successfully!\n", v, w);
            else
                printf("Failed to insert arc! Check if vertices exist or arc already exists.\n");
            break;
        }

        case 10: // Delete edge
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            KeyType v, w;
            printf("Enter first vertex key: ");
            scanf("%d", &v);
            printf("Enter second vertex key: ");
            scanf("%d", &w);
            getchar();

            if (DeleteArc(graphs[currentGraph].second, v, w) == OK)
                printf("Arc between vertices %d and %d deleted successfully!\n", v, w);
            else
                printf("Failed to delete arc! Check if vertices exist or arc exists.\n");
            break;
        }

        case 11: // DFS traversal
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            printf("DFS Traversal of graph '%s':\n", graphs[currentGraph].first.c_str());
            DFSTraverse(graphs[currentGraph].second, visit);
            printf("\n");
            break;
        }

        case 12: // BFSTraverse
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            printf("BFS Traversal of graph '%s':\n", graphs[currentGraph].first.c_str());
            BFSTraverse(graphs[currentGraph].second, visit);
            printf("\n");
            break;
        }

        case 13: // Find vertices within distance k
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            VertexType v;
            int k;
            printf("Enter vertex key: ");
            scanf("%d", &v.key);
            printf("Enter distance k: ");
            scanf("%d", &k);
            getchar();

            int vertexPos = LocateVex(graphs[currentGraph].second, v.key);
            if (vertexPos == -1)
            {
                printf("Vertex with key %d not found!\n", v.key);
                break;
            }

            v = graphs[currentGraph].second.vertices[vertexPos].data;
            VertexType *result = VerticesSetLessThanK1(graphs[currentGraph].second, v, k);

            if (result == NULL)
            {
                printf("No vertices within distance %d of vertex %d!\n", k, v.key);
                break;
            }

            printf("Vertices within distance %d of vertex %d:\n", k, v.key);
            int i = 0;
            while (result[i].key != -1)
            {
                printf("%d %s | ", result[i].key, result[i].others);
                i++;
            }
            printf("\nTotal: %d vertices\n", i);

            delete[] result;
            break;
        }

        case 14: // Calculate shortest path length
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            VertexType v, w;
            printf("Enter first vertex key: ");
            scanf("%d", &v.key);
            printf("Enter second vertex key: ");
            scanf("%d", &w.key);
            getchar();

            int vertexPos1 = LocateVex(graphs[currentGraph].second, v.key);
            int vertexPos2 = LocateVex(graphs[currentGraph].second, w.key);

            if (vertexPos1 == -1 || vertexPos2 == -1)
            {
                printf("One or both vertices not found!\n");
                break;
            }

            v = graphs[currentGraph].second.vertices[vertexPos1].data;
            w = graphs[currentGraph].second.vertices[vertexPos2].data;

            int length = ShortestPathLen(graphs[currentGraph].second, v, w);

            if (length == INT_MAX)
                printf("No path exists between vertices %d and %d!\n", v.key, w.key);
            else if (length == ERROR)
                printf("Error computing shortest path!\n");
            else
                printf("Shortest path length between vertices %d and %d: %d\n", v.key, w.key, length);
            break;
        }

        case 15: // Count connected components
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            int count = ConnectedComponent(graphs[currentGraph].second);
            printf("Graph '%s' has %d connected component(s).\n", graphs[currentGraph].first.c_str(), count);
            break;
        }

        case 16: // Save graph to file
        {
            if (graphs[currentGraph].second.vexnum == 0)
            {
                printf("Graph '%s' doesn't exist!\n", graphs[currentGraph].first.c_str());
                break;
            }

            char filename[100];
            printf("Enter filename to save: ");
            scanf("%s", filename);
            getchar();

            if (SaveGraph(graphs[currentGraph].second, filename) == OK)
                printf("Graph '%s' saved to file '%s' successfully!\n",
                       graphs[currentGraph].first.c_str(), filename);
            else
                printf("Failed to save graph to file!\n");
            break;
        }

        case 17: // Load graph from file
        {
            if (graphs[currentGraph].second.vexnum != 0)
            {
                printf("Graph '%s' already exists! Destroy it first.\n", graphs[currentGraph].first.c_str());
                break;
            }

            char filename[100];
            printf("Enter filename to load: ");
            scanf("%s", filename);
            getchar();

            if (LoadGraph(graphs[currentGraph].second, filename) == OK)
                printf("Graph '%s' loaded from file '%s' successfully!\n",
                       graphs[currentGraph].first.c_str(), filename);
            else
                printf("Failed to load graph from file!\n");
            break;
        }

        case 18: // Create new graph
        {
            char graphName[30];
            printf("Enter name for the new graph: ");
            scanf("%s", graphName);
            getchar();

            // Check for duplicate names
            bool nameExists = false;
            for (int i = 0; i < graphs.size(); i++)
            {
                if (graphs[i].first == graphName)
                {
                    nameExists = true;
                    break;
                }
            }

            if (nameExists)
            {
                printf("Graph '%s' already exists!\n", graphName);
                break;
            }

            ALGraph newGraph;
            newGraph.vexnum = 0;
            newGraph.arcnum = 0;
            graphs.push_back({string(graphName), newGraph});
            printf("New graph '%s' created successfully at index %d!\n", graphName, graphs.size() - 1);
            break;
        }

        case 19: // Remove graph from collection
        {
            if (graphs.size() == 0)
            {
                printf("No graphs to remove!\n");
                break;
            }

            printf("Available graphs:\n");
            for (int i = 0; i < graphs.size(); i++)
            {
                printf("%d. %s%s\n", i, graphs[i].first.c_str(), (i == currentGraph ? " [ACTIVE]" : ""));
            }

            int index;
            printf("Enter index of graph to remove: ");
            scanf("%d", &index);
            getchar();

            if (index < 0 || index >= graphs.size())
            {
                printf("Invalid graph index!\n");
                break;
            }

            DestroyGraph(graphs[index].second);
            graphs.erase(graphs.begin() + index);
            printf("Graph removed successfully!\n");

            // Adjust current graph index if necessary
            if (graphs.empty())
            {
                printf("No graphs left. Create a new graph to continue.\n");
            }
            else if (index == currentGraph)
            {
                printf("You removed the active graph. Switch to another graph to continue.\n");
            }
            break;
        }

        case 20: // Display graph information
        {
            if (graphs.empty())
            {
                printf("No graphs available!\n");
                break;
            }

            printf("Total number of graphs: %d\n", graphs.size());
            printf("Current active graph: '%s' (index %d)\n", graphs[currentGraph].first.c_str(), currentGraph);

            for (int i = 0; i < graphs.size(); i++)
            {
                printf("Graph %d: '%s' - %s", i, graphs[i].first.c_str(),
                       (graphs[i].second.vexnum == 0) ? "Empty" : "Has data");

                if (graphs[i].second.vexnum > 0)
                {
                    printf(" (vertices: %d, arcs: %d)",
                           graphs[i].second.vexnum, graphs[i].second.arcnum);

                    int components = ConnectedComponent(graphs[i].second);
                    printf(", connected components: %d", components);
                }

                if (i == currentGraph)
                    printf(" [ACTIVE]");
                printf("\n");
            }
            break;
        }

        case 21: // Switch active graph
        {
            if (graphs.empty())
            {
                printf("No graphs available!\n");
                break;
            }

            printf("Current graph: '%s' (index %d)\n", graphs[currentGraph].first.c_str(), currentGraph);
            printf("Available graphs:\n");
            for (int i = 0; i < graphs.size(); i++)
            {
                printf("%d. %s%s\n", i, graphs[i].first.c_str(), (i == currentGraph ? " [ACTIVE]" : ""));
            }

            int index;
            printf("Enter graph index to switch to: ");
            scanf("%d", &index);
            getchar();

            if (index >= 0 && index < graphs.size())
            {
                currentGraph = index;
                printf("Switched to graph '%s' at index %d\n", graphs[currentGraph].first.c_str(), currentGraph);
            }
            else
                printf("Invalid graph index!\n");
            break;
        }

        case 22: // Input graph definition manually
        {
            printf("Enter graph vertex definitions (format: key data, -1 to end):\n");
            printf("Example: 1 A 2 B 3 C -1 END\n");

            int i = 0;
            while (1)
            {
                if (scanf("%d %s", &V[i].key, V[i].others) != 2)
                {
                    printf("Invalid input format!\n");
                    break;
                }

                if (V[i].key == -1)
                    break;

                i++;
                if (i >= MAX_VERTEX_NUM)
                {
                    printf("Maximum %d vertices allowed!\n", MAX_VERTEX_NUM);
                    break;
                }
            }

            printf("Enter graph edges (format: vertex1 vertex2, -1 -1 to end):\n");
            printf("Example: 1 2 2 3 1 3 -1 -1\n");

            i = 0;
            while (1)
            {
                if (scanf("%d %d", &VR[i][0], &VR[i][1]) != 2)
                {
                    printf("Invalid input format!\n");
                    break;
                }

                if (VR[i][0] == -1 && VR[i][1] == -1)
                    break;

                i++;
                if (i >= MAX_VERTEX_NUM * MAX_VERTEX_NUM)
                {
                    printf("Maximum edges reached!\n");
                    break;
                }
            }

            getchar();
            printf("Graph definition stored. Use option 1 to create the graph.\n");
            break;
        }

        case 0: // Exit program
            // Clean up all graphs before exiting
            for (auto &graph : graphs)
            {
                if (graph.second.vexnum > 0)
                    DestroyGraph(graph.second);
            }
            printf("Thank you for using the Graph Management System!\n");
            break;

        default:
            printf("Invalid option! Please try again.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
    }

    return 0;
}