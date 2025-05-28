#include <bits/stdc++.h>
using namespace std;

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
// #define OVERFLOW -2
#define MAX_VERTEX_NUM 20
typedef int status;
typedef int KeyType;
typedef enum
{
    DG,
    DN,
    UDG,
    UDN
} GraphKind;
typedef struct
{
    KeyType key;
    char others[20];
} VertexType; // 顶点类型定义
typedef struct ArcNode
{                            // 表结点类型定义
    int adjvex;              // 顶点位置编号
    struct ArcNode *nextarc; // 下一个表结点指针
} ArcNode;
typedef struct VNode
{                      // 头结点及其数组类型定义
    VertexType data;   // 顶点信息
    ArcNode *firstarc; // 指向第一条弧
} VNode, AdjList[MAX_VERTEX_NUM];
typedef struct
{                       // 邻接表的类型定义
    AdjList vertices;   // 头结点数组
    int vexnum, arcnum; // 顶点数、弧数
    GraphKind kind;     // 图的类型
} ALGraph;

static int duplicated(VertexType V[], int &n)
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
/*根据V和VR构造图T并返回OK，如果V和VR不正确，返回ERROR
如果有相同的关键字，返回ERROR。此题允许通过增加其它函数辅助实现本关任务*/
{
    if (V[0].key == -1 || (V[1].key == -1 && VR[0][0] != -1) || duplicated(V, G.vexnum))
        return ERROR; // Invalid input
    if (G.vexnum > MAX_VERTEX_NUM)
        return ERROR;

    G.arcnum = 0;
    G.kind = UDG;
    unordered_map<KeyType, int> dic;
    for (int i = 0; i < G.vexnum; i++)
    {
        dic[V[i].key] = i;
        G.vertices[i].data = V[i];
        G.vertices[i].firstarc = NULL;
    }

    while (VR[G.arcnum][0] != -1)
        G.arcnum++;
    for (int i = 0; i < G.arcnum; i++)
    {
        if (VR[i][0] == VR[i][1])
            continue;
        KeyType v1 = VR[i][0], v2 = VR[i][1];
        int j, k, flg = 0;
        if (dic.find(v1) == dic.end() || dic.find(v2) == dic.end())
            return ERROR; // Invalid edge
        j = dic[v1], k = dic[v2];

        for (ArcNode *p = G.vertices[j].firstarc; p != NULL; p = p->nextarc)
            if (p->adjvex == k)
            {
                flg = 1; // Duplicate edge found
                break;
            }
        if (flg)
            continue;

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
/*销毁无向图G,删除G的全部顶点和边*/
{
    for (int i = 0; i < G.vexnum; i++)
    {
        ArcNode *p = G.vertices[i].firstarc;
        while (p != NULL)
        {
            ArcNode *q = p;
            p = p->nextarc;
            delete q; // free(q);
        }
        G.vertices[i].firstarc = NULL;
    }
    G.vexnum = 0;
    G.arcnum = 0;
    return OK;
}

int LocateVex(ALGraph G, KeyType u)
// 根据u在图G中查找顶点，查找成功返回位序，否则返回-1；
{
    for (int i = 0; i < G.vexnum; i++)
        if (G.vertices[i].data.key == u)
            return i;
    return -1;
}

status PutVex(ALGraph &G, KeyType u, VertexType value)
// 根据u在图G中查找顶点，查找成功将该顶点值修改成value，返回OK；
// 如果查找失败或关键字不唯一，返回ERROR
{
    int i = LocateVex(G, u);
    if (i == -1)
        return ERROR;
    int j = LocateVex(G, value.key);
    if (j != -1 && j != i)
        return ERROR; // Duplicate key found
    G.vertices[i].data = value;
    return OK;
}

int FirstAdjVex(ALGraph G, KeyType u)
// 根据u在图G中查找顶点，查找成功返回顶点u的第一邻接顶点位序，否则返回-1；
{
    int i = LocateVex(G, u);
    if (i == -1 || G.vertices[i].firstarc == NULL)
        return -1;
    return G.vertices[i].firstarc->adjvex;
}

int NextAdjVex(ALGraph G, KeyType v, KeyType w)
// v对应G的一个顶点,w对应v的邻接顶点；操作结果是返回v的（相对于w）下一个邻接顶点的位序；如果w是最后一个邻接顶点，或v、w对应顶点不存在，则返回-1。
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
// 在图G中插入顶点v，成功返回OK,否则返回ERROR
{
    if (G.vexnum == MAX_VERTEX_NUM || LocateVex(G, v.key) != -1)
        return ERROR;
    G.vertices[G.vexnum].data = v;
    G.vertices[G.vexnum++].firstarc = NULL;
    return OK;
}

status DeleteVex(ALGraph &G, KeyType v)
// 在图G中删除关键字v对应的顶点以及相关的弧，成功返回OK,否则返回ERROR
{
    int i = LocateVex(G, v);
    if (i == -1 || G.vexnum <= 1)
        return ERROR;
    ArcNode *p = G.vertices[i].firstarc;
    while (p != NULL)
    {
        ArcNode *q = p;
        p = p->nextarc;
        delete q; // free(q);
        G.arcnum--;
    }
    for (int j = i; j < G.vexnum - 1; j++)
        G.vertices[j] = G.vertices[j + 1];
    G.vexnum--;
    for (int j = 0; j < G.vexnum; j++)
    {
        ArcNode *p = G.vertices[j].firstarc, *q = NULL;
        while (p && p->adjvex == i) // if is enough
        {
            ArcNode *temp = p;
            p = p->nextarc;
            delete temp; // free(temp);
        }
        G.vertices[j].firstarc = p;
        q = p;
        while (p != NULL)
        {
            if (p->adjvex == i)
            {
                q->nextarc = p->nextarc;
                ArcNode *temp = p;
                p = p->nextarc;
                delete temp; // free(temp);
            }
            else if (p->adjvex > i)
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
// 在图G中增加弧<v,w>，成功返回OK,否则返回ERROR
{
    int i = LocateVex(G, v), j = LocateVex(G, w);
    if (i == -1 || j == -1)
        return ERROR;
    ArcNode *p = G.vertices[i].firstarc;
    while (p != NULL)
    {
        if (p->adjvex == j)
            return ERROR; // Duplicate edge found
        p = p->nextarc;
    }
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
{
    if ((*p)->adjvex == i)
    {
        ArcNode *temp = *p;
        *p = (*p)->nextarc;
        delete temp; // free(temp);
        return 1;
    }
    ArcNode *q = *p, *r = (*p)->nextarc;
    while (r != NULL)
    {
        if (r->adjvex == i)
        {
            q->nextarc = r->nextarc;
            delete r; // free(r);
            return 1;
        }
        q = r;
        r = r->nextarc;
    }
    return 0; // Edge not found
}
status DeleteArc(ALGraph &G, KeyType v, KeyType w)
// 在图G中删除弧<v,w>，成功返回OK,否则返回ERROR
{
    int i = LocateVex(G, v), j = LocateVex(G, w);
    if (i == -1 || j == -1)
        return ERROR;
    if (da(&G.vertices[i].firstarc, j) == 0)
        return ERROR; // Edge not found
    da(&G.vertices[j].firstarc, i);
    G.arcnum--;
    return OK;
}

status DFSTraverse(ALGraph &G, void (*visit)(VertexType))
// 对图G进行深度优先搜索遍历，依次对图中的每一个顶点使用函数visit访问一次，且仅访问一次
{
    if (G.vexnum == 0)
        return ERROR;
    unordered_set<int> visited;
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
// 对图G进行广度优先搜索遍历，依次对图中的每一个顶点使用函数visit访问一次，且仅访问一次
{
    if (G.vexnum == 0)
        return ERROR;
    unordered_set<int> visited;
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
                for (ArcNode *p = G.vertices[i].firstarc; p != NULL; p = p->nextarc)
                    if (visited.find(p->adjvex) == visited.end())
                        q.push(p->adjvex);
            }
        }
    }
    return OK;
}

status SaveGraph(ALGraph G, char FileName[])
// 将图的数据写入到文件FileName中
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
        fprintf(fp, "-1\n");
    }
    fclose(fp);
    return OK;
}
status LoadGraph(ALGraph &G, char FileName[])
// 读入文件FileName的图数据，创建图的邻接表
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
        if (tmp == -1)
            continue;
        else
        {
            ArcNode *p = new ArcNode;
            p->adjvex = tmp;
            p->nextarc = NULL;
            G.vertices[i].firstarc = p;

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
{
    // all weights 1, n*Dijkstra(nmlogm),Floyd(n^3)
    // Floyd
    int **dist = new int *[G.vexnum];
    for (int i = 0; i < G.vexnum; i++)
        dist[i] = new int[G.vexnum];
    for (int i = 0; i < G.vexnum; i++)
        for (int j = 0; j < G.vexnum; j++)
            dist[i][j] = INT_MAX;
    for (int i = 0; i < G.vexnum; i++)
    { // D0
        dist[i][i] = 0;
        ArcNode *p = G.vertices[i].firstarc;
        while (p != NULL)
        {
            dist[i][p->adjvex] = 1;
            p = p->nextarc;
        }
    }
    for (int k = 0; k < G.vexnum; k++)
        for (int i = 0; i < G.vexnum; i++)
            for (int j = 0; j < G.vexnum; j++)
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
    return dist;
}

VertexType *VerticesSetLessThanK1(ALGraph &G, VertexType v, int k)
{ // Floyd
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
    result[count].key = -1; // end
    for (int j = 0; j < G.vexnum; j++)
        delete[] dist[j];
    delete[] dist;
    return result;
}

VertexType *VerticesSetLessThanK(ALGraph &G, VertexType v, int k)
{ // Dijkstra
    int i = LocateVex(G, v.key);
    if (i == -1)
        return ERROR;
    if (G.vertices[i].firstarc == NULL)
        return NULL;
    VertexType *result = new VertexType[G.vexnum];
    int count = 0;
    int *dist = new int[G.vexnum];      // d(i,j)
    bool *visited = new bool[G.vexnum]; // set 0
    memset(dist, INT_MAX, sizeof(int) * G.vexnum);
    memset(visited, false, sizeof(bool) * G.vexnum);
    dist[i] = 0;
    // priority_queue<> q; // set 1

    return result;
}

int ShortestPathLen(ALGraph G, VertexType v, VertexType w)
{ // Floyd
    int i = LocateVex(G, v.key);
    int j = LocateVex(G, w.key);
    if (i == -1 || j == -1 || G.vertices[i].firstarc == NULL)
        return ERROR;
    int **dist = distance(G);
    int result = dist[i][j];
    for (int k = 0; k < G.vexnum; k++)
        delete[] dist[k];
    delete[] dist;
    return result;
}

int main(void)
{
    vector<pair<string, ALGraph>> graphs(1, {"Default", {}});
    int op = 1;
    int currentGraph = 0; // Index of currently active graph
    VertexType V[MAX_VERTEX_NUM];
    KeyType VR[MAX_VERTEX_NUM * MAX_VERTEX_NUM][2];

    auto visit = [](VertexType vertex)
    {
        printf("%d %s | ", vertex.key, vertex.others);
    };

    while (op)
    {
        // system("cls");
        // printf("\n\n");
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
        case 1: // CreateGraph
        {
            if (V[0].key == -1)
            {
                printf("No graph definition available! Use option 22 to input one first.\n");
                break;
            }

            if (CreateCraph(graphs[currentGraph].second, V, VR) == OK)
            {
                printf("Graph '%s' created successfully!\n", graphs[currentGraph].first.c_str());
                // Reset the definition arrays for future use
                memset(V, -1, sizeof(V));
                memset(VR, -1, sizeof(VR));
            }
            else
                printf("Error creating graph! Check for duplicate keys or invalid data.\n");
            break;
        }

        case 2: // DestroyGraph
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

        case 3: // LocateVex
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
                       graphs[currentGraph].second.vertices[loc].data.key,
                       loc);
            else
                printf("Vertex with key %d not found in graph '%s'!\n", key, graphs[currentGraph].first.c_str());
            break;
        }

        case 4: // PutVex
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

        case 5: // FirstAdjVex
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

        case 6: // NextAdjVex
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

        case 7: // InsertVex
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

        case 8: // DeleteVex
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

        case 9: // InsertArc
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

        case 10: // DeleteArc
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

        case 11: // DFSTraverse
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

        case 13: // VerticesSetLessThanK
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

        case 14: // ShortestPathLen
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

        case 15: // ConnectedComponent
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

        case 16: // SaveGraph
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

        case 17: // LoadGraph
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

        case 18: // CreateNewGraph
        {
            char graphName[30];
            printf("Enter name for the new graph: ");
            scanf("%s", graphName);
            getchar();

            // Check if graph name already exists
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

        case 19: // RemoveGraph
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

        case 20: // GraphsInfo
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

        case 21: // SwitchGraph
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

        case 22: // InputGraphDefinition
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

        case 0: // Exit
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