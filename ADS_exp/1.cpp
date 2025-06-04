#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2

typedef int status;
typedef int ElemType; // Data element type definition

#define LIST_INIT_SIZE 100
#define LISTINCREMENT 10
typedef struct
{ // Sequential list definition
    ElemType *elem;
    int length;
    int listsize;
} SqList;
typedef struct
{ // List management structure
    struct
    {
        char name[30];
        SqList L;
    } elem[10];
    int length;
    int listsize;
} LISTS;

status InitList(SqList &L)
// Initializes an empty sequential list
{
    if (L.elem != NULL)
        return INFEASIBLE;
    L.length = 0;
    L.listsize = LIST_INIT_SIZE;
    L.elem = (ElemType *)malloc(LIST_INIT_SIZE * sizeof(ElemType));
    return OK;
}

status DestroyList(SqList &L)
// Destroys the list and frees memory
{
    if (L.elem == NULL)
        return INFEASIBLE;
    free(L.elem);
    L.elem = NULL, L.length = 0;
    return OK;
}

status ClearList(SqList &L)
// Clears all elements from the list
{
    if (L.elem == NULL)
        return INFEASIBLE;
    L.length = 0;
    return OK;
}

status ListEmpty(SqList &L)
// Checks if the list is empty
{
    if (L.elem == NULL)
        return INFEASIBLE;
    return L.length == 0 ? TRUE : FALSE;
}

status ListLength(SqList &L)
// Returns the number of elements in the list
{
    if (L.elem == NULL)
        return INFEASIBLE;
    return L.length;
}

status GetElem(SqList &L, int i, ElemType &e)
// Gets the element at position i (1-based indexing)
{
    if (L.elem == NULL)
        return INFEASIBLE;
    if (i < 1 or i > L.length)
        return ERROR;
    e = L.elem[i - 1];
    return OK;
}

int LocateElem(SqList &L, ElemType &e)
// Finds the position of element e in the list
{
    if (L.elem == NULL)
        return INFEASIBLE;
    for (int i = 0; i < L.length; i++)
        if (L.elem[i] == e)
            return i + 1;
    return ERROR;
}

status PriorElem(SqList &L, ElemType &e, ElemType &pre)
// Gets the predecessor of element e
{
    if (L.elem == NULL)
        return INFEASIBLE;
    if (L.elem[0] == e)
        return ERROR;
    for (int i = 1; i < L.length; i++)
        if (L.elem[i] == e)
        {
            pre = L.elem[i - 1];
            return OK;
        }
    return ERROR;
}

status NextElem(SqList &L, ElemType &e, ElemType &next)
// Gets the successor of element e
{
    if (L.elem == NULL)
        return INFEASIBLE;
    for (int i = 0; i < L.length - 1; i++)
        if (L.elem[i] == e)
        {
            next = L.elem[i + 1];
            return OK;
        }
    return ERROR;
}

status ListInsert(SqList &L, int i, ElemType &e)
// Inserts element e at position i, expands array if necessary
{
    if (L.elem == NULL)
        return INFEASIBLE;
    if (i < 1 || i > L.length + 1)
        return ERROR;
    if (L.length == L.listsize)
    {
        L.elem = (ElemType *)realloc(L.elem, (L.listsize + LISTINCREMENT) * sizeof(ElemType));
        L.listsize += LISTINCREMENT;
    }
    L.length++;
    for (int j = L.length - 1; j > i - 2; j--)
        L.elem[j + 1] = L.elem[j];
    L.elem[i - 1] = e;
    return OK;
}

status ListDelete(SqList &L, int i, ElemType &e)
// Deletes element at position i and returns it in e
{
    if (L.elem == NULL)
        return INFEASIBLE;
    if (i < 1 || i > L.length)
        return ERROR;
    e = L.elem[i - 1];
    for (int j = i - 1; j < L.length - 1; j++)
        L.elem[j] = L.elem[j + 1];
    L.length--;
    return OK;
}

status ListTraverse(SqList &L)
// Displays all elements in the list
{
    if (L.elem == NULL)
        return INFEASIBLE;
    printf("\n------------------all elements ----------------\n");
    for (int j = 0; j < L.length; j++)
        printf("%d%s", L.elem[j], (j == L.length - 1 ? "" : " "));
    printf("\n--------------------- end ---------------------\n");
    return OK;
}

