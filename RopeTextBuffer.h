
#ifndef ROPE_TEXTBUFFER_H
#define ROPE_TEXTBUFFER_H

#include "main.h"

/*
    Student can define other list data structures here
*/

/**
 * Rope (AVL-based, fixed leaf chunk size = 8)
 */
class Rope
{
public:
    static const int CHUNK_SIZE = 8;

private:
    class Node
    {
    public:
        enum BalanceFactor
        {
            LH = 1,
            EH = 0,
            RH = -1
        };

        friend class Rope;

    private:
        Node *left;
        Node *right;
        string data;
        int weight;
        int height;
        BalanceFactor balance;

        Node();
        explicit Node(const string &s);
        bool isLeaf() const;

        // friend class TestHelper;
    };

    Node *root;
    // int totalLength; // extra
    int height(Node *node) const;
    int getTotalLength(Node *node) const;
    void update(Node *node);
    Node *rotateLeft(Node *x);
    Node *rotateRight(Node *y);
    Node *rebalance(Node *node);
    void split(Node *node, int index, Node *&outLeft, Node *&outRight);
    Node *concatNodes(Node *left, Node *right);
    char charAt(Node *node, int index) const;
    void toStringHelper(Node *node, std::ostringstream &oss) const; // extra
    string toString(Node *node) const;
    void destroy(Node *&node);
    void collectSubstring(Node *node, int start, int length, int &currentIndex, string &result) const; // extra

public:
    Rope();
    ~Rope();

    int length() const;
    bool empty() const;
    char charAt(int index) const;
    string substring(int start, int length) const;
    void insert(int index, const string &s);
    void deleteRange(int start, int length);
    string toString() const;
    // extra function
    int dfsFindFirst(Node *node, char c, int &index) const;
    int findFirst(char c) const;

    int countChar(Node *node, char c) const;
    void collectCharIndices(Node *node, char c, int *&result, int &idx, int &pos) const;
    int *findAll(char c) const;
    // friend class TestHelper; // ex
    // friend class TestHelper;

#ifdef TESTING
    friend class TestHelper;
#endif
};

class RopeTextBuffer
{
public:
    class HistoryManager;

private:
    Rope rope;
    int cursorPos;
    HistoryManager *history;

public:
    RopeTextBuffer();
    ~RopeTextBuffer();

    void insert(const string &s);
    void deleteRange(int length);
    void replace(int length, const string &s);
    void moveCursorTo(int index);
    void moveCursorLeft();
    void moveCursorRight();
    int getCursorPos() const;
    string getContent() const;
    int findFirst(char c) const;
    int *findAll(char c) const;
    void undo();
    void redo();
    void clear();
    void printHistory() const;
    // friend class TestHelper;
#ifdef TESTING
    friend class TestHelper;
#endif
};

class RopeTextBuffer::HistoryManager
{
public:
    struct Action
    {
        string actionName;
        int cursorBefore;
        int cursorAfter;
        string data;
        string newData;

        Action(const string &actionName = "", int cursorBefore = 0, int cursorAfter = 0, const string &data = "")
            : actionName(actionName), cursorBefore(cursorBefore),
              cursorAfter(cursorAfter), data(data) {}
        Action(const string &name, int cb, int ca, const string &oldD, const string &newD)
            : actionName(name), cursorBefore(cb), cursorAfter(ca), data(oldD), newData(newD) {}
    };
    // static const int maxSize = 1000;
    // string *newData;
    Action *history;
    std::string *undoPayloadStack;
    Action *redoStack;
    std::string *redoPayloadStack;
    int historySize;
    int redoSize;
    int historyCap;
    int redoCap;

    void ensureCapacity(Action *&arr, int &cap, int size);

    // TODO: may provide some attributes

public:
    HistoryManager();
    ~HistoryManager();
    void addAction(const Action &a);
    bool canUndo() const;
    bool canRedo() const;
    void printHistory() const;
    // extra
    void clear();
    Action popUndo();
    Action popRedo();
    void pushUndoPayload(const std::string &payload)
    {
        undoPayloadStack[historySize - 1] = payload;
    }

    std::string popUndoPayload()
    {
        return undoPayloadStack[historySize]; // hoặc theo index undo hiện tại
    }

    std::string popRedoPayload()
    {
        return redoPayloadStack[redoSize]; // hoặc theo index redo hiện tại
    }
    // string popNewData();
    // friend class TestHelper;
#ifdef TESTING
    friend class TestHelper;
#endif
};

#endif // ROPE_TEXTBUFFER_H
