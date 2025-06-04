#include <bits/stdc++.h>
using namespace std;

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1

typedef int status;
typedef int KeyType;
typedef struct
{
    KeyType key;
    char others[20];
} TElemType; // Binary tree node element definition

typedef struct
{
    int pos;
    TElemType data;
} DEF; // Position-indexed tree definition

typedef struct BiTNode
{ // Binary tree node structure
    TElemType data;
    struct BiTNode *lchild, *rchild;
} BiTNode, *BiTree;

static int duplicated1(TElemType definition[])
// Checks for duplicate keys in preorder sequence
{
    unordered_set<KeyType> s;
    int i = 0;
    while (definition[i].key != -1)
    {
        if (definition[i].key == 0) // Skip null nodes
        {
            i++;
            continue;
        }
        if (!s.insert(definition[i].key).second)
            return 1; // Duplicate found
        i++;
    }
    return 0; // No duplicates
}

static int duplicated(DEF definition[])
// Checks for duplicate keys in position-indexed definition
{
    unordered_set<KeyType> s;
    int i = 0;
    while (i < 100)
    {
        if (definition[i].data.key == 0)
        {
            i++;
            continue;
        }
        if (definition[i].pos == 0) // End of definition
            break;
        if (!s.insert(definition[i].data.key).second)
            return 1; // Duplicate found
        i++;
    }
    return 0; // No duplicates
}

static BiTree spawnNode(void)
// Creates and initializes a new tree node
{
    BiTree node = (BiTree)malloc(sizeof(BiTNode));
    node->lchild = node->rchild = NULL;
    return node;
}

static void helper0(BiTree &T, TElemType definition[], int &i)
// Recursive helper for building tree from preorder sequence
{
    if (definition[i].key == -1)
        return;
    if (definition[i].key == 0) // Null node
    {
        T = NULL;
        i++;
        return;
    }
    T = spawnNode();
    T->data = definition[i++];
    helper0(T->lchild, definition, i); // Build left subtree
    helper0(T->rchild, definition, i); // Build right subtree
}

status CreateBiTree1(BiTree &T, TElemType definition[])
// Creates binary tree from preorder sequence with null markers
{
    if (duplicated1(definition))
        return ERROR;
    int i = 0;
    if (definition[i].key != -1)
        helper0(T, definition, i);
    return OK;
}

status CreateBiTree(BiTree &T, DEF definition[])
// Creates binary tree from position-indexed definition
{
    if (duplicated(definition))
        return ERROR;
    int i = 0, j;
    static BiTNode *p[100]; // Array to hold nodes by position
    while (j = definition[i].pos)
    {
        p[j] = (BiTNode *)malloc(sizeof(BiTNode));
        p[j]->data = definition[i].data;
        p[j]->lchild = NULL;
        p[j]->rchild = NULL;
        // Link to parent based on position
        if (j != 1)
            if (j % 2) // Right child (odd position)
                p[j / 2]->rchild = p[j];
            else // Left child (even position)
                p[j / 2]->lchild = p[j];
        i++;
    }
    T = p[1]; // Root is at position 1
    return OK;
}

status ClearBiTree(BiTree &T)
// Destroys tree and frees all nodes recursively
{
    if (T == NULL)
        return OK;
    ClearBiTree(T->lchild);
    ClearBiTree(T->rchild);
    free(T);
    T = NULL;
    return OK;
}

int BiTreeDepth(BiTree T)
// Returns the depth of the binary tree
{
    if (T == NULL)
        return 0;
    int l = BiTreeDepth(T->lchild), r = BiTreeDepth(T->rchild);
    return l > r ? l + 1 : r + 1;
}

BiTNode *LocateNode(BiTree T, KeyType e)
// Finds node with given key using recursive search
{
    if (T == NULL)
        return NULL;
    if (T->data.key == e)
        return T;
    BiTree l = LocateNode(T->lchild, e);
    if (l)
        return l;
    BiTree r = LocateNode(T->rchild, e);
    if (r)
        return r;
    return NULL;
}

status Assign(BiTree &T, KeyType e, TElemType value)
// Updates node data, checks for key conflicts
{
    BiTree m = LocateNode(T, value.key);
    if (m && m->data.key != e) // Key conflict with another node
        return ERROR;
    BiTree n = LocateNode(T, e);
    if (n == NULL) // Target node not found
        return ERROR;
    n->data = value;
    return OK;
}

