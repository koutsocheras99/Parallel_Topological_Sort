#include <stdio.h> 
#include <stdlib.h> 

// struct for node which will contain a number
struct Node 
{ 
    int data; 
    struct Node *next; 
}; 
  

// function for inserting a node into the front of the linked list
void push(struct Node **head, int data)
{
    if (!(*head))
    {
        (*head) = (struct Node *)malloc(1 * sizeof(struct Node));
        (*head)->next = NULL;
        (*head)->data = data;
    }
    else
    {
        struct Node *current = (*head);
        while (current->next != NULL)
        {
            current = current->next;
        }

        current->next = (struct Node *)malloc(sizeof(struct Node));
        current->next->data = data;
        current->next->next = NULL;
    }
}

  
// function for removing a node from the start of the linked list 
int dequeue(struct Node **head)
{
    if (!(*head))
    {
        return -1;
    }
    else
    {
        struct Node *temp;
        int toPop = (*head)->data;

        temp = (*head);
        (*head) = (*head)->next;
        temp->next = NULL;

        free(temp);
        return toPop;
    }
} 
  

// function for printing the linked list starting from the given node
void printList(struct Node *node) 
{ 
    while (node != NULL) 
    { 
        printf(" %d ", node->data); 
        node = node->next; 
    } 
    printf("\n");
} 
