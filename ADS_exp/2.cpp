#include <bits/stdc++.h>
using namespace std;

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1

typedef int status;
typedef int ElemType; // Data element type definition

typedef struct LNode
{ // Singly linked list node definition
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;

status InitList(LinkList &L)
// Creates an empty linked list with head node
{
    if (L != NULL)
        return INFEASIBLE;
    LinkList n = (LinkList)malloc(sizeof(LNode));
    n->next = NULL;
    L = n;
    return OK;
}

status DestroyList(LinkList &L)
// Destroys the list and frees all memory
{
    if (L == NULL)
        return INFEASIBLE;
    LinkList p, q = L;
    while (q->next)
    {
        p = q;
        q = q->next;
        free(p);
    }
    free(q);
    L = NULL;
    return OK;
}

status ClearList(LinkList &L)
// Removes all elements but keeps the head node
{
    if (L == NULL)
        return INFEASIBLE;
    if (!L->next)
        return OK;
    LinkList p, q = L->next;
    while (q->next)
    {
        p = q;
        q = q->next;
        free(p);
    }
    free(q);
    L->next = NULL;
    return OK;
}

status ListEmpty(LinkList L)
// Checks if the list is empty
{
    if (!L)
        return INFEASIBLE;
    if (!L->next)
        return TRUE;
    return FALSE;
}

int ListLength(LinkList L)
// Returns the number of elements in the list
{
    if (!L)
        return INFEASIBLE;
    LinkList t = L;
    int c = 0;
    while (t->next)
        t = t->next, c++;
    return c;
}

status GetElem(LinkList L, int i, ElemType &e)
// Gets the element at position i (1-based indexing)
{
    if (i < 1)
        return ERROR;
    if (!L)
        return INFEASIBLE;
    LinkList t = L->next;
    int c = 0;
    while (t)
    {
        c++;
        if (c == i)
        {
            e = t->data;
            return OK;
        }
        t = t->next;
    }
    return ERROR;
}

status LocateElem(LinkList L, ElemType e)
// Finds the position of element e in the list
{
    if (!L)
        return INFEASIBLE;
    LinkList t = L->next;
    int c = 0;
    while (t)
    {
        c++;
        if (e == t->data)
            return c;
        t = t->next;
    }
    return ERROR;
}

status PriorElem(LinkList L, ElemType e, ElemType &pre)
// Gets the predecessor of element e
{
    if (!L)
        return INFEASIBLE;
    if (!L->next)
        return ERROR;
    LinkList p = L->next, t = p->next;
    while (t)
    {
        if (e == t->data)
        {
            pre = p->data;
            return OK;
        }
        p = p->next, t = t->next;
    }
    return ERROR;
}

status NextElem(LinkList L, ElemType e, ElemType &next)
// Gets the successor of element e
{
    if (!L)
        return INFEASIBLE;
    LinkList p = L, t = p->next;
    while (t)
    {
        if (e == p->data)
        {
            next = t->data;
            return OK;
        }
        p = p->next, t = t->next;
    }
    return ERROR;
}

status ListInsert(LinkList &L, int i, ElemType e)
// Inserts element e at position i
{
    if (i < 1)
        return ERROR;
    if (!L)
        return INFEASIBLE;
    LinkList t = L;
    int c = 0;
    // Find the position before insertion point
    while (t && c < i - 1)
    {
        c++;
        t = t->next;
    }
    if (c < i - 1 || !t)
        return ERROR;
    // Create new node and link it
    LinkList n = (LinkList)malloc(sizeof(LNode));
    n->next = t->next;
    t->next = n;
    n->data = e;
    return OK;
}

status ListDelete(LinkList &L, int i, ElemType &e)
// Deletes element at position i and returns it in e
{
    if (i < 1)
        return ERROR;
    if (!L)
        return INFEASIBLE;
    LinkList t = L, p = NULL;
    int c = 0;
    // Find the node before deletion point
    while (t && c < i - 1)
    {
        c++;
        t = t->next;
    }
    if (c < i - 1 || !t || !t->next)
        return ERROR;
    // Delete the node
    p = t->next;
    e = p->data;
    t->next = p->next;
    free(p);
    return OK;
}

status ListTraverse(LinkList L)
// Displays all elements in the list
{
    if (!L)
        return INFEASIBLE;
    LinkList t = L->next;
    while (t)
    {
        printf("%d ", t->data);
        t = t->next;
    }
    return OK;
}

status SaveList(LinkList L, char FileName[])
// Saves list elements to a text file
{
    if (!L)
        return INFEASIBLE;
    FILE *fp = fopen(FileName, "w");
    LinkList t = L->next;
    while (t)
    {
        fprintf(fp, "%d ", t->data);
        t = t->next;
    }
    fclose(fp);
    return OK;
}

status LoadList(LinkList &L, char FileName[])
// Loads list elements from a text file
{
    if (L)
        return INFEASIBLE;
    // Initialize list with head node
    LinkList n = (LinkList)malloc(sizeof(LNode));
    n->next = NULL;
    L = n;
    FILE *fp = fopen(FileName, "r");
    if (fp == NULL)
        return ERROR;
    LinkList t = L;
    ElemType e;
    // Read elements and create nodes
    while ((fscanf(fp, "%d", &e)) == 1)
    {
        LinkList n = (LinkList)malloc(sizeof(LNode));
        n->next = NULL;
        t->next = n;
        n->data = e;
        t = t->next;
    }
    fclose(fp);
    return OK;
}

status ReverseList(LinkList &L)
// Reverses the order of elements in the list
{
    if (!L)
        return INFEASIBLE;
    LinkList p = L->next, q = NULL, r = NULL;
    // Use three pointers to reverse links
    while (p)
    {
        r = q;
        q = p;
        p = p->next;
        q->next = r;
    }
    L->next = q;
    return OK;
}

status RemoveNthFromEnd(LinkList &L, int n)
// Removes the nth node from the end of the list
{
    if (!L || (n == 0))
        return INFEASIBLE;
    if (n < 0)
        return ERROR;
    // Use two pointers with n distance apart
    LinkList p = L->next, q = L->next;
    for (int i = 0; i < n; i++)
    {
        if (!p)
            return ERROR;
        p = p->next;
    }
    LinkList pre = L;
    while (p)
    {
        pre = pre->next;
        p = p->next;
    }
    if (pre->next == NULL)
        return ERROR;
    LinkList t = pre->next;
    pre->next = t->next;
    free(t);
    return OK;
}

status SortList(LinkList &L)
// Sorts the list in ascending order using bubble sort
{
    if (!L)
        return INFEASIBLE;
    if (!L->next || !L->next->next)
        return OK;
    LinkList p = L->next, q = NULL, r = NULL;
    // Bubble sort implementation
    while (p)
    {
        r = p->next;
        while (r)
        {
            if (p->data > r->data)
            {
                ElemType temp = p->data;
                p->data = r->data;
                r->data = temp;
            }
            r = r->next;
        }
        p = p->next;
    }
    return OK;
}

int main(void)
// Main function providing interactive menu for linked list operations
{
    vector<pair<string, LinkList>> lists; // Stores named lists
    LinkList L = NULL;
    lists.push_back({"Default", NULL}); // Default list with name
    int op = 1, size = 1;
    int currentList = 0; // Index of currently active list

    while (op)
    {
        // Display menu
        printf("\n\n");
        printf("      Menu for Linear Table On Linked List Structure \n");
        printf("-----------------------Basic---------------------\n");
        printf("    1. InitList             2. DestroyList\n");
        printf("    3. ClearList            4. ListEmpty\n");
        printf("    5. ListLength           6. GetElem\n");
        printf("    7. LocateElem           8. PriorElem\n");
        printf("    9. NextElem            10. ListInsert\n");
        printf("   11. ListDelete          12. ListTraverse\n");
        printf("----------------------Advanced--------------------\n");
        printf("   13. ReverseList         14. RemoveNthFromEnd\n");
        printf("   15. SortList            16. SaveList\n");
        printf("   17. LoadList            18. AddNewList\n");
        printf("   19. SwitchList          20. ListsInfo\n");
        printf("   21. RemoveList          22. SetList\n");
        printf("-----------------------Exit----------------------\n");
        printf("    0. Exit\n");
        printf("Please select your operation [0~22]:\n");

        scanf("%d", &op);
        getchar();

        switch (op)
        {
        case 1: // Initialize list
            if (InitList(lists[currentList].second) == OK)
                printf("List '%s' created successfully!\n", lists[currentList].first.c_str());
            else
                printf("List creation failed!\n");
            break;

        case 2: // Destroy list
            if (DestroyList(lists[currentList].second) == OK)
                printf("List '%s' destroyed successfully!\n", lists[currentList].first.c_str());
            else
                printf("List does not exist!\n");
            break;

        case 3: // Clear list
            if (ClearList(lists[currentList].second) == OK)
                printf("List '%s' cleared successfully!\n", lists[currentList].first.c_str());
            else
                printf("List does not exist!\n");
            break;

        case 4: // Check if empty
            switch (ListEmpty(lists[currentList].second))
            {
            case TRUE:
                printf("List '%s' is empty!\n", lists[currentList].first.c_str());
                break;
            case FALSE:
                printf("List '%s' is not empty!\n", lists[currentList].first.c_str());
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;

        case 5: // Get list length
        {
            int len = ListLength(lists[currentList].second);
            if (len == INFEASIBLE)
                printf("List does not exist!\n");
            else
                printf("The length of list '%s' is %d\n", lists[currentList].first.c_str(), len);
            break;
        }

        case 6: // Get element at position
        {
            int pos, elem;
            printf("Enter the position of the element: ");
            scanf("%d", &pos);
            getchar();

            switch (GetElem(lists[currentList].second, pos, elem))
            {
            case OK:
                printf("Element at position %d in list '%s' is: %d\n", pos, lists[currentList].first.c_str(), elem);
                break;
            case ERROR:
                printf("Invalid position!\n");
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;
        }

        case 7: // Locate element
        {
            int elem;
            printf("Enter the element to locate: ");
            scanf("%d", &elem);
            getchar();

            int pos = LocateElem(lists[currentList].second, elem);
            if (pos == INFEASIBLE)
                printf("List does not exist!\n");
            else if (pos == ERROR)
                printf("Element not found!\n");
            else
                printf("Element %d found at position %d in list '%s'\n", elem, pos, lists[currentList].first.c_str());
            break;
        }

        case 8: // Get predecessor
        {
            int elem, prior;
            printf("Enter the element to find its predecessor: ");
            scanf("%d", &elem);
            getchar();

            switch (PriorElem(lists[currentList].second, elem, prior))
            {
            case OK:
                printf("The predecessor of %d in list '%s' is: %d\n", elem, lists[currentList].first.c_str(), prior);
                break;
            case ERROR:
                printf("No predecessor found for this element!\n");
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;
        }

        case 9: // Get successor
        {
            int elem, next;
            printf("Enter the element to find its successor: ");
            scanf("%d", &elem);
            getchar();

            switch (NextElem(lists[currentList].second, elem, next))
            {
            case OK:
                printf("The successor of %d in list '%s' is: %d\n", elem, lists[currentList].first.c_str(), next);
                break;
            case ERROR:
                printf("No successor found for this element!\n");
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;
        }

        case 10: // Insert element
        {
            int pos, elem;
            printf("Enter position to insert: ");
            scanf("%d", &pos);
            printf("Enter element to insert: ");
            scanf("%d", &elem);
            getchar();

            switch (ListInsert(lists[currentList].second, pos, elem))
            {
            case OK:
                printf("Element %d inserted successfully at position %d in list '%s'\n", elem, pos, lists[currentList].first.c_str());
                break;
            case ERROR:
                printf("Invalid position for insertion!\n");
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;
        }

        case 11: // Delete element
        {
            int pos, elem;
            printf("Enter position to delete: ");
            scanf("%d", &pos);
            getchar();

            switch (ListDelete(lists[currentList].second, pos, elem))
            {
            case OK:
                printf("Element %d deleted successfully from position %d in list '%s'\n", elem, pos, lists[currentList].first.c_str());
                break;
            case ERROR:
                printf("Invalid position for deletion!\n");
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;
        }

        case 12: // Display all elements
            printf("Elements in list '%s':\n", lists[currentList].first.c_str());
            if (ListTraverse(lists[currentList].second) == INFEASIBLE)
                printf("List does not exist!\n");
            break;

        case 13: // Reverse list
            switch (ReverseList(lists[currentList].second))
            {
            case OK:
                printf("List '%s' reversed successfully!\n", lists[currentList].first.c_str());
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;

        case 14: // Remove nth from end
        {
            int n;
            printf("Enter the nth node from end to remove: ");
            scanf("%d", &n);
            getchar();

            switch (RemoveNthFromEnd(lists[currentList].second, n))
            {
            case OK:
                printf("Removed %dth node from end of list '%s' successfully!\n", n, lists[currentList].first.c_str());
                break;
            case ERROR:
                printf("Invalid node position!\n");
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;
        }

        case 15: // Sort list
            switch (SortList(lists[currentList].second))
            {
            case OK:
                printf("List '%s' sorted successfully!\n", lists[currentList].first.c_str());
                break;
            case INFEASIBLE:
                printf("List does not exist!\n");
                break;
            }
            break;

        case 16: // Save to file
        {
            char filename[100];
            printf("Enter filename to save: ");
            scanf("%s", filename);
            getchar();

            if (SaveList(lists[currentList].second, filename) == OK)
                printf("List '%s' saved to %s successfully!\n", lists[currentList].first.c_str(), filename);
            else
                printf("Failed to save list!\n");
            break;
        }

        case 17: // Load from file
        {
            char filename[100];
            printf("Enter filename to load: ");
            scanf("%s", filename);
            getchar();

            int t = LoadList(lists[currentList].second, filename);
            if (t == OK)
                printf("List '%s' loaded from %s successfully!\n", lists[currentList].first.c_str(), filename);
            else if (t == INFEASIBLE)
                printf("List already exists!\n");
            else
                printf("Failed to load list!\n");
            break;
        }

        case 18: // Add new named list
        {
            char listName[30];
            printf("Enter name for the new list: ");
            scanf("%s", listName);
            getchar();

            // Check for duplicate names
            bool nameExists = false;
            size = lists.size();
            for (int i = 0; i < size; i++)
            {
                if (lists[i].first == listName)
                {
                    nameExists = true;
                    break;
                }
            }

            if (nameExists)
            {
                printf("List '%s' already exists!\n", listName);
                break;
            }

            LinkList newList = NULL;
            if (InitList(newList) == OK)
            {
                lists.push_back({string(listName), newList});
                printf("New list '%s' created successfully at index %d!\n", listName, lists.size() - 1);
            }
            else
                printf("Failed to create new list!\n");
            break;
        }

        case 19: // Switch active list
        {
            printf("Current list: '%s' (index %d)\n", lists[currentList].first.c_str(), currentList);
            printf("Available lists:\n");
            size = lists.size();
            for (int i = 0; i < size; i++)
            {
                printf("%d. %s%s\n", i, lists[i].first.c_str(), (i == currentList ? " [ACTIVE]" : ""));
            }

            int index;
            printf("Enter list index to switch to: ");
            scanf("%d", &index);
            getchar();

            if (index >= 0 && index < size)
            {
                currentList = index;
                printf("Switched to list '%s' at index %d\n", lists[currentList].first.c_str(), currentList);
            }
            else
                printf("Invalid list index!\n");
            break;
        }

        case 20: // Display all lists info
        {
            size = lists.size();
            printf("Total number of lists: %d\n", size);
            printf("Current active list: '%s' (index %d)\n", lists[currentList].first.c_str(), currentList);

            for (int i = 0; i < size; i++)
            {
                int len = ListLength(lists[i].second);
                printf("List %d: '%s' - %s", i, lists[i].first.c_str(),
                       (lists[i].second == NULL) ? "Not initialized" : (len == 0) ? "Empty"
                                                                                  : "Contains elements");
                if (len > 0)
                    printf(" (%d elements)", len);
                if (i == currentList)
                    printf(" [ACTIVE]");
                printf("\n");
            }
            break;
        }

        case 21: // Remove list from manager
        {
            int index;
            printf("Enter the index of the list to remove: ");
            scanf("%d", &index);
            getchar();

            if (index < 0 || index >= lists.size())
            {
                printf("Invalid list index!\n");
                break;
            }

            if (lists[index].second != NULL)
                DestroyList(lists[index].second);

            lists.erase(lists.begin() + index);
            if (currentList >= lists.size())
                currentList = lists.size() - 1; // Adjust active list index

            printf("List at index %d removed successfully!\n", index);
            break;
        }

        case 22: // Manually set list elements
        {
            if (lists[currentList].second == NULL)
            {
                printf("List '%s' is not initialized!\n", lists[currentList].first.c_str());
                break;
            }

            ClearList(lists[currentList].second);
            int len;
            printf("Enter the number of elements to add: ");
            scanf("%d", &len);
            printf("Enter %d elements: ", len);

            for (int i = 0; i < len; i++)
            {
                int elem;
                scanf("%d", &elem);
                ListInsert(lists[currentList].second, i + 1, elem);
            }
            getchar();
            printf("Elements added to list '%s' successfully!\n", lists[currentList].first.c_str());
            break;
        }

        case 0: // Exit program
            // Clean up all lists before exiting
            for (auto &list : lists)
            {
                if (list.second != NULL)
                    DestroyList(list.second);
            }
            printf("Thank you for using the Linked List Management System!\n");
            break;

        default:
            printf("Invalid option! Please try again.\n");
        }

        if (op != 0)
        {
            printf("\nPress Enter to continue...");
            getchar();
        }
    }

    return 0;
}
