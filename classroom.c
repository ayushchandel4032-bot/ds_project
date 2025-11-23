/* classroom_dsa.c
   DSA-Based Classroom Communication & Syllabus Management System (Console)
   Features: Hash table users, Graph chat, Message queues, BST syllabus,
             Announcement stack, Min-heap assignments, File save/load
   Compile: gcc -std=c99 -O2 classroom_dsa.c -o classroom_dsa
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME 642

#define HASH_SIZE 101
#define MAX_TEXT 512
#define INITIAL_USER_CAP 100
#define MAX_SUBJECT_NAME 64
#define ASSIGNMENT_MAX 500

/* =========================
   Utility
   ========================= */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int read_int() {
    int v;
    int rc = scanf("%d", &v);
    if (rc == 1) { clear_input_buffer(); return v; }
    if (rc == EOF) {
        // No input available (non-interactive or EOF). Signal caller to exit.
        return -2;
    }
    clear_input_buffer();
    return -1;
}

void read_str(const char *prompt, char *out, int maxlen) {
    printf("%s", prompt);
    if (!fgets(out, maxlen, stdin)) { out[0] = '\0'; return; }
    size_t n = strlen(out);
    if (n && out[n-1] == '\n') out[n-1] = '\0';
}

/* =========================
   Users - Hash table with chaining
   ========================= */

typedef enum {ROLE_STUDENT, ROLE_TEACHER, ROLE_ADMIN} Role;

typedef struct User {
    char username[MAX_NAME];
    char password[MAX_NAME];
    Role role;
    int id; // unique numeric id
    struct User *next; // for chaining
} User;

User* user_table[HASH_SIZE];
int next_user_id = 1;
int total_users = 0;

unsigned int hash_username(const char *s) {
    unsigned long h = 5381;
    while (*s) h = ((h << 5) + h) + (unsigned char)(*s++);
    return (unsigned int)(h % HASH_SIZE);
}

User* find_user_by_name(const char *username) {
    unsigned int h = hash_username(username);
    User* cur = user_table[h];
    while (cur) {
        if (strcmp(cur->username, username) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

User* find_user_by_id(int id) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        User* cur = user_table[i];
        while (cur) {
            if (cur->id == id) return cur;
            cur = cur->next;
        }
    }
    return NULL;
}

User* create_user(const char *username, const char *password, Role role) {
    if (find_user_by_name(username)) return NULL;
    User* u = (User*)malloc(sizeof(User));
    strncpy(u->username, username, MAX_NAME-1); u->username[MAX_NAME-1]='\0';
    strncpy(u->password, password, MAX_NAME-1); u->password[MAX_NAME-1]='\0';
    u->role = role;
    u->id = next_user_id++;
    unsigned int h = hash_username(username);
    u->next = user_table[h];
    user_table[h] = u;
    total_users++;
    return u;
}

/* =========================
   Chat System - Graph (Adj list) + message queue per chat
   ========================= */

typedef struct Message {
    int sender_id;
    char text[MAX_TEXT];
    time_t ts;
    struct Message* next;
} Message;

/* For each pair (u,v) we store a Chat node in adjacency list of u for v */
typedef struct ChatNode {
    int peer_id;
    Message* head; // linked list queue (new messages appended at tail)
    Message* tail;
    struct ChatNode* next;
} ChatNode;

ChatNode* chat_adj[1000]; // indexed by user id up to some capacity
int chat_capacity = 1000;

ChatNode* find_chatnode(int user_id, int peer_id) {
    if (user_id <=0 || user_id >= chat_capacity) return NULL;
    ChatNode* cur = chat_adj[user_id];
    while (cur) {
        if (cur->peer_id == peer_id) return cur;
        cur = cur->next;
    }
    return NULL;
}

void ensure_chat_capacity(int id) {
    if (id >= chat_capacity) {
        int newcap = chat_capacity * 2;
        while (newcap <= id) newcap *= 2;
        chat_adj[newcap-1] = NULL; // just to resize array trick not allowed in C easily
        // Simpler: we won't reallocate; assume capacity >= many users (set large)
    }
}

void add_chat_edge(int u, int v) {
    if (u<=0||v<=0) return;
    if (!find_chatnode(u,v)) {
        ChatNode* cn = (ChatNode*)malloc(sizeof(ChatNode));
        cn->peer_id = v; cn->head = cn->tail = NULL; cn->next = chat_adj[u];
        chat_adj[u] = cn;
    }
    if (!find_chatnode(v,u)) {
        ChatNode* cn = (ChatNode*)malloc(sizeof(ChatNode));
        cn->peer_id = u; cn->head = cn->tail = NULL; cn->next = chat_adj[v];
        chat_adj[v] = cn;
    }
}

void send_message(int from_id, int to_id, const char *text) {
    add_chat_edge(from_id, to_id);
    ChatNode* cn = find_chatnode(to_id, from_id); // store in recipient's node so they can read
    if (!cn) return;
    Message* m = (Message*)malloc(sizeof(Message));
    m->sender_id = from_id;
    strncpy(m->text, text, MAX_TEXT-1); m->text[MAX_TEXT-1] = '\0';
    m->ts = time(NULL);
    m->next = NULL;
    if (!cn->head) cn->head = cn->tail = m;
    else { cn->tail->next = m; cn->tail = m; }
}

void view_messages_between(int viewer_id, int peer_id) {
    ChatNode* cn = find_chatnode(viewer_id, peer_id);
    if (!cn || !cn->head) {
        printf("No messages between you and user id %d.\n", peer_id);
        return;
    }
    Message* cur = cn->head;
    char timestr[64];
    while (cur) {
        struct tm *tm = localtime(&cur->ts);
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm);
        User* sender = find_user_by_id(cur->sender_id);
        printf("[%s] %s: %s\n", timestr, sender ? sender->username : "Unknown", cur->text);
        cur = cur->next;
    }
}

