/* phases.c
 * Bruno Olmedo & Deanna Bessy
 * 3/10/15
 *
 * C code of assembly code
 * 
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

void phase_1(char *input);
void phase_2(char *input);
void phase_3(char *input);
void phase_4(char *input);
void phase_5(char *input);
void phase_6(char *input);

extern void explode_bomb(); 
extern bool strings_not_equal(char *input, char *string);
extern int * read_six_numbers(char *input);


void phase_1(char *input)
{
        if(strings_not_equal(input, 
                             "Wouldn't you prefer a nice game of chess?")) {
             explode_bomb();
        }
}

void phase_2(char *input)
{
        int *numbers = read_six_numbers(input);
        int ii = 0;
        int sum = 0;
        while (ii < 3) {
                if (numbers[ii] == numbers[ii+3]) {
                        sum = numbers[ii];
                        ii++;
                } else {
                        explode_bomb();
                }
        }
        if (sum == 0) {
                explode_bomb();
        }
}

void phase_3(char *input)
{
        int index;
        int val;
        int numScanned;
        int x;
        
        numScanned = sscanf(input, "%i %i", &index, &val); 
        
        if (val <= 1) {
                explode_bomb();
        }
        
        if (index > 7) {
                explode_bomb();
        }
        
        switch(index) {
          case 0:
                   x = 681;
          case 1:
                   x = 497;
          case 2:
                   x = 62;
          case 3:
                   x = 985;
          case 4: 
                   x = 664;
          case 5:
                   x = 631;
          case 6:
                   x = 352;
          case 7:
                   x = 923;
        }
                  
        if (val != x) {
                explode_bomb();
        }

        (void)numScanned;
}

/* Calculates the factorial of an int */
int func4(int val) 
{
        int i = 0;
        if (i < 1) {
                return val;
        } else {
                return func4(val * func4(val-1));
        }
}

void phase_4(char *input)
{
        int val = 0;
        int numScanned = sscanf(input, "%i", &val);
        int result = 0;
        if (val <= 1) {
                explode_bomb();
        }

        result = func4(val);

        if (result != 720) {
                explode_bomb();
        }

        (void)numScanned;
}

void phase_5(char *input)
{
        int array[16] = {10, 2, 14, 7, 8, 12, 15, 11, 0, 4, 1, 13, 3, 9, 6, 5}; 
        int count = 0; 
        int sum;
        int result = 0;
        int numScanned;
        int p = 0;
        numScanned = sscanf(input, "%i %i", &p, &result);
        p = (p + 16) % 16; /* -3 and 13 will both start in same place */        
        sum = 0;

        while(count < 10) {
                p = array[p]; 
                sum += p; 
                count++; 
        }

        if(sum != result) {
                explode_bomb(); 
        } 

        (void)numScanned; 
}

typedef struct listNode {
        int index;
        struct listNode *next;
        int value;
} listNode;

/* Sorts a linked list from largest to smallest */
listNode *fun6(listNode *start) {
        listNode *head = start;
        listNode *p = head->next;
        listNode *q = NULL;
        listNode *r;
        
        while (p != NULL) {
                if ((head == NULL) || (p->value > head->value)) {
                        p->next = head;
                        head = p;
                } 
                else {
                        q = p;
                        r = head;
                        while (r != NULL) {
                                if ((q->next == NULL) || 
                                    (r->value > q->value)) {
                                        r->next = q->next;
                                        q->next = r;
                                        break;
                                }
                                r = r->next;
                                q = q->next;
                        }
                }
        }
        
        return head;
        
}

void phase_6(char *input)
{
        int i = 0;
        int value;
        value = atoi((const char *)input);
        listNode node9 = {9, NULL, 261};
        listNode node8 = {8, &node9, 194};
        listNode node7 = {7, &node8, 516};
        listNode node6 = {6, &node7, 374};
        listNode node5 = {5, &node6, 563};
        listNode node4 = {4, &node5, 763};
        listNode node3 = {3, &node4, 293};
        listNode node2 = {2, &node3, 692};
        listNode node1 = {1, &node2, 334};

        listNode *start = &node1;
        start = fun6(start); 
        listNode *p = start; 

        while (i < 5) {
                p = p->next;
                i++;
        }

        if (p->value != value) {
                explode_bomb();
        }
        
        (void)i;
        (void)value;
        
}