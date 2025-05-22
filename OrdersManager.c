#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define MAX_NAME 50
#define MAX_NOTE 100

//Trang thai don hang
typedef enum {
    isServing = 1,
    hasBeenPaid = 2,
    canceled = 3,
} State;


//Mon an
typedef struct Dish {
    char id[MAX_NAME];
    char time[20];
    int numOrdered; //so luong suat an duoc order
    int numTransfered; //so luong suat an da hoan thanh
    char updateTime[20];
    char note[MAX_NOTE];

    struct Dish* next;
} Dish;

//Don hang, luu duoi dang mot danh sach lien ket don
typedef struct Order {
    char time[20];
    char employeeName[MAX_NAME];
    int tableId;
    Dish* dishList;
    int totalDishes; //tong so luong mon an khac nhau da order
    int totalMeals; //tong so luong suat an da order
    int totalDishesDone; //tong so luong mon an da hoan thanh
    int totalMealsDone; //tong so luong suat an da hoan thanh
    char updateTime[20];
    State state;
    struct Order* next;
} Order;

Order* root = NULL;

char query[MAX_NOTE];

int digitCount(int A) {
    if (A == 0)
        return 1;

    int cnt = 0;
    while (A > 0) {
        ++cnt;
        A /= 10;
    }

    return cnt;
}

Order* search_order(int tableId) {
    if (root == NULL) {
        return NULL;
    }

    Order* tmp = root;

    while (tmp != NULL) {
        if (tmp->tableId == tableId)
            break;
        tmp = tmp->next;
    }

    if (tmp == NULL || tmp->state == 2) {
        return NULL;
    }

    return tmp;
}

void print_order(int tableId) {
    Order* table = search_order(tableId);

    if (table == NULL) {
        printf("Chua co order\n");
        return;
    }

    printf("Nhan vien phuc vu: %s\n", table->employeeName);
    printf("Danh sach cac mon an:\n");
    Dish* tmp = table->dishList;

    for (int i = 1; i <= 250; ++i)
            printf("-");
    printf("\n");

    while (tmp != NULL) {
        printf("|%s:", tmp->id);
        for (int i = 1; i <= 249 - strlen(tmp->id); ++i)
            printf(" ");
        printf("|\n");

        printf("|    - So luong suat an da dat: %d", tmp->numOrdered);
        for (int i = 1; i <= 219 - digitCount(tmp->numOrdered); ++i)
            printf(" ");
        printf("|\n");

        printf("|    - So luong suat an da hoan thanh: %d", tmp->numTransfered);
        for (int i = 1; i <= 212 - digitCount(tmp->numTransfered); ++i)
            printf(" ");
        printf("|\n");

        printf("|    - Ghi chu: %s", tmp->note);
        for (int i = 1; i <= 235 - strlen(tmp->note); ++i)
            printf(" ");
        printf("|\n");

        for (int i = 1; i <= 250; ++i)
            printf("-");
        printf("\n");

        tmp = tmp->next;
    }
}

void create_order(int tableId, char* employeeName, char* time) {
    if (search_order(tableId) != NULL)
        return;

    Order* new_order = (Order*)malloc(sizeof(Order));

    Order* tmp = root;

    strcpy(new_order->time, time);
    strcpy(new_order->employeeName, employeeName);
    new_order->tableId = tableId;
    new_order->state = 1;

    int replace = 0;

    while (tmp->next != NULL) {
        Order* nxt = tmp->next;
        if (tmp->next->tableId == tableId)
            tmp->next = new_order;
        if (tmp->tableId == tableId) {
            new_order->next = tmp->next;
            replace = 1;
            break;
        }
        tmp = nxt;
    }

    if (replace == 0) {
        tmp->next = new_order;
    }

    return;
}

int add_dish(int tableId, int dishId, int number, char* Note) {
    Order* table = search_order(tableId);
    
}

int main() {
    char tmp[2];
    while (scanf("%s", tmp) == 1) {

    }
}
