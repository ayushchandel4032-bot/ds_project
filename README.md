# Data Structures & Algorithms Project

A comprehensive educational project demonstrating practical implementations of fundamental data structures and algorithms through two real-world applications: a classroom management system and a syllabus tracking platform. This project showcases both low-level C implementations and modern web technologies, making it an excellent resource for learning DSA concepts through hands-on examples.

## 🎯 Project Overview

This repository contains two complete applications that demonstrate how core data structures and algorithms can be applied to solve real-world problems in educational technology:

1. **Classroom Management System** - A full-featured console application in C that uses multiple DSA concepts (hash tables, graphs, trees, heaps, queues, stacks) to manage classroom communication, user interactions, syllabus organization, and assignment prioritization.

2. **Syllabus Tracker** - A dual-platform application (web and CLI) that helps students and teachers track learning progress, featuring beginner-friendly implementations of data structures in both JavaScript and C.

## Projects

### 1. Classroom Management System (`classroom.c`)

A comprehensive console-based classroom management system that demonstrates advanced DSA implementations in C. This application serves as a practical example of how multiple data structures work together in a real-world scenario.

**Key Features:**
- **Hash Table** - Efficient user authentication and management with collision handling
- **Graph Data Structure** - Social network-style chat system connecting students and teachers
- **Message Queues** - FIFO message handling for classroom communications
- **Binary Search Tree (BST)** - Organized syllabus storage with fast search and traversal
- **Stack** - LIFO announcement system for important notices
- **Min-Heap** - Priority queue for assignment management (earliest deadlines first)
- **File I/O** - Persistent data storage and retrieval

**Learning Value:**
This project demonstrates how different data structures are chosen based on their strengths: hash tables for O(1) lookups, graphs for relationships, BSTs for ordered data, heaps for priority queues, and stacks for LIFO operations.

**Compile:**
```bash
gcc -std=c99 -O2 classroom.c -o classroom
```

**Run:**
```bash
./classroom
```

### 2. Syllabus Tracker (`syllabus-tracker/`)

A dual-platform educational tool that helps students and teachers collaboratively track syllabus progress. This project provides the same functionality in both web and CLI formats, making it accessible to different user preferences while demonstrating data structure implementations in multiple languages.

**Key Features:**
- **Web Application** - Modern, responsive UI built with vanilla HTML, CSS, and JavaScript
  - Interactive data structure visualizations (Linked List, Stack, Queue, Tree, Graph)
  - Real-time progress tracking with LocalStorage persistence
  - Separate interfaces for teachers (content management) and students (progress tracking)
  - No backend required - fully client-side application
  
- **C Console Application** - Command-line version with the same core functionality
  - Demonstrates the same data structures in C
  - Cross-platform compatibility
  - Lightweight and fast execution

**Learning Value:**
Perfect for beginners to understand how the same data structures can be implemented across different languages and platforms, while providing a practical tool for educational use.

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

## 🚀 Getting Started

### Prerequisites
- **For C programs:** GCC compiler (C99 standard)
- **For web app:** Modern web browser (Chrome, Firefox, Safari, Edge)
- **For building:** Make utility (for syllabus-tracker C program)

### Quick Start

**Classroom Management System:**
```bash
gcc -std=c99 -O2 classroom.c -o classroom
./classroom
```

**Syllabus Tracker Web App:**
```bash
# Simply open in your browser
open syllabus-tracker/web/index.html
# Or navigate to the file in your file manager
```

**Syllabus Tracker CLI:**
```bash
cd syllabus-tracker/c
make
./syllabus
```

## 📚 Data Structures Implemented

This project demonstrates implementations of:

- **Hash Tables** - User management and authentication
- **Graphs** - Social connections and chat systems
- **Binary Search Trees** - Ordered data storage and retrieval
- **Heaps (Min-Heap)** - Priority queues for assignments
- **Stacks** - LIFO operations for announcements
- **Queues** - FIFO message handling
- **Linked Lists** - Dynamic data organization

## 🎓 Educational Purpose

This project is designed for:
- **Students** learning data structures and algorithms
- **Educators** teaching DSA concepts with practical examples
- **Developers** looking for reference implementations
- **Anyone** interested in understanding how DSA applies to real-world problems

## 📝 Notes

- Web notifications may require browser permission; if denied, the app uses alerts
- Data is stored locally in LocalStorage and is device-specific
- Build artifacts (`.o`, `.dSYM`, binaries) are excluded from version control
- All code is well-commented for educational purposes

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the issues page or submit a pull request.

## 📄 License

This project is open source and available for educational purposes.

