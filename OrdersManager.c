#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define MAX_NAME 50
#define MAX_NOTE 100

typedef enum {
    isServing = 1,
    hasBeenPaid = 2,
    canceled = 3,
} State;

typedef struct Dish {
    char id[MAX_NAME];
    char time[20];
    int numOrdered;
    int numTransfered;
    char updateTime[20];
    char note[MAX_NOTE];
    State state;

    struct Dish* next;
} Dish;

typedef struct Order {
    char time[20];
    char employeeName[MAX_NAME];
    int tableId;
    Dish* dishList;
    int totalDishes;
    int totalMeals;
    int totalDishesDone;
    int totalMealsDone;
    char updateTime[20];
    State state;
    struct Order* next;
} Order;

Order* root = NULL;

char query[500 * MAX_NOTE];

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

    if (tmp == NULL || tmp->state == hasBeenPaid) {
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

    if (table->state == hasBeenPaid) {
        printf("Da hoan thanh\n");
        return;
    }

    if (table->state == canceled) {
        printf("Da huy\n");
        return;
    }

    printf("Nhan vien phuc vu: %s\n", table->employeeName);
    printf("Danh sach cac mon an:\n");
    Dish* tmp = table->dishList;

    for (int i = 1; i <= 252; ++i)
            printf("-");
    printf("\n");

    while (tmp != NULL) {
        if (tmp->state != canceled) {
            printf("|%s:", tmp->id);
            for (int i = 1; i <= 249 - (int)strlen(tmp->id); ++i)
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
            for (int i = 1; i <= 235 - (int)strlen(tmp->note); ++i)
                printf(" ");
            printf("|\n");

            for (int i = 1; i <= 252; ++i)
                printf("-");
            printf("\n");
        }

        tmp = tmp->next;
    }
}

Order* create_order(int tableId, char* employeeName, char* time) {
    if (search_order(tableId) != NULL)
        return NULL;

    Order* new_order = (Order*)malloc(sizeof(Order));
    if (new_order == NULL) return NULL;

    strcpy(new_order->time, time);
    strcpy(new_order->employeeName, employeeName);
    new_order->tableId = tableId;
    new_order->state = isServing;
    new_order->dishList = NULL;
    new_order->totalDishes = 0;
    new_order->totalMeals = 0;
    new_order->totalDishesDone = 0;
    new_order->totalMealsDone = 0;
    new_order->updateTime[0] = 0;
    new_order->next = NULL;

    if (root == NULL) {
        root = new_order;
        return new_order;
    }

    Order* tmp = root;
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }
    tmp->next = new_order;

    return new_order;
}

char* extract_quoted_string(char* p, char* buf) {
    while (*p && *p != '"') p++;  // tìm dấu "
    if (*p == '"') p++;           // bỏ qua dấu "
    int i = 0;
    while (*p && *p != '"' && i < MAX_NOTE-1) {
        buf[i++] = *p++;
    }
    buf[i] = 0;
    if (*p == '"') p++;           // bỏ qua dấu đóng "
    return p;
}

int add_dish(int tableId, char* dishId, int number, char* Note, char* employeeName) {
    time_t t = time(NULL);
    struct tm *time_info = localtime(&t);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    Order* table = search_order(tableId);

    if (table == NULL) {
        table = create_order(tableId, employeeName, buffer);
        if (table == NULL) return 0;
    } else {
        strcpy(table->updateTime, buffer);
    }

    if (table->dishList == NULL) {
        Dish* new_dish = (Dish*)malloc(sizeof(Dish));
        if (new_dish == NULL) return 0;

        strcpy(new_dish->time, buffer);
        strcpy(new_dish->note, Note);
        strcpy(new_dish->id, dishId);
        new_dish->numOrdered = number;
        new_dish->numTransfered = 0;
        new_dish->state = isServing;
        new_dish->updateTime[0] = 0;
        new_dish->next = NULL;

        table->dishList = new_dish;
        table->totalDishes = 1;
        table->totalMeals = number;
        table->state = isServing;

        return 1;
    }

    Dish* tmp = table->dishList;
    Dish* prev = NULL;

    while (tmp != NULL) {
        if (strcmp(tmp->id, dishId) == 0) {
            tmp->numOrdered += number;
            tmp->state = isServing;
            strcpy(tmp->note, Note);
            strcpy(tmp->updateTime, buffer);

            table->totalMeals += number;
            table->state = isServing;
            strcpy(table->updateTime, buffer);

            return 1;
        }
        prev = tmp;
        tmp = tmp->next;
    }

    Dish* new_dish = (Dish*)malloc(sizeof(Dish));
    if (new_dish == NULL) return 0;

    strcpy(new_dish->time, buffer);
    strcpy(new_dish->note, Note);
    strcpy(new_dish->id, dishId);
    new_dish->numOrdered = number;
    new_dish->numTransfered = 0;
    new_dish->state = isServing;
    new_dish->updateTime[0] = 0;
    new_dish->next = NULL;

    prev->next = new_dish;

    table->totalDishes++;
    table->totalMeals += number;
    table->state = isServing;
    strcpy(table->updateTime, buffer);

    return 1;
}