int MaxSubArray(SqList &L)
// Finds maximum sum of contiguous subarray using Kadane's algorithm
{
    if (L.elem == NULL || L.length < 1)
        return ERROR;
    int max = L.elem[0], sum = L.elem[0];
    for (int i = 1; i < L.length; i++)
    {
        if (sum < 0)
            sum = L.elem[i];
        else
            sum += L.elem[i];
        if (sum > max)
            max = sum;
    }
    return max;
}

int SubArrayNum(SqList &L, int k)
// Counts number of contiguous subarrays with sum equal to k
{
    if (L.elem == NULL || L.length < 1)
        return ERROR;
    int count = 0, sum = 0;
    for (int i = 0; i < L.length; i++)
    {
        sum = 0;
        for (int j = i; j < L.length; j++)
        {
            sum += L.elem[j];
            if (sum == k)
                count++;
        }
    }
    return count;
}

status SortList(SqList &L)
// Sorts the list in ascending order using bubble sort
{
    if (L.elem == NULL || L.length < 1)
        return ERROR;
    for (int i = 0; i < L.length - 1; i++)
        for (int j = 0; j < L.length - i - 1; j++)
            if (L.elem[j] > L.elem[j + 1])
            {
                int t = L.elem[j];
                L.elem[j] = L.elem[j + 1];
                L.elem[j + 1] = t;
            }
    return OK;
}

status SaveList(SqList &L, char FileName[])
// Saves the list to a text file
{
    if (L.elem == NULL)
        return INFEASIBLE;
    FILE *fp = fopen(FileName, "w");
    if (fp == NULL)
    {
        printf("File open error!\n");
        return ERROR;
    }
    for (int j = 0; j < L.length; j++)
        fprintf(fp, "%d%s", L.elem[j], (j == L.length - 1 ? "" : " "));
    fclose(fp);
    return OK;
}

status LoadList(SqList &L, char FileName[])
// Loads list data from a text file
{
    if (L.elem != NULL)
        return INFEASIBLE;
    L.length = 0;
    L.listsize = LIST_INIT_SIZE;
    L.elem = (ElemType *)malloc(LIST_INIT_SIZE * sizeof(ElemType));
    FILE *fp = fopen(FileName, "r");
    if (fp == NULL)
    {
        printf("File open error!\n");
        return ERROR;
    }
    ElemType t;
    int i = 0;
    while (fscanf(fp, "%d", &t) == 1)
    {
        L.elem[i++] = t;
        if (i == L.listsize)
        {
            L.elem = (ElemType *)realloc(L.elem, (L.listsize + LISTINCREMENT) * sizeof(ElemType));
            L.listsize += LISTINCREMENT;
        }
    }
    L.length = i;
    fclose(fp);
    return OK;
}

status AddList(LISTS &Lists, char ListName[])
// Adds a new named list to the list manager
{
    if (Lists.length >= 10)
    {
        printf("Lists is full!\n");
        return ERROR;
    }
    Lists.elem[Lists.length].L.elem = NULL;
    strcpy(Lists.elem[Lists.length].name, ListName);
    InitList(Lists.elem[Lists.length].L);
    Lists.length++;
    return OK;
}

status RemoveList(LISTS &Lists, char ListName[])
// Removes a named list from the list manager
{
    int c = 0, i = 0;
    for (; i < Lists.length; i++)
    {
        if (strcmp(ListName, Lists.elem[i].name) == 0)
        {
            DestroyList(Lists.elem[i].L);
            c++;
            break;
        }
    }
    if (c == 0)
        return ERROR;
    for (; i < Lists.length - 1; i++)
        Lists.elem[i] = Lists.elem[i + 1];
    Lists.length--;
    return OK;
}

int LocateList(LISTS &Lists, char ListName[])
// Finds the position of a named list in the manager
{
    int i = 0;
    for (; i < Lists.length; i++)
        if (strcmp(ListName, Lists.elem[i].name) == 0)
            return i + 1;
    return 0;
}

