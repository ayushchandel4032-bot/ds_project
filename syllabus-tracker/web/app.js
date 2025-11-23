// Basic beginner-friendly data structures in JS

class LinkedListNode { constructor(value) { this.value = value; this.next = null; } }
class LinkedList {
	constructor() { this.head = null; this.length = 0; }
	append(value) { const node = new LinkedListNode(value); if (!this.head) { this.head = node; } else { let cur = this.head; while (cur.next) cur = cur.next; cur.next = node; } this.length++; return node; }
	toArray() { const out = []; let cur = this.head; while (cur) { out.push(cur.value); cur = cur.next; } return out; }
}

class Stack { constructor() { this.items = []; } push(x) { this.items.push(x); } pop() { return this.items.pop(); } peek() { return this.items[this.items.length - 1]; } size() { return this.items.length; } }
class Queue { constructor() { this.items = []; } enqueue(x) { this.items.push(x); } dequeue() { return this.items.shift(); } size() { return this.items.length; } }

class TreeNode { constructor(value) { this.value = value; this.children = []; } addChild(node) { this.children.push(node); } }

class Graph { constructor() { this.adj = new Map(); } addVertex(v) { if (!this.adj.has(v)) this.adj.set(v, new Set()); } addEdge(u, v) { this.addVertex(u); this.addVertex(v); this.adj.get(u).add(v); }
	inDegree(v) { let d = 0; for (const [u, nbrs] of this.adj) if (nbrs.has(v)) d++; return d; }
}

// Domain models
function createSubject(name) { return { id: crypto.randomUUID(), name, topics: new LinkedList(), createdAt: Date.now() }; }
function createTopic(title, dateStr, resourceUrl) { return { id: crypto.randomUUID(), title, plannedDate: dateStr || null, resourceUrl: resourceUrl || null, taught: false, studied: false, createdAt: Date.now() }; }

// Storage
const STORAGE_KEY = "syllabus-tracker.v1";
function loadState() {
	const raw = localStorage.getItem(STORAGE_KEY);
	if (!raw) return { subjects: [], notes: "", notesUndo: [] };
	try {
		const data = JSON.parse(raw);
		// Rehydrate linked lists
		data.subjects = (data.subjects || []).map(s => {
			const ls = new LinkedList();
			(s.topicsArray || []).forEach(t => ls.append(t));
			return { id: s.id, name: s.name, topics: ls, createdAt: s.createdAt };
		});
		return data;
	} catch { return { subjects: [], notes: "", notesUndo: [] }; }
}
function saveState(state) {
	const toSave = {
		subjects: state.subjects.map(s => ({ id: s.id, name: s.name, topicsArray: s.topics.toArray(), createdAt: s.createdAt })),
		notes: state.notes,
		notesUndo: state.notesUndo || []
	};
	localStorage.setItem(STORAGE_KEY, JSON.stringify(toSave));
}

const appState = loadState();

// Graph for dependencies (topicId -> subsequent topicIds)
const topicGraph = new Graph();

// UI helpers
function el(tag, className, children) { const e = document.createElement(tag); if (className) e.className = className; if (children) children.forEach(c => e.appendChild(c)); return e; }
function txt(s) { return document.createTextNode(s); }
function formatDate(d) { if (!d) return ""; try { return new Date(d).toLocaleDateString(); } catch { return d; } }

// Progress
function computeProgress(subject) {
	const topics = subject.topics.toArray();
	const total = topics.length;
	const taught = topics.filter(t => t.taught).length;
	const studied = topics.filter(t => t.studied).length;
	return { total, taught, studied, pctTaught: total ? Math.round((taught / total) * 100) : 0, pctStudied: total ? Math.round((studied / total) * 100) : 0 };
}

// Reminders queue (pending plannedDate in the next 48h)
const reminderQueue = new Queue();
function refreshReminders() {
	while (reminderQueue.size()) reminderQueue.dequeue();
	const now = Date.now();
	const horizon = now + 1000 * 60 * 60 * 48;
	for (const s of appState.subjects) {
		for (const t of s.topics.toArray()) {
			if (t.plannedDate && !t.taught) {
				const ts = new Date(t.plannedDate).getTime();
				if (ts >= now && ts <= horizon) reminderQueue.enqueue({ subjectId: s.id, topic: t, when: ts });
			}
		}
	}
}