int update_dish(int tableId, char* dishId, int number) {
    time_t t = time(NULL);
    struct tm *time_info = localtime(&t);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    Order* table = search_order(tableId);

    if (table == NULL || table->state == canceled) 
        return 0;

    if (table->dishList == NULL)
        return 0;

    Dish* tmp = table->dishList;

    while (tmp != NULL) {
        if (strcmp(tmp->id, dishId) == 0) {
            if (tmp->numOrdered >= number) {
                tmp->numTransfered += number;
                table->totalMealsDone += number;
                
                if (tmp->numOrdered == tmp->numTransfered) {
                    tmp->state = hasBeenPaid;
                    table->totalDishesDone++;
                    if (table->totalDishesDone == table->totalDishes) {
                        table->state = hasBeenPaid;
                    }
                }
                strcpy(tmp->updateTime, buffer);
                strcpy(table->updateTime, buffer);
                return 1;
            }
        }
        tmp = tmp->next;
    }

    return 0;
}

int cancel_dish(int tableId, char* dishId, char* Note) {
    time_t t = time(NULL);
    struct tm *time_info = localtime(&t);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    Order* table = search_order(tableId);

    if (table == NULL) {
        return 0;
    }

    Dish* tmp = table->dishList;

    while (tmp != NULL) {
        if (strcmp(tmp->id, dishId) == 0) {
            if (tmp->numTransfered == 0) {
                table->totalDishes--;
                table->totalMeals -= tmp->numOrdered;
                tmp->numOrdered = 0;
                tmp->numTransfered = 0;
                tmp->state = canceled;
                strcpy(tmp->note, "Khach huy");
                strcpy(tmp->updateTime, buffer);
                strcpy(table->updateTime, buffer);
                return 1;
            }
        }
        tmp = tmp->next;
    }

    return 0;
}

int cancel_order(int tableId) {
    time_t t = time(NULL);
    struct tm *time_info = localtime(&t);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    Order* table = search_order(tableId);
    
    if (table == NULL || table->totalMealsDone > 0) {
        return 0;
    }

    table->totalDishes = 0;
    table->totalMeals = 0;
    table->totalMealsDone = 0;
    table->totalDishesDone = 0;
    table->state = canceled;
    strcpy(table->updateTime, buffer);
    
    return 1;
}

void create_bill(int tableId) {
    Order* table = search_order(tableId);

    if (table == NULL) 
        return;
    if (table->state == canceled)
        return;

    if (strlen(table->updateTime) == 0)
        printf("%s\n", table->time);
    else
        printf("%s\n", table->updateTime);

    printf("Ban %d\n", table->tableId);

    for (int i = 1; i <= 252; ++i)
        printf("-");
    printf("\n");

    Dish* tmp = table->dishList;

    for (int i = 1; i <= 252; ++i)
            printf("-");
    printf("\n");

    while (tmp != NULL) {
        if (tmp->state != canceled) {
            printf("|%s:", tmp->id);
            for (int i = 1; i <= 249 - (int)strlen(tmp->id); ++i)
                printf(" ");
            printf("|\n");

            printf("|    - So luong suat an da hoan thanh: %d", tmp->numTransfered);
            for (int i = 1; i <= 212 - digitCount(tmp->numTransfered); ++i)
                printf(" ");
            printf("|\n");

            printf("|    - Ghi chu: %s", tmp->note);
            for (int i = 1; i <= 235 - (int)strlen(tmp->note); ++i)
                printf(" ");
            printf("|\n");

            for (int i = 1; i <= 252; ++i)
                printf("-");
            printf("\n");
        }

        tmp = tmp->next;
    }

    printf("Tong so mon: %d\n", table->totalDishesDone);
    printf("Tong so suat an: %d\n", table->totalMealsDone);
}

char buffer[10000 * MAX_NOTE], query[500 * MAX_NOTE], s[500 * MAX_NOTE], Time[20], dishId[MAX_NAME], Note[MAX_NOTE];
int tableId, quantity;