/* =========================
   Announcements - Stack
   ========================= */

typedef struct Announcement {
    char text[MAX_TEXT];
    time_t ts;
    struct Announcement* next;
} Announcement;

Announcement* ann_stack = NULL;

void push_announcement(const char *text) {
    Announcement* a = (Announcement*)malloc(sizeof(Announcement));
    strncpy(a->text, text, MAX_TEXT-1); a->text[MAX_TEXT-1] = '\0';
    a->ts = time(NULL);
    a->next = ann_stack;
    ann_stack = a;
}

void view_announcements() {
    if (!ann_stack) { printf("No announcements.\n"); return; }
    Announcement* cur = ann_stack;
    char timestr[64];
    while (cur) {
        struct tm *tm = localtime(&cur->ts);
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm);
        printf("[%s] %s\n", timestr, cur->text);
        cur = cur->next;
    }
}

/* =========================
   Syllabus - BST per subject
   ========================= */

typedef struct Topic {
    char name[128];
    int completed; // 0/1
    struct Topic *left, *right;
} Topic;

typedef struct Subject {
    char name[MAX_SUBJECT_NAME];
    Topic* root;
    struct Subject* next;
} Subject;

Subject* subjects = NULL;

Topic* create_topic_node(const char *name) {
    Topic* t = (Topic*)malloc(sizeof(Topic));
    strncpy(t->name, name, sizeof(t->name)-1); t->name[sizeof(t->name)-1] = '\0';
    t->completed = 0; t->left = t->right = NULL;
    return t;
}

Topic* bst_insert(Topic* root, const char* name) {
    if (!root) return create_topic_node(name);
    int cmp = strcmp(name, root->name);
    if (cmp < 0) root->left = bst_insert(root->left, name);
    else if (cmp > 0) root->right = bst_insert(root->right, name);
    else { /* duplicate; ignore */ }
    return root;
}