static BiTree parent(BiTree T, KeyType e)
// Finds parent of node with given key
{
    if (T == NULL)
        return NULL;
    if ((T->lchild && T->lchild->data.key == e) || (T->rchild && T->rchild->data.key == e))
        return T;
    BiTree l = parent(T->lchild, e);
    if (l)
        return l;
    BiTree r = parent(T->rchild, e);
    if (r)
        return r;
    return NULL;
}

BiTNode *GetSibling(BiTree T, KeyType e)
// Returns sibling node of the given key
{
    if (T == NULL || e < 1)
        return NULL;
    BiTree p = parent(T, e);
    if (p == NULL)
        return NULL;
    if (p->lchild && p->lchild->data.key == e)
        return p->rchild; // Return right sibling
    else
        return p->lchild; // Return left sibling
}

status InsertNode(BiTree &T, KeyType e, int LR, TElemType c)
// Inserts new node as child of specified parent
{
    if (LocateNode(T, c.key)) // Key already exists
        return ERROR;
    BiTree m = spawnNode(), n = NULL;
    m->data = c;
    if (LR == -1) // Insert as new root
    {
        m->rchild = T;
        T = m;
        return OK;
    }
    n = LocateNode(T, e);
    if (n == NULL) // Parent not found
        return ERROR;
    if (LR == 0) // Insert as left child
    {
        m->rchild = n->lchild;
        m->lchild = NULL;
        n->lchild = m;
    }
    else // Insert as right child
    {
        m->rchild = n->rchild;
        m->lchild = NULL;
        n->rchild = m;
    }
    return OK;
}

static int getdegr(BiTree &t)
// Returns the degree (number of children) of a node
{
    if (t == NULL)
        return 0;
    int c = 0;
    if (t->lchild)
        c++;
    if (t->rchild)
        c++;
    return c;
}

status DeleteNode(BiTree &T, KeyType e)
// Deletes node with given key, handles different cases
{
    if (T == NULL || e < 1)
        return ERROR;

    if (T->data.key == e) // Deleting root
    {
        int dt = getdegr(T);
        switch (dt)
        {
        case 0: // Leaf node
            free(T);
            T = NULL;
            break;
        case 1: // One child
        {
            BiTree p = T;
            T = T->lchild ? T->lchild : T->rchild;
            free(p);
            break;
        }
        case 2: // Two children - attach right subtree to rightmost of left
        {
            BiTree p = T, l = T->lchild, r = T->rchild;
            T = l;
            while (l && l->rchild)
                l = l->rchild;
            l->rchild = r;
            free(p);
            break;
        }
        }
        return OK;
    }

    BiTree m = LocateNode(T, e), p = parent(T, e);
    if (m == NULL || p == NULL)
        return ERROR;
    int dm = getdegr(m);
    switch (dm)
    {
    case 0: // Leaf node - simply remove link
        if (p->lchild && p->lchild->data.key == e)
            p->lchild = NULL;
        else
            p->rchild = NULL;
        free(m);
        break;
    case 1: // One child - replace with child
    {
        BiTree c = m->lchild ? m->lchild : m->rchild;
        if (p->lchild && p->lchild->data.key == e)
            p->lchild = c;
        else
            p->rchild = c;
        free(m);
        break;
    }
    case 2: // Two children - merge subtrees
    {
        BiTree l = m->lchild, r = m->rchild;
        if (p->lchild && p->lchild->data.key == e)
            p->lchild = l;
        else
            p->rchild = l;
        while (l->rchild)
            l = l->rchild;
        l->rchild = r;
        free(m);
        break;
    }
    }
    return OK;
}

status PreOrderTraverse(BiTree T, void (*visit)(BiTree))
// Recursive preorder traversal
{
    if (T == NULL)
        return OK;
    visit(T);
    PreOrderTraverse(T->lchild, visit);
    PreOrderTraverse(T->rchild, visit);
    return OK;
}

status PreOrderTraverseIII(BiTree T, void (*visit)(BiTree))
// Iterative preorder traversal using stack
{
    if (T == NULL)
        return OK;
    stack<BiTree> s;
    s.push(T);
    while (!s.empty())
    {
        BiTree t = s.top();
        s.pop();
        visit(t);
        if (t->rchild) // Push right first (will be processed after left)
            s.push(t->rchild);
        if (t->lchild)
            s.push(t->lchild);
    }
    return OK;
}