function requestNotifyPermission() { if ("Notification" in window && Notification.permission === "default") Notification.requestPermission(); }
function pushNotice(text) {
	if ("Notification" in window && Notification.permission === "granted") new Notification(text);
	else alert(text);
}

// Renderers
function renderSubjects(targetEl, subjects, onClick) {
	targetEl.innerHTML = "";
	subjects.forEach(s => {
		const pr = computeProgress(s);
		const bar = el("div", "progress", [el("div", "", [])]);
		bar.firstChild.style.width = pr.pctTaught + "%";
		const item = el("div", "item", [
			el("div", "", [txt(s.name + " "), el("span", "meta", [txt(`(Taught ${pr.taught}/${pr.total}, Studied ${pr.studied}/${pr.total})`)])]),
			el("span", "badge", [txt(pr.pctTaught + "% taught")])
		]);
		item.addEventListener("click", () => onClick && onClick(s));
		targetEl.appendChild(item);
		targetEl.appendChild(bar);
	});
}

function renderTopics(targetEl, subject, mode) {
	targetEl.innerHTML = "";
	if (!subject) return;
	subject.topics.toArray().forEach(t => {
		const left = el("div", "", [
			el("div", "", [txt(t.title)]),
			el("div", "meta", [txt([t.plannedDate ? `Planned: ${formatDate(t.plannedDate)}` : null, t.resourceUrl ? `Res: ${t.resourceUrl}` : null].filter(Boolean).join(" · "))])
		]);
		const right = el("div", "", []);
		if (mode === "student") {
			const studied = el("button", "", [txt(t.studied ? "Studied✓" : "Mark Studied")]);
			studied.addEventListener("click", () => { t.studied = !t.studied; saveState(appState); updateUI(); });
			right.appendChild(studied);
		} else {
			const taught = el("button", "", [txt(t.taught ? "Taught✓" : "Mark Taught")]);
			taught.addEventListener("click", () => { t.taught = !t.taught; saveState(appState); updateUI(); });
			right.appendChild(taught);
		}
		const it = el("div", "item", [left, right]);
		targetEl.appendChild(it);
	});
}

function renderProgress(subject) {
	const c = document.getElementById("progress-container");
	c.innerHTML = "";
	if (!subject) return;
	const pr = computeProgress(subject);
	const tBar = el("div", "progress", [el("div")]); tBar.firstChild.style.width = pr.pctTaught + "%";
	const sBar = el("div", "progress", [el("div")]); sBar.firstChild.style.width = pr.pctStudied + "%";
	c.appendChild(el("div", "", [txt(`Taught: ${pr.pctTaught}%`)])); c.appendChild(tBar);
	c.appendChild(el("div", "", [txt(`Studied: ${pr.pctStudied}%`)])); c.appendChild(sBar);
}

function renderTeacherAnalytics() {
	const a = document.getElementById("teacher-analytics");
	const stats = appState.subjects.map(s => ({ name: s.name, ...computeProgress(s) }));
	a.innerHTML = stats.map(x => `${x.name}: taught ${x.taught}/${x.total}, studied ${x.studied}/${x.total}`).join("\n");
}

function renderReminders() {
	const r = document.getElementById("reminders");
	r.innerHTML = "";
	const items = [];
	for (const item of reminderQueue.items) items.push(item);
	items.sort((a, b) => a.when - b.when);
	items.forEach(it => {
		const when = new Date(it.when).toLocaleString();
		r.appendChild(el("div", "reminder", [txt(`${it.topic.title} due ${when}`)]));
	});
}

// Notes with undo using a stack
const notesInput = () => document.getElementById("notes-input");
function loadNotesUI() { notesInput().value = appState.notes || ""; document.getElementById("notes-history").textContent = `Saved versions: ${(appState.notesUndo || []).length}`; }
function saveNote() { const stack = new Stack(); (appState.notesUndo || []).forEach(v => stack.push(v)); stack.push(appState.notes || ""); appState.notesUndo = stack.items; appState.notes = notesInput().value; saveState(appState); loadNotesUI(); }
function undoNote() { if (!appState.notesUndo || appState.notesUndo.length === 0) return; const prev = appState.notesUndo.pop(); appState.notes = prev; saveState(appState); loadNotesUI(); }