Subject* find_subject(const char* name) {
    Subject* cur = subjects;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void create_subject(const char* name) {
    if (find_subject(name)) { printf("Subject already exists.\n"); return; }
    Subject* s = (Subject*)malloc(sizeof(Subject));
    strncpy(s->name, name, MAX_SUBJECT_NAME-1); s->name[MAX_SUBJECT_NAME-1] = '\0';
    s->root = NULL;
    s->next = subjects;
    subjects = s;
}

void add_topic_to_subject(const char* subj, const char* topic) {
    Subject* s = find_subject(subj);
    if (!s) { printf("Subject not found.\n"); return; }
    s->root = bst_insert(s->root, topic);
}

void inorder_print_topics(Topic* root) {
    if (!root) return;
    inorder_print_topics(root->left);
    printf(" - %s [%s]\n", root->name, root->completed ? "Completed":"Pending");
    inorder_print_topics(root->right);
}

void view_subject_topics(const char* subj) {
    Subject* s = find_subject(subj);
    if (!s) { printf("Subject not found.\n"); return; }
    printf("Topics for %s:\n", s->name);
    inorder_print_topics(s->root);
}

Topic* bst_search(Topic* root, const char* name) {
    if (!root) return NULL;
    int cmp = strcmp(name, root->name);
    if (cmp == 0) return root;
    if (cmp < 0) return bst_search(root->left, name);
    return bst_search(root->right, name);
}

int count_topics(Topic* root) {
    if (!root) return 0;
    return 1 + count_topics(root->left) + count_topics(root->right);
}

int count_completed(Topic* root) {
    if (!root) return 0;
    return (root->completed ? 1 : 0) + count_completed(root->left) + count_completed(root->right);
}

void mark_topic_complete(const char* subj, const char* topic) {
    Subject* s = find_subject(subj);
    if (!s) { printf("Subject not found.\n"); return; }
    Topic* t = bst_search(s->root, topic);
    if (!t) { printf("Topic not found.\n"); return; }
    t->completed = 1;
    printf("Marked '%s' as completed in %s.\n", topic, subj);
}

double subject_completion_percent(const char* subj) {
    Subject* s = find_subject(subj);
    if (!s) return 0.0;
    int total = count_topics(s->root);
    if (total == 0) return 0.0;
    int done = count_completed(s->root);
    return (100.0 * done) / total;
}

/* =========================
   Assignments - Min-Heap by dueDate (yyyymmdd integer)
   Students can submit -> update record
   ========================= */

typedef struct Submission {
    int student_id;
    time_t ts;
    char filename[128]; // simulation of uploaded file name
    struct Submission* next;
} Submission;

typedef struct Assignment {
    int id;
    char title[128];
    int dueDate; // YYYYMMDD as int for priority
    char description[MAX_TEXT];
    Submission* submissions; // linked list of submissions
} Assignment;

Assignment* assignment_heap[ASSIGNMENT_MAX];
int heap_size = 0;
int next_assignment_id = 1;

void heap_swap(int i, int j) {
    Assignment* tmp = assignment_heap[i];
    assignment_heap[i] = assignment_heap[j];
    assignment_heap[j] = tmp;
}

void heap_push(Assignment* a) {
    if (heap_size >= ASSIGNMENT_MAX) { printf("Assignment heap full.\n"); return; }
    assignment_heap[heap_size] = a;
    int i = heap_size++;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (assignment_heap[p]->dueDate <= assignment_heap[i]->dueDate) break;
        heap_swap(p, i);
        i = p;
    }
}

Assignment* heap_pop() {
    if (heap_size == 0) return NULL;
    Assignment* top = assignment_heap[0];
    assignment_heap[0] = assignment_heap[--heap_size];
    int i = 0;
    while (1) {
        int l = 2*i + 1, r = 2*i + 2, smallest = i;
        if (l < heap_size && assignment_heap[l]->dueDate < assignment_heap[smallest]->dueDate) smallest = l;
        if (r < heap_size && assignment_heap[r]->dueDate < assignment_heap[smallest]->dueDate) smallest = r;
        if (smallest == i) break;
        heap_swap(i, smallest);
        i = smallest;
    }
    return top;
}

Assignment* peek_min_assignment() {
    if (heap_size == 0) return NULL;
    return assignment_heap[0];
}