int main() {
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "? %s", query) == 1) {
            if (strcmp(query, "create_order") == 0) {
                while(1) {
                    if (!fgets(buffer, sizeof(buffer), stdin)) break;

                    char employeeName[MAX_NAME];
                    int tableId;
                    char time[100];

                    char *p = buffer;

                    sscanf(p, "%s", employeeName);
                    if (strcmp(employeeName, "#") == 0) break;

                    p += strlen(employeeName);
                    while (*p == ' ') p++;

                    sscanf(p, "%d", &tableId);
                    while (*p && *p != ' ') p++;
                    while (*p == ' ') p++;

                    strncpy(time, p, sizeof(time));
                    size_t len = strlen(time);
                    if (len > 0 && (time[len-1] == '\n' || time[len-1] == '\r'))
                        time[len-1] = 0;

                    create_order(tableId, employeeName, time);
                }
                continue;
            }

            if (strcmp(query, "add_dish") == 0) {
                while(1) {
                    if (!fgets(buffer, sizeof(buffer), stdin)) break;

                    char employeeName[MAX_NAME];
                    int tableId;
                    char dishName[MAX_NOTE];
                    int quantity;
                    char note[MAX_NOTE];

                    char *p = buffer;

                    sscanf(p, "%s", employeeName);
                    if (strcmp(employeeName, "#") == 0) break;

                    p += strlen(employeeName);
                    while (*p == ' ') p++;

                    sscanf(p, "%d", &tableId);
                    while (*p && *p != ' ') p++;
                    while (*p == ' ') p++;

                    p = extract_quoted_string(p, dishName);
                    while (*p == ' ') p++;

                    sscanf(p, "%d", &quantity);
                    while (*p && *p != ' ') p++;
                    while (*p == ' ') p++;

                    p = extract_quoted_string(p, note);

                    add_dish(tableId, dishName, quantity, note, employeeName);
                }
                continue;
            }

            if (strcmp(query, "update_dish") == 0) {
                while(1) {
                    if (!fgets(buffer, sizeof(buffer), stdin)) break;

                    char employeeName[MAX_NAME];
                    int tableId;
                    char dishName[MAX_NOTE];
                    int quantity;

                    char *p = buffer;

                    sscanf(p, "%s", employeeName);
                    if (strcmp(employeeName, "#") == 0) break;

                    p += strlen(employeeName);
                    while (*p == ' ') p++;

                    sscanf(p, "%d", &tableId);
                    while (*p && *p != ' ') p++;
                    while (*p == ' ') p++;

                    p = extract_quoted_string(p, dishName);
                    while (*p == ' ') p++;

                    sscanf(p, "%d", &quantity);

                    update_dish(tableId, dishName, quantity);
                }
                continue;
            }

            if (strcmp(query, "cancel_dish") == 0) {
                while(1) {
                    if (!fgets(buffer, sizeof(buffer), stdin)) break;

                    char employeeName[MAX_NAME];
                    int tableId;
                    char dishName[MAX_NOTE];
                    char note[MAX_NOTE];

                    char *p = buffer;

                    sscanf(p, "%s", employeeName);
                    if (strcmp(employeeName, "#") == 0) break;

                    p += strlen(employeeName);
                    while (*p == ' ') p++;

                    sscanf(p, "%d", &tableId);
                    while (*p && *p != ' ') p++;
                    while (*p == ' ') p++;

                    p = extract_quoted_string(p, dishName);
                    while (*p == ' ') p++;

                    p = extract_quoted_string(p, note);

                    cancel_dish(tableId, dishName, note);
                }
                continue;
            }

            if (strcmp(query, "cancel_order") == 0) {
                while(1) {
                    if (!fgets(buffer, sizeof(buffer), stdin)) break;

                    char employeeName[MAX_NAME];
                    int tableId;

                    sscanf(buffer, "%s", employeeName);
                    if (strcmp(employeeName, "#") == 0) break;

                    sscanf(buffer, "%d", &tableId);
                    cancel_order(tableId);
                }
                continue;
            }

            if (strcmp(query, "print_order") == 0) {
                if (!fgets(buffer, sizeof(buffer), stdin)) break;

                sscanf(buffer, "%d", &tableId);
                print_order(tableId);
                continue;
            }

            if (strcmp(query, "create_bill") == 0) {
                while(1) {
                    if (!fgets(buffer, sizeof(buffer), stdin)) break;

                    char employeeName[MAX_NAME];
                    int tableId;

                    sscanf(buffer, "%s", employeeName);
                    if (strcmp(employeeName, "#") == 0) break;

                    sscanf(buffer, "%d", &tableId);
                    create_bill(tableId);
                }
                continue;
            }
        }
        else
            break;
    }
}