status InOrderTraverse(BiTree T, void (*visit)(BiTree))
// Recursive inorder traversal
{
    if (T == NULL)
        return OK;
    InOrderTraverse(T->lchild, visit);
    visit(T);
    InOrderTraverse(T->rchild, visit);
    return OK;
}

status PostOrderTraverse(BiTree T, void (*visit)(BiTree))
// Recursive postorder traversal
{
    if (T == NULL)
        return OK;
    PostOrderTraverse(T->lchild, visit);
    PostOrderTraverse(T->rchild, visit);
    visit(T);
    return OK;
}

status LevelOrderTraverse(BiTree T, void (*visit)(BiTree))
// Level order traversal using queue
{
    if (T == NULL)
        return OK;
    queue<BiTree> q;
    q.push(T);
    while (!q.empty())
    {
        BiTree t = q.front();
        q.pop();
        visit(t);
        if (t->lchild)
            q.push(t->lchild);
        if (t->rchild)
            q.push(t->rchild);
    }
    return OK;
}

static void preprint(BiTree T, FILE *fp)
// Helper function to write preorder sequence to file
{
    if (T == NULL)
    {
        fprintf(fp, "0 NULL ");
        return;
    }
    fprintf(fp, "%d %s ", T->data.key, T->data.others);
    preprint(T->lchild, fp);
    preprint(T->rchild, fp);
}

status SaveBiTree(BiTree T, char FileName[])
// Saves tree to file in preorder format with null markers
{
    FILE *fp = fopen(FileName, "w");
    if (fp == NULL)
        return ERROR;
    if (T == NULL)
    {
        fprintf(fp, "-1 NULL");
        return OK;
    }
    preprint(T, fp);
    fprintf(fp, "-1 NULL"); // Terminator
    fclose(fp);
    return OK;
}

status LoadBiTree(BiTree &T, char FileName[])
// Loads tree from file containing preorder sequence
{
    if (T != NULL)
        ClearBiTree(T);
    FILE *fp = fopen(FileName, "r");
    if (fp == NULL)
        return ERROR;
    TElemType v[100] = {};
    TElemType t;
    int i = 0;
    // Read elements until terminator
    while (1)
    {
        fscanf(fp, "%d %s", &t.key, &t.others);
        v[i++] = t;
        if (t.key == -1)
            break;
    }
    fclose(fp);
    CreateBiTree1(T, v);
    return OK;
}

status InvertTree(BiTree &T)
// Swaps left and right subtrees recursively
{
    if (T == NULL)
        return OK;
    BiTree t = T->lchild;
    T->lchild = T->rchild;
    T->rchild = t;
    InvertTree(T->lchild);
    InvertTree(T->rchild);
    return OK;
}

int MaxPathSum(BiTree &T)
// Finds maximum path sum from root to leaf
{
    if (T == NULL)
        return 0;
    if (T->lchild == NULL && T->rchild == NULL)
        return T->data.key;
    int l = MaxPathSum(T->lchild);
    int r = MaxPathSum(T->rchild);
    return max(l, r) + T->data.key;
}

BiTree LowestCommonAncestor(BiTree T, BiTree p, BiTree q)
// Finds the lowest common ancestor of two nodes
{
    if (T == NULL || T == p || T == q)
        return T;
    BiTree l = LowestCommonAncestor(T->lchild, p, q);
    BiTree r = LowestCommonAncestor(T->rchild, p, q);
    if (l && r) // Found nodes in different subtrees
        return T;
    return l ? l : r; // Return non-null result
}