Assignment* create_assignment(const char* title, const char* desc, int dueDate) {
    Assignment* a = (Assignment*)malloc(sizeof(Assignment));
    a->id = next_assignment_id++;
    strncpy(a->title, title, sizeof(a->title)-1); a->title[sizeof(a->title)-1]='\0';
    strncpy(a->description, desc, sizeof(a->description)-1); a->description[sizeof(a->description)-1]='\0';
    a->dueDate = dueDate;
    a->submissions = NULL;
    return a;
}

void submit_assignment(int assignment_id, int student_id, const char* filename) {
    // find assignment in heap array (linear search)
    for (int i=0;i<heap_size;i++) {
        if (assignment_heap[i]->id == assignment_id) {
            Submission* s = (Submission*)malloc(sizeof(Submission));
            s->student_id = student_id;
            s->ts = time(NULL);
            strncpy(s->filename, filename, sizeof(s->filename)-1);
            s->filename[sizeof(s->filename)-1] = '\0';
            s->next = assignment_heap[i]->submissions;
            assignment_heap[i]->submissions = s;
            printf("Submission recorded for assignment %d by user %d\n", assignment_id, student_id);
            return;
        }
    }
    printf("Assignment with id %d not found.\n", assignment_id);
}

void list_assignments() {
    if (heap_size==0) { printf("No assignments.\n"); return; }
    // We'll print a shallow copy sorted by dueDate - easiest approach: copy pointers and sort by dueDate
    int n = heap_size;
    Assignment* tmp[ASSIGNMENT_MAX];
    for (int i=0;i<n;i++) tmp[i] = assignment_heap[i];
    // simple selection sort for small n
    for (int i=0;i<n;i++) {
        int min=i;
        for (int j=i+1;j<n;j++) if (tmp[j]->dueDate < tmp[min]->dueDate) min = j;
        if (min!=i) { Assignment* t=tmp[i]; tmp[i]=tmp[min]; tmp[min]=t; }
    }
    for (int i=0;i<n;i++) {
        printf("ID:%d Title:%s Due:%d Desc:%s\n", tmp[i]->id, tmp[i]->title, tmp[i]->dueDate, tmp[i]->description);
        Submission* s = tmp[i]->submissions;
        if (!s) printf("  No submissions yet.\n");
        else {
            printf("  Submissions:\n");
            while (s) {
                User* u = find_user_by_id(s->student_id);
                char timestr[64];
                struct tm *tm = localtime(&s->ts);
                strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm);
                printf("   - %s (by %s) at %s\n", s->filename, u ? u->username : "Unknown", timestr);
                s = s->next;
            }
        }
    }
}

/* =========================
   Reports & Admin
   ========================= */

void list_users() {
    printf("Users (%d):\n", total_users);
    for (int i=0;i<HASH_SIZE;i++) {
        User* cur = user_table[i];
        while (cur) {
            printf(" - ID:%d Username:%s Role:%s\n", cur->id, cur->username,
                   cur->role==ROLE_ADMIN?"Admin": cur->role==ROLE_TEACHER?"Teacher":"Student");
            cur = cur->next;
        }
    }
}

void syllabus_report() {
    Subject* cur = subjects;
    if (!cur) { printf("No subjects.\n"); return; }
    while (cur) {
        double p = subject_completion_percent(cur->name);
        printf("Subject: %s  Completion: %.2f%%\n", cur->name, p);
        cur = cur->next;
    }
}

/* =========================
   File Save/Load (simple text formats)
   ========================= */

void save_users_to_file(const char* fname) {
    FILE* f = fopen(fname, "w");
    if (!f) { printf("Unable to open file for users save.\n"); return; }
    for (int i=0;i<HASH_SIZE;i++) {
        User* cur = user_table[i];
        while (cur) {
            fprintf(f, "%d|%s|%s|%d\n", cur->id, cur->username, cur->password, (int)cur->role);
            cur = cur->next;
        }
    }
    fclose(f);
    printf("Users saved to %s\n", fname);
}

