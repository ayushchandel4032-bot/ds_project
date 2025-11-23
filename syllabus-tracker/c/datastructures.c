#include "datastructures.h"
#include <stdlib.h>
#include <string.h>

void ll_init(LinkedList* ll) { ll->head = NULL; ll->length = 0; }
void ll_append(LinkedList* ll, Topic v) {
	ListNode* n = (ListNode*)malloc(sizeof(ListNode));
	n->value = v; n->next = NULL;
	if (!ll->head) ll->head = n; else { ListNode* c = ll->head; while (c->next) c = c->next; c->next = n; }
	ll->length++;
}
void ll_free(LinkedList* ll) { ListNode* c = ll->head; while (c) { ListNode* t = c->next; free(c); c = t; } ll->head = NULL; ll->length = 0; }

void stack_init(Stack* s) { s->items = NULL; s->size = 0; s->cap = 0; }
void stack_push(Stack* s, const char* str) {
	if (s->size == s->cap) { s->cap = s->cap ? s->cap * 2 : 4; s->items = (char**)realloc(s->items, s->cap * sizeof(char*)); }
	s->items[s->size] = (char*)malloc(strlen(str) + 1); strcpy(s->items[s->size], str); s->size++;
}
char* stack_pop(Stack* s) { if (!s->size) return NULL; char* out = s->items[s->size - 1]; s->size--; return out; }
void stack_free(Stack* s) { for (size_t i = 0; i < s->size; ++i) free(s->items[i]); free(s->items); s->items = NULL; s->size = s->cap = 0; }

void queue_init(Queue* q) { q->items = NULL; q->head = q->tail = q->size = q->cap = 0; }
void queue_enqueue(Queue* q, int v) {
	if (q->size + 1 > q->cap) { size_t ncap = q->cap ? q->cap * 2 : 4; int* nitems = (int*)malloc(ncap * sizeof(int)); for (size_t i = 0; i < q->size; ++i) nitems[i] = q->items[(q->head + i) % (q->cap ? q->cap : 1)]; free(q->items); q->items = nitems; q->cap = ncap; q->head = 0; q->tail = q->size; }
	q->items[q->tail] = v; q->tail = (q->tail + 1) % q->cap; q->size++;
}
int queue_dequeue(Queue* q, int* ok) { if (!q->size) { if (ok) *ok = 0; return 0; } int v = q->items[q->head]; q->head = (q->head + 1) % q->cap; q->size--; if (ok) *ok = 1; return v; }
void queue_free(Queue* q) { free(q->items); q->items = NULL; q->head = q->tail = q->size = q->cap = 0; }

TreeNode* tree_create(const char* label) { TreeNode* n = (TreeNode*)malloc(sizeof(TreeNode)); strncpy(n->label, label, sizeof(n->label)); n->label[sizeof(n->label)-1] = '\0'; n->children = NULL; n->childCount = 0; n->childCap = 0; return n; }
void tree_add_child(TreeNode* parent, TreeNode* child) { if (parent->childCount == parent->childCap) { parent->childCap = parent->childCap ? parent->childCap * 2 : 2; parent->children = (TreeNode**)realloc(parent->children, parent->childCap * sizeof(TreeNode*)); } parent->children[parent->childCount++] = child; }
void tree_free(TreeNode* root) { if (!root) return; for (size_t i = 0; i < root->childCount; ++i) tree_free(root->children[i]); free(root->children); free(root); }

void graph_init(Graph* g, int n) { g->n = n > MAXV ? MAXV : n; for (int i = 0; i < MAXV; ++i) for (int j = 0; j < MAXV; ++j) g->adj[i][j] = 0; }
void graph_add_edge(Graph* g, int u, int v) { if (u >= 0 && v >= 0 && u < g->n && v < g->n) g->adj[u][v] = 1; }
int graph_indegree(Graph* g, int v) { int d = 0; if (v < 0 || v >= g->n) return 0; for (int u = 0; u < g->n; ++u) if (g->adj[u][v]) d++; return d; }