// UI state
let selectedStudentSubject = null;
let selectedTeacherSubject = null;

// Update all views
function updateUI() {
	renderSubjects(document.getElementById("student-subject-list"), appState.subjects, s => { selectedStudentSubject = s; renderTopics(document.getElementById("student-topic-list"), s, "student"); renderProgress(s); });
	renderSubjects(document.getElementById("teacher-subject-list"), appState.subjects, s => { selectedTeacherSubject = s; renderTopics(document.getElementById("teacher-topic-list"), s, "teacher"); });
	// subject select for adding topics
	const select = document.getElementById("topic-subject");
	select.innerHTML = appState.subjects.map(s => `<option value="${s.id}">${s.name}</option>`).join("");
	if (selectedStudentSubject) { renderTopics(document.getElementById("student-topic-list"), selectedStudentSubject, "student"); renderProgress(selectedStudentSubject); }
	if (selectedTeacherSubject) { renderTopics(document.getElementById("teacher-topic-list"), selectedTeacherSubject, "teacher"); }
	refreshReminders(); renderReminders(); renderTeacherAnalytics();
}

// Event bindings
function bindEvents() {
	document.getElementById("tab-student").addEventListener("click", () => switchView("student"));
	document.getElementById("tab-teacher").addEventListener("click", () => switchView("teacher"));

	document.getElementById("add-subject-form").addEventListener("submit", (e) => {
		e.preventDefault();
		const name = document.getElementById("subject-name").value.trim(); if (!name) return;
		const subj = createSubject(name);
		appState.subjects.push(subj);
		saveState(appState);
		document.getElementById("subject-name").value = "";
		updateUI();
	});

	document.getElementById("add-topic-form").addEventListener("submit", (e) => {
		e.preventDefault();
		const sid = document.getElementById("topic-subject").value;
		const title = document.getElementById("topic-title").value.trim();
		const date = document.getElementById("topic-date").value;
		const res = document.getElementById("topic-resource").value.trim();
		if (!sid || !title) return;
		const s = appState.subjects.find(x => x.id === sid);
		if (!s) return;
		const topic = createTopic(title, date || null, res || null);
		s.topics.append(topic);
		// Add dependency example: if last topic exists, make it a prerequisite
		const arr = s.topics.toArray();
		if (arr.length >= 2) { const prev = arr[arr.length - 2]; topicGraph.addEdge(prev.id, topic.id); }
		saveState(appState);
		document.getElementById("topic-title").value = "";
		document.getElementById("topic-date").value = "";
		document.getElementById("topic-resource").value = "";
		updateUI();
	});

	document.getElementById("push-notice").addEventListener("click", () => {
		pushNotice("New update from Teacher. Check upcoming topics and materials.");
	});
	document.getElementById("mark-taught-today").addEventListener("click", () => {
		const today = new Date().toISOString().slice(0, 10);
		for (const s of appState.subjects) {
			for (const t of s.topics.toArray()) { if (t.plannedDate === today) t.taught = true; }
		}
		saveState(appState);
		updateUI();
	});

	document.getElementById("save-note").addEventListener("click", saveNote);
	document.getElementById("undo-note").addEventListener("click", undoNote);
}

function switchView(which) {
	const s = document.getElementById("student-view");
	const t = document.getElementById("teacher-view");
	const btS = document.getElementById("tab-student");
	const btT = document.getElementById("tab-teacher");
	if (which === "student") { s.classList.add("active"); t.classList.remove("active"); btS.classList.add("active"); btT.classList.remove("active"); }
	else { t.classList.add("active"); s.classList.remove("active"); btT.classList.add("active"); btS.classList.remove("active"); }
}

// Reminders polling
setInterval(() => { refreshReminders(); renderReminders(); }, 15000);

// Init
requestNotifyPermission();
bindEvents();
loadNotesUI();
updateUI();