void load_users_from_file(const char* fname) {
    FILE* f = fopen(fname, "r");
    if (!f) { printf("No users file to load.\n"); return; }
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        int id, role;
        char uname[MAX_NAME], pwd[MAX_NAME];
        line[strcspn(line, "\n")] = '\0';
        if (sscanf(line, "%d|%63[^|]|%63[^|]|%d", &id, uname, pwd, &role) == 4) {
            // create but preserve id
            if (find_user_by_name(uname)) continue;
            User* u = (User*)malloc(sizeof(User));
            strncpy(u->username, uname, MAX_NAME-1); u->username[MAX_NAME-1]='\0';
            strncpy(u->password, pwd, MAX_NAME-1); u->password[MAX_NAME-1]='\0';
            u->role = (Role)role;
            u->id = id;
            unsigned int h = hash_username(uname);
            u->next = user_table[h];
            user_table[h] = u;
            total_users++;
            if (id >= next_user_id) next_user_id = id+1;
        }
    }
    fclose(f);
    printf("Users loaded from %s\n", fname);
}

/* For syllabus and assignments we will skip complex load/save to keep code manageable.
   Could be added similarly with recursive traversal and parsing.
*/

/* =========================
   Demo / Sample Data Initialization
   ========================= */

void init_sample_data() {
    // create admin, teacher, students
    create_user("admin", "adminpass", ROLE_ADMIN);
    create_user("teacher1", "teachpass", ROLE_TEACHER);
    create_user("alice", "alice123", ROLE_STUDENT);
    create_user("bob", "bob123", ROLE_STUDENT);

    // create subjects & topics
    create_subject("Math");
    add_topic_to_subject("Math", "Algebra");
    add_topic_to_subject("Math", "Calculus");
    add_topic_to_subject("Math", "Probability");

    create_subject("CS");
    add_topic_to_subject("CS", "Data Structures");
    add_topic_to_subject("CS", "Algorithms");
    add_topic_to_subject("CS", "Operating Systems");

    // announcements
    push_announcement("Welcome to the semester! Check syllabus updates.");
    push_announcement("Midterm scheduled in 2 weeks.");

    // assignments
    Assignment* a1 = create_assignment("Algebra HW1", "Solve Q1-Q10", 20251105);
    heap_push(a1);
    Assignment* a2 = create_assignment("DS Lab1", "Implement linked list", 20251030);
    heap_push(a2);

    // chat edges
    User* t = find_user_by_name("teacher1");
    User* a = find_user_by_name("alice");
    User* b = find_user_by_name("bob");
    if (t && a) add_chat_edge(t->id, a->id);
    if (t && b) add_chat_edge(t->id, b->id);
}

/* =========================
   Menus and main flow
   ========================= */

User* current_user = NULL;

void register_flow() {
    char uname[MAX_NAME], pwd[MAX_NAME], role_s[16];
    read_str("Enter username: ", uname, sizeof(uname));
    if (find_user_by_name(uname)) { printf("Username already exists.\n"); return; }
    read_str("Enter password: ", pwd, sizeof(pwd));
    read_str("Role (student/teacher/admin): ", role_s, sizeof(role_s));
    Role r = ROLE_STUDENT;
    if (strcmp(role_s, "teacher") == 0) r = ROLE_TEACHER;
    else if (strcmp(role_s, "admin") == 0) r = ROLE_ADMIN;
    create_user(uname, pwd, r);
    printf("Registered %s as %s.\n", uname, role_s);
}

void login_flow() {
    char uname[MAX_NAME], pwd[MAX_NAME];
    read_str("Username: ", uname, sizeof(uname));
    read_str("Password: ", pwd, sizeof(pwd));
    User* u = find_user_by_name(uname);
    if (!u || strcmp(u->password, pwd) != 0) { printf("Invalid credentials.\n"); return; }
    current_user = u;
    printf("Logged in as %s (id %d)\n", u->username, u->id);
}