int main(void)
// Main function providing interactive menu for binary tree operations
{
    vector<pair<string, BiTree>> trees(1, {"Default", NULL}); // Tree collection
    int op = 1;
    int currentTree = 0;      // Index of currently active tree
    DEF definition[100] = {}; // Tree definition storage

    // Lambda function for node visiting
    auto visit = [](BiTree node)
    {
        printf("%d %s | ", node->data.key, node->data.others);
    };

    while (op)
    {
        // Display menu
        printf("\n\n");
        printf("      Menu for Binary Tree Management System \n");
        printf("-----------------------Basic---------------------\n");
        printf("    1. CreateBiTree          2. DestroyBiTree\n");
        printf("    3. ClearBiTree           4. BiTreeEmpty\n");
        printf("    5. BiTreeDepth           6. LocateNode\n");
        printf("    7. Assign                8. GetSibling\n");
        printf("    9. InsertNode           10. DeleteNode\n");
        printf("   11. PreOrderTraverse     12. InOrderTraverse\n");
        printf("   13. PostOrderTraverse    14. LevelOrderTraverse\n");
        printf("----------------------Advanced--------------------\n");
        printf("   15. MaxPathSum           16. LowestCommonAncestor\n");
        printf("   17. InvertTree           18. SaveBiTree\n");
        printf("   19. LoadBiTree           20. AddNewTree\n");
        printf("   21. RemoveTree           22. TreesInfo\n");
        printf("   23. SwitchTree           24. InputTreeDefinition\n");
        printf("-----------------------Exit----------------------\n");
        printf("    0. Exit\n");
        printf("Current tree: '%s' (index %d)\n", trees[currentTree].first.c_str(), currentTree);
        printf("Please select your operation [0~24]:\n");

        scanf("%d", &op);
        getchar();

        switch (op)
        {
        case 1: // Create tree from definition
        {
            if (trees[currentTree].second != NULL)
            {
                printf("Tree '%s' already exists! Destroy it first.\n", trees[currentTree].first.c_str());
                break;
            }

            printf("Using tree definition from memory.\n");

            if (definition[0].pos == 0)
            {
                printf("No tree definition available! Use option 24 to input one first.\n");
                break;
            }

            if (CreateBiTree(trees[currentTree].second, definition) == OK)
            {
                printf("Tree '%s' created successfully!\n", trees[currentTree].first.c_str());
                memset(definition, 0, sizeof(definition)); // Clear definition
            }
            else
                printf("Error creating tree! Check for duplicate keys.\n");
            break;
        }

        case 2: // Destroy tree
            if (ClearBiTree(trees[currentTree].second) == OK)
            {
                trees[currentTree].second = NULL;
                printf("Tree '%s' destroyed successfully!\n", trees[currentTree].first.c_str());
            }
            else
                printf("Tree does not exist!\n");
            break;

        case 3: // Clear tree
            if (ClearBiTree(trees[currentTree].second) == OK)
                printf("Tree '%s' cleared successfully!\n", trees[currentTree].first.c_str());
            else
                printf("Tree does not exist!\n");
            break;

        case 4: // Check if empty
            if (trees[currentTree].second == NULL)
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
            else if (trees[currentTree].second->lchild == NULL && trees[currentTree].second->rchild == NULL)
                printf("Tree '%s' is empty!\n", trees[currentTree].first.c_str());
            else
                printf("Tree '%s' is not empty!\n", trees[currentTree].first.c_str());
            break;

        case 5: // Get tree depth
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            int depth = BiTreeDepth(trees[currentTree].second);
            printf("The depth of tree '%s' is: %d\n", trees[currentTree].first.c_str(), depth);
            break;
        }

        case 6: // Locate node by key
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            KeyType key;
            printf("Enter the key to locate: ");
            scanf("%d", &key);
            getchar();

            BiTree node = LocateNode(trees[currentTree].second, key);
            if (node != NULL)
                printf("Node with key %d found in tree '%s'! Data: %s\n", key, trees[currentTree].first.c_str(), node->data.others);
            else
                printf("Node with key %d not found in tree '%s'!\n", key, trees[currentTree].first.c_str());
            break;
        }

        case 7: // Assign new value to node
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            KeyType oldKey;
            TElemType newData;
            printf("Enter the key of the node to update: ");
            scanf("%d", &oldKey);
            printf("Enter the new key for the node: ");
            scanf("%d", &newData.key);
            printf("Enter the new data for the node: ");
            scanf("%s", newData.others);
            getchar();

            if (Assign(trees[currentTree].second, oldKey, newData) == OK)
                printf("Node with key %d updated successfully in tree '%s'!\n", oldKey, trees[currentTree].first.c_str());
            else
                printf("Failed to update node! Check if any conflicts.\n");
            break;
        }

        case 8: // Find sibling node
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            KeyType key;
            printf("Enter the key to find its sibling: ");
            scanf("%d", &key);
            getchar();

            BiTree sibling = GetSibling(trees[currentTree].second, key);
            if (sibling != NULL)
                printf("Sibling of node %d found! Key: %d, Data: %s\n", key, sibling->data.key, sibling->data.others);
            else
                printf("No sibling found for node with key %d!\n", key);
            break;
        }

        case 9: // Insert new node
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            KeyType parentKey;
            int LR;
            TElemType newNode;
            printf("Enter parent node key: ");
            scanf("%d", &parentKey);
            printf("Enter position (0 for left, 1 for right, -1 for root): ");
            scanf("%d", &LR);
            printf("Enter new node key: ");
            scanf("%d", &newNode.key);
            printf("Enter new node data: ");
            scanf("%s", newNode.others);
            getchar();

            if (InsertNode(trees[currentTree].second, parentKey, LR, newNode) == OK)
                printf("Node inserted successfully into tree '%s'!\n", trees[currentTree].first.c_str());
            else
                printf("Failed to insert node! Check if parent exists or key conflicts.\n");
            break;
        }

        case 10: // Delete node
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            KeyType key;
            printf("Enter the key of the node to delete: ");
            scanf("%d", &key);
            getchar();

            if (DeleteNode(trees[currentTree].second, key) == OK)
                printf("Node with key %d deleted successfully from tree '%s'!\n", key, trees[currentTree].first.c_str());
            else
                printf("Failed to delete node! Check if key exists.\n");
            break;
        }

        case 11: // Preorder traversal
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            printf("Pre-order traversal of tree '%s':\n", trees[currentTree].first.c_str());
            if (PreOrderTraverse(trees[currentTree].second, visit) == OK)
                printf("\nTraversal completed successfully!\n");
            else
                printf("\nTraversal failed!\n");
            break;
        }

        case 12: // Inorder traversal
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            printf("In-order traversal of tree '%s':\n", trees[currentTree].first.c_str());
            if (InOrderTraverse(trees[currentTree].second, visit) == OK)
                printf("\nTraversal completed successfully!\n");
            else
                printf("\nTraversal failed!\n");
            break;
        }

        case 13: // Postorder traversal
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            printf("Post-order traversal of tree '%s':\n", trees[currentTree].first.c_str());
            if (PostOrderTraverse(trees[currentTree].second, visit) == OK)
                printf("\nTraversal completed successfully!\n");
            else
                printf("\nTraversal failed!\n");
            break;
        }

        case 14: // Level order traversal
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            printf("Level-order traversal of tree '%s':\n", trees[currentTree].first.c_str());
            if (LevelOrderTraverse(trees[currentTree].second, visit) == OK)
                printf("\nTraversal completed successfully!\n");
            else
                printf("\nTraversal failed!\n");
            break;
        }

        case 15: // Calculate maximum path sum
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            int maxSum = MaxPathSum(trees[currentTree].second);
            printf("Maximum path sum in tree '%s': %d\n", trees[currentTree].first.c_str(), maxSum);
            break;
        }

        case 16: // Find lowest common ancestor
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            KeyType key1, key2;
            printf("Enter first node key: ");
            scanf("%d", &key1);
            printf("Enter second node key: ");
            scanf("%d", &key2);
            getchar();

            BiTree node1 = LocateNode(trees[currentTree].second, key1);
            BiTree node2 = LocateNode(trees[currentTree].second, key2);

            if (node1 == NULL || node2 == NULL)
            {
                printf("One or both nodes not found in the tree!\n");
                break;
            }

            BiTree lca = LowestCommonAncestor(trees[currentTree].second, node1, node2);
            if (lca == NULL)
            {
                printf("Lowest common ancestor not found!\n");
                break;
            }
            printf("Lowest common ancestor of nodes %d and %d is: %d\n", key1, key2, lca->data.key);
            break;
        }

        case 17: // Invert tree
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            if (InvertTree(trees[currentTree].second) == OK)
                printf("Tree '%s' inverted successfully!\n", trees[currentTree].first.c_str());
            else
                printf("Failed to invert tree!\n");
            break;
        }

        case 18: // Save tree to file
        {
            if (trees[currentTree].second == NULL)
            {
                printf("Tree '%s' does not exist!\n", trees[currentTree].first.c_str());
                break;
            }

            char filename[100];
            printf("Enter filename to save: ");
            scanf("%s", filename);
            getchar();

            if (SaveBiTree(trees[currentTree].second, filename) == OK)
                printf("Tree '%s' saved to file '%s' successfully!\n", trees[currentTree].first.c_str(), filename);
            else
                printf("Failed to save tree to file!\n");
            break;
        }

        case 19: // Load tree from file
        {
            if (trees[currentTree].second != NULL)
            {
                printf("Tree '%s' already exists! Destroy it first.\n", trees[currentTree].first.c_str());
                break;
            }

            char filename[100];
            printf("Enter filename to load: ");
            scanf("%s", filename);
            getchar();

            if (LoadBiTree(trees[currentTree].second, filename) == OK)
                printf("Tree '%s' loaded from file '%s' successfully!\n", trees[currentTree].first.c_str(), filename);
            else
                printf("Failed to load tree from file!\n");
            break;
        }

        case 20: // Add new tree to collection
        {
            char treeName[30];
            printf("Enter name for the new tree: ");
            scanf("%s", treeName);
            getchar();

            // Check for duplicate names
            bool nameExists = false;
            for (int i = 0; i < trees.size(); i++)
            {
                if (trees[i].first == treeName)
                {
                    nameExists = true;
                    break;
                }
            }

            if (nameExists)
            {
                printf("Tree '%s' already exists!\n", treeName);
                break;
            }

            trees.push_back({string(treeName), NULL});
            printf("New tree '%s' created successfully at index %d!\n", treeName, trees.size() - 1);
            break;
        }

        case 21: // Remove tree from collection
        {
            if (trees.size() == 0)
            {
                printf("No trees to remove!\n");
                break;
            }

            printf("Available trees:\n");
            for (int i = 0; i < trees.size(); i++)
            {
                printf("%d. %s%s\n", i, trees[i].first.c_str(), (i == currentTree ? " [ACTIVE]" : ""));
            }

            int index;
            printf("Enter index of tree to remove: ");
            scanf("%d", &index);
            getchar();

            if (index < 0 || index >= trees.size())
            {
                printf("Invalid tree index!\n");
                break;
            }

            if (trees[index].second != NULL)
                ClearBiTree(trees[index].second);

            trees.erase(trees.begin() + index);
            printf("Tree removed successfully!\n");

            // Adjust current tree index if necessary
            if (trees.empty())
            {
                printf("No trees left. Create a new tree to continue.\n");
            }
            else if (index == currentTree)
            {
                printf("You removed the active tree. Switch to another tree to continue.\n");
            }
            break;
        }

        case 22: // Display information about all trees
        {
            if (trees.empty())
            {
                printf("No trees available!\n");
                break;
            }

            printf("Total number of trees: %d\n", trees.size());
            printf("Current active tree: '%s' (index %d)\n", trees[currentTree].first.c_str(), currentTree);

            for (int i = 0; i < trees.size(); i++)
            {
                printf("Tree %d: '%s' - %s", i, trees[i].first.c_str(),
                       (trees[i].second == NULL) ? "Not created" : "Exists");
                if (trees[i].second != NULL)
                {
                    int depth = BiTreeDepth(trees[i].second);
                    printf(" (depth: %d)", depth);
                }
                if (i == currentTree)
                    printf(" [ACTIVE]");
                printf("\n");
            }
            break;
        }

        case 23: // Switch to different tree
        {
            if (trees.empty())
            {
                printf("No trees available!\n");
                break;
            }
            printf("Current tree: '%s' (index %d)\n", trees[currentTree].first.c_str(), currentTree);
            printf("Available trees:\n");
            for (int i = 0; i < trees.size(); i++)
            {
                printf("%d. %s%s\n", i, trees[i].first.c_str(), (i == currentTree ? " [ACTIVE]" : ""));
            }

            int index;
            printf("Enter tree index to switch to: ");
            scanf("%d", &index);
            getchar();

            if (index >= 0 && index < trees.size())
            {
                currentTree = index;
                printf("Switched to tree '%s' at index %d\n", trees[currentTree].first.c_str(), currentTree);
            }
            else
                printf("Invalid tree index!\n");
            break;
        }

        case 24: // Input tree definition manually
        {
            printf("Enter tree definition (format: position key data):\n");
            printf("Enter '0 0 NULL' to finish input\n");
            int i = 0;

            while (1)
            {
                scanf("%d %d %s", &definition[i].pos, &definition[i].data.key, definition[i].data.others);
                if (definition[i].pos == 0)
                    break;
                i++;
                if (i >= 100)
                {
                    printf("Maximum 100 nodes allowed.\n");
                    break;
                }
            }

            getchar();
            printf("Tree definition stored. Use option 1 to create the tree.\n");
            break;
        }

        case 0: // Exit program
            // Clean up all trees before exiting
            for (auto &tree : trees)
            {
                if (tree.second != NULL)
                    ClearBiTree(tree.second);
            }
            printf("Thank you for using the Binary Tree Management System!\n");
            break;

        default:
            printf("Invalid option! Please try again.\n");
        }
    }

    return 0;
}