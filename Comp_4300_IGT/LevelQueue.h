#pragma once
#include <queue>
#include <string>

class LevelQueue {
    std::queue<std::string> q;
public:
    void enqueue(const std::string& path) { q.push(path); }
    void dequeue()                         { if (!q.empty()) q.pop(); }
    std::string front() const              { return q.empty() ? "" : q.front(); }
    bool isEmpty() const                   { return q.empty(); }
};