void logout_flow() {
    if (current_user) { printf("Logged out %s.\n", current_user->username); current_user = NULL; }
    else printf("No user logged in.\n");
}

/* Chat menu */
void chat_menu() {
    if (!current_user) { printf("Please login first.\n"); return; }
    while (1) {
        printf("\n--- Chat Menu ---\n");
        printf("1. List chat peers\n2. Send message\n3. View messages from peer\n4. Back\nChoose: ");
        int c = read_int();
        if (c < 0) { printf("(No input) Returning to main menu.\n"); break; }
        if (c == 1) {
            // list adjacency
            ChatNode* cur = chat_adj[current_user->id];
            if (!cur) { printf("No peers.\n"); continue; }
            printf("Peers:\n");
            while (cur) {
                User* u = find_user_by_id(cur->peer_id);
                printf(" - ID:%d Name:%s\n", cur->peer_id, u ? u->username : "Unknown");
                cur = cur->next;
            }
        } else if (c == 2) {
            char peername[MAX_NAME], msg[MAX_TEXT];
            read_str("Enter recipient username: ", peername, sizeof(peername));
            User* p = find_user_by_name(peername);
            if (!p) { printf("User not found.\n"); continue; }
            read_str("Enter message text: ", msg, sizeof(msg));
            send_message(current_user->id, p->id, msg);
            printf("Message sent.\n");
        } else if (c == 3) {
            char peername[MAX_NAME];
            read_str("Enter peer username: ", peername, sizeof(peername));
            User* p = find_user_by_name(peername);
            if (!p) { printf("User not found.\n"); continue; }
            view_messages_between(current_user->id, p->id);
        } else break;
    }
}

/* Syllabus menu */
void syllabus_menu() {
    if (!current_user) { printf("Please login first.\n"); return; }
    while (1) {
        printf("\n--- Syllabus Menu ---\n");
        printf("1. Create subject (teacher/admin)\n2. Add topic (teacher/admin)\n3. View topics\n4. Mark topic complete (teacher)\n5. View subject completion\n6. List all subjects\n7. Back\nChoose: ");
        int c = read_int();
        if (c < 0) { printf("(No input) Returning to main menu.\n"); break; }
        if (c == 1) {
            if (current_user->role == ROLE_STUDENT) { printf("Permission denied.\n"); continue; }
            char sub[MAX_SUBJECT_NAME];
            read_str("Subject name: ", sub, sizeof(sub));
            create_subject(sub);
            printf("Subject created.\n");
        } else if (c == 2) {
            if (current_user->role == ROLE_STUDENT) { printf("Permission denied.\n"); continue; }
            char sub[MAX_SUBJECT_NAME], topic[128];
            read_str("Subject: ", sub, sizeof(sub));
            read_str("Topic name: ", topic, sizeof(topic));
            add_topic_to_subject(sub, topic);
            printf("Topic added.\n");
        } else if (c == 3) {
            char sub[MAX_SUBJECT_NAME];
            read_str("Subject: ", sub, sizeof(sub));
            view_subject_topics(sub);
        } else if (c == 4) {
            if (current_user->role != ROLE_TEACHER && current_user->role != ROLE_ADMIN) { printf("Permission denied.\n"); continue; }
            char sub[MAX_SUBJECT_NAME], topic[128];
            read_str("Subject: ", sub, sizeof(sub));
            read_str("Topic: ", topic, sizeof(topic));
            mark_topic_complete(sub, topic);
        } else if (c == 5) {
            char sub[MAX_SUBJECT_NAME];
            read_str("Subject: ", sub, sizeof(sub));
            double p = subject_completion_percent(sub);
            printf("Completion for %s: %.2f%%\n", sub, p);
        } else if (c == 6) {
            Subject* cur = subjects;
            if (!cur) { printf("No subjects.\n"); continue; }
            printf("Subjects:\n");
            while (cur) { printf(" - %s\n", cur->name); cur = cur->next; }
        } else break;
    }
}

