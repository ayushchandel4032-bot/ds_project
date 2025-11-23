# Data Structures Project

A collection of data structures and algorithms implementations in C, along with a web-based syllabus tracker application.

## Projects

### 1. Classroom Management System (`classroom.c`)
A DSA-based classroom communication and syllabus management system implemented in C.

**Features:**
- Hash table for user management
- Graph-based chat system
- Message queues
- BST (Binary Search Tree) for syllabus
- Announcement stack
- Min-heap for assignments
- File save/load functionality

**Compile:**
```bash
gcc -std=c99 -O2 classroom.c -o classroom
```

**Run:**
```bash
./classroom
```

### 2. Syllabus Tracker (`syllabus-tracker/`)
A simple web and CLI project to help students and teachers track syllabus progress in real time.

**Features:**
- Web app (HTML/CSS/JS) with beginner-friendly data structures: Linked List, Stack, Queue, Tree, Graph
- C console program showcasing the same data structures and a syllabus demo
- No backend required; web app stores data in LocalStorage

**Quick Start:**

**Web:**
1. Open `syllabus-tracker/web/index.html` in a browser
2. Use Teacher tab to add subjects, topics, resources, and mark taught
3. Use Student tab to mark studied, view progress, notes, and reminders

**C CLI:**
```bash
cd syllabus-tracker/c
make
./syllabus
```

## Project Structure

```
.
├── classroom.c              # Classroom management system
├── syllabus-tracker/
│   ├── c/                  # C implementations and CLI
│   │   ├── datastructures.c
│   │   ├── datastructures.h
│   │   ├── syllabus.c
│   │   └── Makefile
│   ├── web/                # Frontend web application
│   │   ├── index.html
│   │   ├── styles.css
│   │   └── app.js
│   └── README.md
└── README.md
```

## Notes

- Web notifications may require permission; if denied, the app uses alerts
- Data is stored locally in LocalStorage and is device-specific
- Build artifacts (`.o`, `.dSYM`, binaries) are excluded from version control

