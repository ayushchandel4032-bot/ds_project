#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "datastructures.h"

static Topic make_topic(const char* title, const char* date, const char* url) {
	Topic t; memset(&t, 0, sizeof(t));
	strncpy(t.title, title, sizeof(t.title) - 1);
	if (date) strncpy(t.plannedDate, date, sizeof(t.plannedDate) - 1);
	if (url) strncpy(t.resourceUrl, url, sizeof(t.resourceUrl) - 1);
	t.taught = 0; t.studied = 0;
	return t;
}

int main(void) {
	printf("Syllabus Tracker (C CLI demo)\n");

	LinkedList topics; ll_init(&topics);
	ll_append(&topics, make_topic("Intro to Graphs", "2025-11-01", "http://example.com/graphs.pdf"));
	ll_append(&topics, make_topic("DFS & BFS", "2025-11-03", "http://example.com/traversals.pdf"));
	ll_append(&topics, make_topic("Trees", "2025-11-05", "http://example.com/trees.pdf"));

	printf("Added %zu topics.\n", topics.length);

	// Notes stack
	Stack notes; stack_init(&notes);
	stack_push(&notes, "Start with graphs");
	stack_push(&notes, "Revise DFS before BFS");
	char* lastNote = stack_pop(&notes);
	printf("Undo last note: %s\n", lastNote ? lastNote : "(none)");
	free(lastNote);

	// Reminder queue (toy) epoch-days
	Queue rem; queue_init(&rem);
	queue_enqueue(&rem, 20251101);
	queue_enqueue(&rem, 20251103);
	int ok = 0; int next = queue_dequeue(&rem, &ok);
	if (ok) printf("Next reminder day: %d\n", next);

	// Tree: Subject -> Topics
	TreeNode* subj = tree_create("Data Structures");
	TreeNode* t1 = tree_create("Intro to Graphs");
	TreeNode* t2 = tree_create("DFS & BFS");
	tree_add_child(subj, t1); tree_add_child(subj, t2);
	printf("Tree children count for subject: %zu\n", subj->childCount);

	// Graph dependencies: t1 -> t2
	Graph g; graph_init(&g, 10); graph_add_edge(&g, 1, 2);
	printf("Indegree of 2: %d\n", graph_indegree(&g, 2));

	// Print topics
	printf("Topics list:\n");
	for (ListNode* n = topics.head; n; n = n->next) {
		printf("- %s (planned: %s) res: %s\n", n->value.title, n->value.plannedDate, n->value.resourceUrl);
	}

	// Cleanup
	ll_free(&topics);
	stack_free(&notes);
	queue_free(&rem);
	tree_free(subj);
	return 0;
}