/* Announcements menu */
void announcements_menu() {
    if (!current_user) { printf("Please login first.\n"); return; }
    while (1) {
        printf("\n--- Announcements Menu ---\n");
        printf("1. Post announcement (teacher/admin)\n2. View announcements\n3. Back\nChoose: ");
        int c = read_int();
        if (c < 0) { printf("(No input) Returning to main menu.\n"); break; }
        if (c == 1) {
            if (current_user->role == ROLE_STUDENT) { printf("Permission denied.\n"); continue; }
            char text[MAX_TEXT];
            read_str("Announcement text: ", text, sizeof(text));
            push_announcement(text);
            printf("Posted.\n");
        } else if (c == 2) {
            view_announcements();
        } else break;
    }
}

/* Assignments menu */
void assignments_menu() {
    if (!current_user) { printf("Please login first.\n"); return; }
    while (1) {
        printf("\n--- Assignments Menu ---\n");
        printf("1. Create assignment (teacher/admin)\n2. List assignments\n3. Submit assignment (student)\n4. Back\nChoose: ");
        int c = read_int();
        if (c < 0) { printf("(No input) Returning to main menu.\n"); break; }
        if (c == 1) {
            if (current_user->role == ROLE_STUDENT) { printf("Permission denied.\n"); continue; }
            char title[128], desc[MAX_TEXT];
            int due;
            read_str("Title: ", title, sizeof(title));
            read_str("Description: ", desc, sizeof(desc));
            printf("Due date (YYYYMMDD): ");
            due = read_int();
            Assignment* a = create_assignment(title, desc, due);
            heap_push(a);
            printf("Assignment created.\n");
        } else if (c == 2) {
            list_assignments();
        } else if (c == 3) {
            if (current_user->role != ROLE_STUDENT) { printf("Only students can submit.\n"); continue; }
            int aid;
            char fname[128];
            printf("Enter assignment ID to submit: ");
            aid = read_int();
            read_str("Enter filename (simulated): ", fname, sizeof(fname));
            submit_assignment(aid, current_user->id, fname);
        } else break;
    }
}

/* Admin menu */
void admin_menu() {
    if (!current_user || current_user->role != ROLE_ADMIN) { printf("Admin only.\n"); return; }
    while (1) {
        printf("\n--- Admin Menu ---\n");
        printf("1. List users\n2. Save users to file\n3. Load users from file\n4. Syllabus report\n5. Back\nChoose: ");
        int c = read_int();
        if (c == 1) list_users();
        else if (c == 2) { char fn[128]; read_str("Filename: ", fn, sizeof(fn)); save_users_to_file(fn); }
        else if (c == 3) { char fn[128]; read_str("Filename: ", fn, sizeof(fn)); load_users_from_file(fn); }
        else if (c == 4) syllabus_report();
        else break;
    }
}

void main_menu() {
    while (1) {
        printf("\n===== Cloud Classroom (DSA Simulation) =====\n");
        printf("Current user: %s\n", current_user?current_user->username:"(none)");
        printf("1. Register\n2. Login\n3. Logout\n4. Chat\n5. Syllabus\n6. Announcements\n7. Assignments\n8. Admin Panel\n9. Exit\nChoose: ");
        int c = read_int();
        if (c == -2) { printf("No input detected. Exiting.\n"); break; }
        if (c == 1) register_flow();
        else if (c == 2) login_flow();
        else if (c == 3) logout_flow();
        else if (c == 4) chat_menu();
        else if (c == 5) syllabus_menu();
        else if (c == 6) announcements_menu();
        else if (c == 7) assignments_menu();
        else if (c == 8) admin_menu();
        else if (c == 9) { printf("Exiting.\n"); break; }
        else printf("Invalid choice.\n");
    }
}

/* =========================
   Entry point
   ========================= */

int main() {
    // initialize chat adjacency to NULL for indices up to reasonable bound
    for (int i=0;i<1000;i++) chat_adj[i] = NULL;
    // sample data
    init_sample_data();
    printf("Welcome to DSA-Based Cloud Classroom Simulation\n");
    main_menu();
    return 0;
}
