#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <emscripten.h>

#define MAX 200

typedef struct{
    char date[20];
    char category[50];
    float amount;
    char note[100];
} Expense;

Expense expenses[MAX];
int count = 0;

float monthlyBudget = 0.0f;

int step = 0;
int menuChoice = 0;

char tempDate[20];
char tempCategory[50];
float tempAmount;
char tempNote[100];
char tempQuery[50];

void saveToFile(){
    FILE *fp = fopen("expenses.dat", "wb");
    if(fp == NULL){
        printf("Error: could not save records.\n");
        return;
    }
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(expenses, sizeof(Expense), count, fp);
    fwrite(&monthlyBudget, sizeof(float), 1, fp);
    fclose(fp);
}

void loadFromFile(){
    FILE *fp = fopen("expenses.dat", "rb");
    if(fp == NULL){
        count = 0;
        monthlyBudget = 0.0f;
        return;
    }
    fread(&count, sizeof(int), 1, fp);
    fread(expenses, sizeof(Expense), count, fp);
    fread(&monthlyBudget, sizeof(float), 1, fp);
    fclose(fp);
}

void toLowerCase(char dest[], char src[]){
    int i = 0;
    while(src[i] != '\0'){
        if(src[i] >= 'A' && src[i] <= 'Z')
            dest[i] = src[i] + 32;
        else
            dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int isSameMonth(char date[], char month[]){
    char extracted[8];
    strncpy(extracted, date, 7);
    extracted[7] = '\0';
    return strcmp(extracted, month) == 0;
}

void addExpenseRecord(){
    if(count >= MAX){
        printf("Expense storage is full.\n");
        return;
    }

    strcpy(expenses[count].date, tempDate);
    strcpy(expenses[count].category, tempCategory);
    expenses[count].amount = tempAmount;
    strcpy(expenses[count].note, tempNote);
    count++;

    saveToFile();

    printf("Expense added successfully.\n");
    printf("Date     : %s\n", tempDate);
    printf("Category : %s\n", tempCategory);
    printf("Amount   : %.2f\n", tempAmount);
    printf("Note     : %s\n", tempNote);
}

void setBudget(float amount){
    if(amount <= 0){
        printf("Invalid budget amount.\n");
        return;
    }

    monthlyBudget = amount;
    saveToFile();

    printf("Monthly budget set successfully.\n");
    printf("Budget Amount : %.2f\n", monthlyBudget);
}

void showTotalForMonth(char month[]){
    float total = 0.0f;

    for(int i = 0; i < count; i++){
        if(isSameMonth(expenses[i].date, month)){
            total += expenses[i].amount;
        }
    }

    printf("Monthly expense summary:\n");
    printf("Month        : %s\n", month);
    printf("Total Amount : %.2f\n", total);

    if(monthlyBudget > 0){
        printf("Budget Limit : %.2f\n", monthlyBudget);
        if(total > monthlyBudget){
            printf("Warning: Monthly expense exceeded the budget.\n");
        }
        else{
            printf("Status: Monthly expense is within the budget.\n");
        }
    }
    else{
        printf("Budget not set yet.\n");
    }
}

void showCategoryTotal(char category[]){
    float total = 0.0f;
    int found = 0;
    char lowerStored[50], lowerQuery[50];

    toLowerCase(lowerQuery, category);

    for(int i = 0; i < count; i++){
        toLowerCase(lowerStored, expenses[i].category);

        if(strcmp(lowerStored, lowerQuery) == 0){
            total += expenses[i].amount;
            found = 1;
        }
    }

    if(!found){
        printf("No expenses found for the given category.\n");
        return;
    }

    printf("Category-wise expense summary:\n");
    printf("Category     : %s\n", category);
    printf("Total Amount : %.2f\n", total);
}

void searchByDate(char date[]){
    int found = 0;

    for(int i = 0; i < count; i++){
        if(strcmp(expenses[i].date, date) == 0){
            if(!found){
                printf("Expenses found for date %s:\n", date);
            }

            found = 1;

            printf("------------------------------\n");
            printf("Category : %s\n", expenses[i].category);
            printf("Amount   : %.2f\n", expenses[i].amount);
            printf("Note     : %s\n", expenses[i].note);
        }
    }

    if(!found){
        printf("No expenses found for the given date.\n");
    }
}

void searchByCategory(char category[]){
    int found = 0;
    char lowerStored[50], lowerQuery[50];

    toLowerCase(lowerQuery, category);

    for(int i = 0; i < count; i++){
        toLowerCase(lowerStored, expenses[i].category);

        if(strcmp(lowerStored, lowerQuery) == 0){
            if(!found){
                printf("Expenses found for category %s:\n", category);
            }

            found = 1;

            printf("------------------------------\n");
            printf("Date   : %s\n", expenses[i].date);
            printf("Amount : %.2f\n", expenses[i].amount);
            printf("Note   : %s\n", expenses[i].note);
        }
    }

    if(!found){
        printf("No expenses found for the given category.\n");
    }
}

void showAllExpenses(){
    if(count == 0){
        printf("No expense records found.\n");
        return;
    }

    printf("All expense records:\n");

    for(int i = 0; i < count; i++){
        printf("------------------------------\n");
        printf("Record %d\n", i + 1);
        printf("Date     : %s\n", expenses[i].date);
        printf("Category : %s\n", expenses[i].category);
        printf("Amount   : %.2f\n", expenses[i].amount);
        printf("Note     : %s\n", expenses[i].note);
    }
}

EMSCRIPTEN_KEEPALIVE
void processInput(char *input){

    if(step == 0){
        menuChoice = atoi(input);

        if(menuChoice == 1){
            step = 1;
            return;
        }
        else if(menuChoice == 2){
            step = 10;
            return;
        }
        else if(menuChoice == 3){
            step = 20;
            return;
        }
        else if(menuChoice == 4){
            step = 30;
            return;
        }
        else if(menuChoice == 5){
            step = 40;
            return;
        }
        else if(menuChoice == 6){
            step = 50;
            return;
        }
        else if(menuChoice == 7){
            showAllExpenses();
            return;
        }
        else{
            printf("Invalid choice.\n");
            return;
        }
    }

    else if(step == 1){
        if(strlen(input) == 0){
            printf("Date cannot be empty.\n");
            step = 0;
            return;
        }

        strcpy(tempDate, input);
        step = 2;
        return;
    }

    else if(step == 2){
        if(strlen(input) == 0){
            printf("Category cannot be empty.\n");
            step = 0;
            return;
        }

        strcpy(tempCategory, input);
        step = 3;
        return;
    }

    else if(step == 3){
        tempAmount = atof(input);

        if(tempAmount <= 0){
            printf("Invalid amount.\n");
            step = 0;
            return;
        }

        step = 4;
        return;
    }

    else if(step == 4){
        strcpy(tempNote, input);
        addExpenseRecord();
        step = 0;
        return;
    }

    else if(step == 10){
        float budget = atof(input);
        setBudget(budget);
        step = 0;
        return;
    }

    else if(step == 20){
        if(strlen(input) == 0){
            printf("Month cannot be empty.\n");
            step = 0;
            return;
        }

        showTotalForMonth(input);
        step = 0;
        return;
    }

    else if(step == 30){
        if(strlen(input) == 0){
            printf("Category cannot be empty.\n");
            step = 0;
            return;
        }

        showCategoryTotal(input);
        step = 0;
        return;
    }

    else if(step == 40){
        int searchChoice = atoi(input);

        if(searchChoice == 1){
            step = 41;
            return;
        }
        else if(searchChoice == 2){
            step = 42;
            return;
        }
        else{
            printf("Invalid search choice.\n");
            step = 0;
            return;
        }
    }

    else if(step == 41){
        if(strlen(input) == 0){
            printf("Date cannot be empty.\n");
            step = 0;
            return;
        }

        searchByDate(input);
        step = 0;
        return;
    }

    else if(step == 42){
        if(strlen(input) == 0){
            printf("Category cannot be empty.\n");
            step = 0;
            return;
        }

        searchByCategory(input);
        step = 0;
        return;
    }

    else if(step == 50){
        printf("Expense & Budget Tracker session closed.\n");
        step = 0;
        return;
    }
}

int main(){
    loadFromFile();
    printf("Expense & Budget Tracker is ready.\n");
    return 0;
}
