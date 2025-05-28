#include <bits/stdc++.h>
using namespace std;

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
// #define OVERFLOW -2

typedef int status;
typedef int ElemType; // Data element type definition

#define LIST_INIT_SIZE 100
#define LISTINCREMENT 10
typedef struct LNode
{ // Definition of a singly linked list node
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;

status InitList(LinkList &L)
// If linear list L does not exist, create an empty list and return OK, otherwise return INFEASIBLE.
{
    if (L != NULL)
        return INFEASIBLE;
    LinkList n = (LinkList)malloc(sizeof(LNode));
    n->next = NULL;
    L = n;
    return OK;
}

status DestroyList(LinkList &L)
// If linear list L exists, destroy it, free memory, and return OK, otherwise return INFEASIBLE.
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
// If linear list L exists, delete all elements in it and return OK, otherwise return INFEASIBLE.
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
// If list L exists, check if it's empty; return TRUE if empty, FALSE otherwise; if list doesn't exist, return INFEASIBLE.
{
    if (!L)
        return INFEASIBLE;
    if (!L->next)
        return TRUE;
    return FALSE;
}

int ListLength(LinkList L)
// If list L exists, return its length, otherwise return INFEASIBLE.
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
// If list L exists, get its i-th element, save in e, return OK; if i is invalid, return ERROR; if list doesn't exist, return INFEASIBLE.
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
// If list L exists, find position of element e; if e doesn't exist, return ERROR; if list doesn't exist, return INFEASIBLE.
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
// If list L exists, get the predecessor of element e, save in pre, return OK; if no predecessor, return ERROR; if list doesn't exist, return INFEASIBLE.
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
// If list L exists, get the successor of element e, save in next, return OK; if no successor, return ERROR; if list doesn't exist, return INFEASIBLE.
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
// If list L exists, insert element e before the i-th element, return OK; if position is invalid, return ERROR; if list doesn't exist, return INFEASIBLE.
{
    if (i < 1)
        return ERROR;
    if (!L)
        return INFEASIBLE;
    LinkList t = L;
    int c = 0;
    while (t && c < i - 1)
    {
        c++;
        t = t->next;
    }
    if (c < i - 1 || !t)
        return ERROR;
    LinkList n = (LinkList)malloc(sizeof(LNode));
    n->next = t->next;
    t->next = n;
    n->data = e;
    return OK;
}

status ListDelete(LinkList &L, int i, ElemType &e)
// If list L exists, delete its i-th element, save it in e, return OK; if position is invalid, return ERROR; if list doesn't exist, return INFEASIBLE.
{
    if (i < 1)
        return ERROR;
    if (!L)
        return INFEASIBLE;
    LinkList t = L, p = NULL;
    int c = 0;
    while (t && c < i - 1)
    {
        c++;
        t = t->next;
    }
    if (c < i - 1 || !t || !t->next)
        return ERROR;
    p = t->next;
    e = p->data;
    t->next = p->next;
    free(p);
    return OK;
}

status ListTraverse(LinkList L)
// If list L exists, display its elements one by one with spaces between them, return OK; if list doesn't exist, return INFEASIBLE.
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
// If list L exists, write its elements to file FileName, return OK, otherwise return INFEASIBLE.
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
// If list L doesn't exist, read data from file FileName into list L, return OK, otherwise return INFEASIBLE.
{
    if (L)
        return INFEASIBLE;
    LinkList n = (LinkList)malloc(sizeof(LNode));
    n->next = NULL;
    L = n;
    FILE *fp = fopen(FileName, "r");
    if (fp == NULL)
        return ERROR;
    LinkList t = L;
    ElemType e;
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
{
    if (!L)
        return INFEASIBLE;
    LinkList p = L->next, q = NULL, r = NULL;
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
{
    if (!L || (n == 0))
        return INFEASIBLE;
    if (n < 0)
        return ERROR;
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
{
    if (!L)
        return INFEASIBLE;
    if (!L->next || !L->next->next)
        return OK;
    LinkList p = L->next, q = NULL, r = NULL;
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
{
    // Changed from vector<LinkList> to vector<pair<string,LinkList>>
    vector<pair<string, LinkList>> lists;
    LinkList L = NULL;
    lists.push_back({"Default", NULL}); // Default list with name
    int op = 1, size = 1;
    int currentList = 0; // Index of currently active list

    while (op)
    {
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
        printf("   21. SetList\n");
        printf("-----------------------Exit----------------------\n");
        printf("    0. Exit\n");
        printf("Please select your operation [0~21]:\n");

        scanf("%d", &op);
        getchar();
        switch (op)
        {
        case 1: // InitList
            if (InitList(lists[currentList].second) == OK)
                printf("List '%s' created successfully!\n", lists[currentList].first.c_str());
            else
                printf("List creation failed!\n");
            break;

        case 2: // DestroyList
            if (DestroyList(lists[currentList].second) == OK)
                printf("List '%s' destroyed successfully!\n", lists[currentList].first.c_str());
            else
                printf("List does not exist!\n");
            break;

        case 3: // ClearList
            if (ClearList(lists[currentList].second) == OK)
                printf("List '%s' cleared successfully!\n", lists[currentList].first.c_str());
            else
                printf("List does not exist!\n");
            break;

        case 4: // ListEmpty
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

        case 5: // ListLength
        {
            int len = ListLength(lists[currentList].second);
            if (len == INFEASIBLE)
                printf("List does not exist!\n");
            else
                printf("The length of list '%s' is %d\n", lists[currentList].first.c_str(), len);
            break;
        }

        case 6: // GetElem
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

        case 7: // LocateElem
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

        case 8: // PriorElem
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

        case 9: // NextElem
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

        case 10: // ListInsert
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

        case 11: // ListDelete
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

        case 12: // ListTraverse
            printf("Elements in list '%s':\n", lists[currentList].first.c_str());
            if (ListTraverse(lists[currentList].second) == INFEASIBLE)
                printf("List does not exist!\n");
            break;

        case 13: // ReverseList
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

        case 14: // RemoveNthFromEnd
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

        case 15: // SortList
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

        case 16: // SaveList
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

        case 17: // LoadList
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

        case 18: // AddNewList
        {
            char listName[30];
            printf("Enter name for the new list: ");
            scanf("%s", listName);
            getchar();

            // Check if list name already exists
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

        case 19: // SwitchList
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

        case 20: // ListsInfo
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

        case 21: // reset current list elements
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
            getchar();
            for (int i = 0; i < len; i++)
            {
                int elem;
                scanf("%d", &elem);
                getchar();
                ListInsert(lists[currentList].second, i + 1, elem);
            }
            printf("Elements added to list '%s' successfully!\n", lists[currentList].first.c_str());
            break;
        }

        case 0: // Exit
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