int main(void)
// Main function providing interactive menu for list operations
{
    SqList L{0, 0, NULL};
    LISTS Lists;
    int op = 1;
    char lastnme[30] = {0}; // Tracks last switched list name
    while (op)
    {
        // Display menu
        printf("\n\n");
        printf("      Menu for Linear Table On Sequence Structure \n");
        printf("-----------------------Basic---------------------\n");
        printf("        1. InitList             2. DestroyList\n");
        printf("        3. ClearList            4. ListEmpty\n");
        printf("        5. ListLength           6. GetElem\n");
        printf("        7. LocateElem           8. PriorElem\n");
        printf("        9. NextElem            10. ListInsert\n");
        printf("       11. ListDelete          12. ListTraverse\n");
        printf("----------------------Advanced---------------------\n");
        printf("       13. MaxSubArray         14. SubArrayNum\n");
        printf("       15. SortList            16. SaveList\n");
        printf("       17. LoadList            18. InitManagerLists\n");
        printf("       19. AddNewList          20. RemoveList\n");
        printf("       21. LocateList          22. RandomSetList\n");
        printf("       23. SetList             24. ShowAllLists\n");
        printf("       25. SwitchList          26. SyncList\n");
        printf("-----------------------Exit---------------------\n");
        printf("        0. Exit\n");
        printf("Please select your operation [0~26]:\n");
        scanf("%d", &op);
        switch (op)
        {
        case 1: // Initialize list
            if (InitList(L) == OK)
                printf("List created successfully!\n");
            else
                printf("List creation failed!\n");
            getchar();
            break;
        case 2: // Destroy list
            if (DestroyList(L) == OK)
                printf("List destroyed successfully!\n");
            else
                printf("List does not exist!\n");
            getchar();
            break;
        case 3: // Clear list
            if (ClearList(L) == OK)
                printf("List cleared successfully!\n");
            else
                printf("List does not exist!\n");
            getchar();
            break;
        case 4: // Check if list is empty
            switch (ListEmpty(L))
            {
            case TRUE:
                printf("The list is empty!\n");
                break;
            case FALSE:
                printf("The list is not empty!\n");
                break;
            case INFEASIBLE:
                printf("The list does not exist!\n");
                break;
            }
            getchar();
            break;
        case 5: // Get list length
        {
            int tmp = ListLength(L);
            if (tmp == INFEASIBLE)
                printf("The list does not exist!\n");
            else
                printf("The length of the list is %d\n", tmp);
            getchar();
            break;
        }
        case 6: // Get element at position
        {
            int tmp, key;
            printf("Please enter the position of the element you want to get: \n");
            scanf("%d", &key);
            switch (GetElem(L, key, tmp))
            {
            case OK:
                printf("The wanted element is %d\n", tmp);
                break;
            case ERROR:
                printf("The position is invalid!\n");
                break;
            case INFEASIBLE:
                printf("The list does not exist!\n");
                break;
            }
            getchar();
            break;
        }
        case 7: // Locate element
        {
            int t, tm;
            printf("Please enter the element you want to find: \n");
            scanf("%d", &tm);
            t = LocateElem(L, tm);
            if (t == INFEASIBLE)
                printf("The list does not exist!\n");
            else if (t == ERROR)
                printf("The element is not found!\n");
            else
                printf("The position of the element is %d\n", t);
            getchar();
            break;
        }
        case 8: // Get predecessor
        {
            int tmp, pre, t;
            printf("Please enter the element you want to find the predecessor of: \n");
            scanf("%d", &tmp);
            t = PriorElem(L, tmp, pre);
            if (t == INFEASIBLE)
                printf("The list does not exist!\n");
            else if (t == ERROR)
                printf("The element has no predecessor!\n");
            else
                printf("The predecessor of the element is %d\n", pre);
            getchar();
            break;
        }
        case 9: // Get successor
        {
            int tmp, next, t;
            printf("Please enter the element you want to find the successor of: \n");
            scanf("%d", &tmp);
            t = NextElem(L, tmp, next);
            if (t == INFEASIBLE)
                printf("The list does not exist!\n");
            else if (t == ERROR)
                printf("The element has no successor!\n");
            else
                printf("The successor of the element is %d\n", next);
            getchar();
            break;
        }
        case 10: // Insert element
        {
            int tmp, key, t;
            printf("Please enter the position where you want to insert the element: \n");
            scanf("%d", &key);
            printf("Please enter the element you want to insert: \n");
            scanf("%d", &tmp);
            t = ListInsert(L, key, tmp);
            if (t == OK)
                printf("Element inserted successfully!\n");
            else if (t == ERROR)
                printf("The position is invalid!\n");
            else if (t == INFEASIBLE)
                printf("The list does not exist!\n");
            else
                printf("Element insertion failed!\n");
            getchar();
            break;
        }
        case 11: // Delete element
        {
            int tmp, key, t;
            printf("Please enter the position of the element you want to delete: \n");
            scanf("%d", &key);
            t = ListDelete(L, key, tmp);
            if (t == OK)
            {
                printf("Element deleted successfully!\n");
                printf("The deleted element is %d\n", tmp);
            }
            else if (t == ERROR)
                printf("The position is invalid!\n");
            else if (t == INFEASIBLE)
                printf("The list does not exist!\n");
            else
                printf("Element deletion failed!\n");
            getchar();
            break;
        }
        case 12: // Display all elements
        {
            int t = ListTraverse(L);
            if (t == OK)
                printf("List displayed successfully!\n");
            if (t == INFEASIBLE)
                printf("The list does not exist!\n");
            getchar();
            break;
        }
        case 13: // Find maximum subarray sum
        {
            int max = MaxSubArray(L);
            if (max == ERROR)
                printf("The list is empty or does not exist!\n");
            else
                printf("The maximum sum of contiguous subarray is %d\n", max);
            getchar();
            break;
        }
        case 14: // Count subarrays with sum k
        {
            int k, count;
            printf("Please enter the value of k(the sum of subarray): \n");
            scanf("%d", &k);
            count = SubArrayNum(L, k);
            if (count == ERROR)
                printf("The list is empty or does not exist!\n");
            else
                printf("The number of continuous subarrays with sum equal to %d is %d\n", k, count);
            getchar();
            break;
        }
        case 15: // Sort list
        {
            int t = SortList(L);
            if (t == OK)
                printf("List sorted in ascending order successfully!\n");
            else if (t == ERROR)
                printf("The list is empty or does not exist!\n");
            getchar();
            break;
        }
        case 16: // Save list to file
        {
            char FileName[30];
            printf("Please enter the file name to save the list: \n");
            scanf("%s", FileName);
            int t = SaveList(L, FileName);
            if (t == OK)
                printf("List saved successfully!\n");
            else if (t == INFEASIBLE)
                printf("The list does not exist!\n");
            getchar();
            break;
        }
        case 17: // Load list from file
        {
            char FileName[30];
            printf("Please enter the file name to load the list: \n");
            scanf("%s", FileName);
            int tm = LoadList(L, FileName);
            if (tm == OK)
                printf("List loaded successfully!\n");
            else if (tm == INFEASIBLE)
                printf("The list already exists!\n");
            getchar();
            break;
        }
        case 18: // Initialize list manager
            Lists.length = 0;
            Lists.listsize = 10;
            printf("List manager initialized successfully!\n");
            getchar();
            break;
        case 19: // Add new list to manager
        {
            if (Lists.listsize != 10)
            {
                printf("The list manager is not initialized!\n");
                getchar();
                break;
            }
            char ListName[30];
            printf("Please enter the name of the new list: ");
            scanf("%s", ListName);
            int dup = LocateList(Lists, ListName);
            if (dup != 0)
            {
                printf("The list already exists!\n");
                getchar();
                break;
            }
            int t = AddList(Lists, ListName);
            if (t == OK)
                printf("List added successfully!\n");
            else if (t == ERROR)
                printf("The list manager is full!\n");
            getchar();
            break;
        }
        case 20: // Remove list from manager
        {
            if (Lists.listsize != 10)
            {
                printf("The list manager is not initialized!\n");
                getchar();
                break;
            }
            char ListName[30];
            printf("Please enter the name of the list to remove: \n");
            scanf("%s", ListName);
            int t = RemoveList(Lists, ListName);
            if (t == OK)
                printf("List removed successfully!\n");
            else if (t == ERROR)
                printf("List not found!\n");
            getchar();
            break;
        }
        case 21: // Locate list in manager
        {
            if (Lists.listsize != 10)
            {
                printf("The list manager is not initialized!\n");
                getchar();
                break;
            }
            char ListName[30];
            printf("Please enter the name of the list to locate: \n");
            scanf("%s", ListName);
            int t = LocateList(Lists, ListName);
            if (t == 0)
                printf("List not found!\n");
            else
                printf("Position of the list: %d\n", t);
            getchar();
            break;
        }
        case 22: // Randomly generate list data
        {
            printf("This will reset the list randomly, are you sure?\n (1: Yes, 2: No): ");
            int n;
            scanf("%d", &n);
            if (n == 2)
            {
                printf("Operation cancelled!\n");
                getchar();
                break;
            }
            else if (n != 1)
            {
                printf("Invalid input!\n");
                printf("Operation cancelled!\n");
                getchar();
                break;
            }
            else
            {
                if (L.elem == NULL)
                {
                    printf("The list does not exist, please initialize!\n");
                    getchar();
                    break;
                }
                ClearList(L);
                printf("Please enter the number of elements in the list: \n");
                scanf("%d", &n);
                while (n > 0)
                {
                    int tmp = rand() % 114514;
                    ListInsert(L, 1, tmp);
                    n--;
                }
                printf("List set successfully!\n");
                getchar();
            }
            break;
        }
        case 23: // Manually set list data
        {
            if (L.elem == NULL)
            {
                printf("The list does not exist, please initialize!\n");
                getchar();
                break;
            }
            printf("This will reset the list, are you sure?\n (1: Yes, 2: No): ");
            int n, i = 1;
            scanf("%d", &n);
            if (n == 2)
            {
                printf("Operation cancelled!\n");
                getchar();
                break;
            }
            else if (n != 1)
            {
                printf("Invalid input!\n");
                printf("Operation cancelled!\n");
                getchar();
                break;
            }
            ClearList(L);
            printf("Please enter the number of elements in the list: \n");
            scanf("%d", &n);
            printf("Please enter the elements: ");
            while (i < n + 1)
            {
                int tmp;
                scanf("%d", &tmp);
                ListInsert(L, i, tmp);
                i++;
            }
            printf("List set successfully!\n");
            getchar();
            break;
        }
        case 24: // Show all managed lists
        {
            if (Lists.listsize != 10)
            {
                printf("The list manager is not initialized!\n");
                getchar();
                break;
            }
            if (Lists.length == 0)
                printf("No lists available!\n");
            else
            {
                printf("All lists:\n");
                for (int i = 0; i < Lists.length; i++)
                    printf("%s\n", Lists.elem[i].name);
            }
            getchar();
            break;
        }
        case 25: // Switch to different list
        {
            if (Lists.listsize != 10)
            {
                printf("The list manager is not initialized!\n");
                getchar();
                break;
            }
            if (Lists.length == 0)
            {
                printf("No lists available!\n");
                getchar();
                break;
            }
            // Save current list state
            if (lastnme[0] != 0)
            {
                int t = LocateList(Lists, lastnme);
                if (0 < t && t < 11)
                {
                    Lists.elem[t - 1].L = L;
                }
            }
            char ListName[30];
            printf("Please enter the name of the list to switch to: \n");
            scanf("%s", ListName);
            strcpy(lastnme, ListName);
            int t = LocateList(Lists, ListName);
            if (t == 0)
                printf("List not found!\n");
            else
                L = Lists.elem[t - 1].L;
            getchar();
            break;
        }
        case 26: // Synchronize current list with manager
        {
            if (L.elem == NULL)
            {
                printf("The list does not exist!\n");
                getchar();
                break;
            }
            if (Lists.listsize != 10)
            {
                printf("The list manager is not initialized!\n");
                getchar();
                break;
            }
            if (Lists.length == 0)
            {
                printf("No lists available!\n");
                getchar();
                break;
            }
            char ListName[30];
            printf("Please enter the name of the list to synchronize: \n");
            scanf("%s", ListName);
            int t = LocateList(Lists, ListName);
            if (t == 0)
                printf("List not found!\n");
            else
                Lists.elem[t - 1].L = L;
            getchar();
            break;
        }
        } // end of switch
    } // end of while
    printf("Thank you for using this system!\n");
    return 0;
}
