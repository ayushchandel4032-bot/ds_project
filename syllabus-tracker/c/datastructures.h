#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H
#include <stddef.h>
#include <stdbool.h>

typedef struct Topic {
	char title[128];
	char plannedDate[16];
	char resourceUrl[256];
	bool taught;
	bool studied;
} Topic;

// Linked List of topics
typedef struct ListNode {
	Topic value;
	struct ListNode* next;
} ListNode;

typedef struct LinkedList {
	ListNode* head;
	size_t length;
} LinkedList;

void ll_init(LinkedList*);
void ll_append(LinkedList*, Topic v);
void ll_free(LinkedList*);

// Stack of strings (notes history)
typedef struct Stack {
	char** items;
	size_t size;
	size_t cap;
} Stack;

void stack_init(Stack*);
void stack_push(Stack*, const char* s);
char* stack_pop(Stack*);
void stack_free(Stack*);

// Queue of reminder ints (epoch days)
typedef struct Queue {
	int* items;
	size_t head;
	size_t tail;
	size_t size;
	size_t cap;
} Queue;

void queue_init(Queue*);
void queue_enqueue(Queue*, int v);
int queue_dequeue(Queue*, int* ok);
void queue_free(Queue*);

// Tree (simple subject->topics)
typedef struct TreeNode {
	char label[128];
	struct TreeNode** children;
	size_t childCount;
	size_t childCap;
} TreeNode;

TreeNode* tree_create(const char* label);
void tree_add_child(TreeNode* parent, TreeNode* child);
void tree_free(TreeNode* root);

// Graph adjacency matrix for up to N topics
#define MAXV 128

typedef struct Graph {
	int n;
	unsigned char adj[MAXV][MAXV];
} Graph;

void graph_init(Graph*, int n);
void graph_add_edge(Graph*, int u, int v);
int graph_indegree(Graph*, int v);

#endif

