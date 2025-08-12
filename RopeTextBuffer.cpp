#include "RopeTextBuffer.h"

Rope::Node::Node() : left(nullptr), right(nullptr), data(""), weight(0), height(1), balance(EH) {}
Rope::Node::Node(const std::string &s)
    : left(nullptr), right(nullptr), data(s), weight(s.length()), height(1), balance(EH) {}
// Rope::Node::Node(const string &s)
// {
//     left = right = nullptr;
//     data = s;
//     weight = s.length();
//     height = 1;
//     balance = EH;
// }
bool Rope::Node::isLeaf() const
{
    return left == nullptr && right == nullptr;
}
// ----------------- Rope -----------------
Rope::Rope()
{
    // TODO
    root = nullptr;
    totalLength = 0;
}

Rope::~Rope()
{
    // TODO
    destroy(root);
    totalLength = 0;
}
/* public methods*/
int Rope::length() const
{
    // return getTotalLength(root);
    return this->totalLength;
}
bool Rope::empty() const
{
    return this->totalLength == 0;
}
char Rope::charAt(int index) const
{
    return charAt(root, index);
}
// extra
void Rope::collectSubstring(Node *node, int start, int length, int &currentIndex, string &result) const
{
    if (!node || result.length() >= length)
        return;

    if (node->isLeaf())
    {
        int leafLength = node->data.length();
        int leafStart = currentIndex;
        int leafEnd = currentIndex + leafLength;

        // Nếu đoạn cần lấy không giao với node này, bỏ qua
        if (leafEnd <= start || leafStart >= start + length)
        {
            currentIndex += leafLength;
            return;
        }

        // Cắt ra phần giao nhau
        int from = max(start, leafStart);
        int to = min(start + length, leafEnd);

        result += node->data.substr(from - leafStart, to - from);
        currentIndex += leafLength;
        return;
    }

    // Duyệt trái
    collectSubstring(node->left, start, length, currentIndex, result);
    // Duyệt phải
    collectSubstring(node->right, start, length, currentIndex, result);
}
string Rope::substring(int start, int length) const
{

    string result;
    int currentIndex = 0;

    collectSubstring(root, start, length, currentIndex, result);

    return result;
}
void Rope::insert(int index, const string &s)
{
    if (index < 0 || index > this->totalLength)
        throw out_of_range("Index is invalid!");
    if (s.empty())
        return;

    Node *left = nullptr, *right = nullptr;
    split(root, index, left, right);

    // Tạo chuỗi trung gian từ s
    Node *middle = nullptr;
    for (size_t i = 0; i < s.length(); i += CHUNK_SIZE)
    {
        string chunk = s.substr(i, CHUNK_SIZE);
        Node *newLeaf = new Node(chunk);
        middle = concatNodes(middle, newLeaf);
    }

    // Nối left + middle + right
    Node *temp = concatNodes(left, middle);
    root = concatNodes(temp, right);

    this->totalLength += s.length();
}
void Rope::deleteRange(int start, int length)
{
    if (start < 0 || start + length > totalLength)
        throw ::out_of_range("Index is invalid!");
    Node *left = nullptr;
    Node *right = nullptr;
    split(root, start, left, right);

    Node *middle = nullptr;
    Node *right2 = nullptr;
    split(right, length, middle, right2);

    destroy(middle);

    root = concatNodes(left, right2);
    this->totalLength -= length;
}
string Rope::toString() const
{
    return toString(root);
}
/* end public methods*/

/*private methods*/
int Rope::height(Node *node) const
{
    return node ? node->height : 0;
}
int Rope::getTotalLength(Node *node) const
{
    if (node == nullptr)
        return 0;
    if (node->isLeaf())
        return node->data.length();
    return node->weight + getTotalLength(node->right);
}
/*Kiểm tra và cập nhật lại các thông tin: weight, height và balance của node được truyền
vào*/
void Rope::update(Node *node)
{
    if (node == nullptr)
        return;
    if (node->left != nullptr)
    {
        node->weight = getTotalLength(node->left);
    }
    else
        node->weight = 0;

    int leftHeight = node->left ? node->left->height : 0;
    int rightHeight = node->right ? node->right->height : 0;
    node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);

    int balanced = leftHeight - rightHeight;
    if (balanced == 1)
        node->balance = Node::LH;
    else if (balanced == 0)
        node->balance = Node::EH;
    else if (balanced == -1)
        node->balance = Node::RH;
}
Rope::Node *Rope::rotateLeft(Node *x)
{
    Node *rightNode = x->right;
    Node *T2 = rightNode->left;

    x->right = T2;
    rightNode->left = x;

    update(x);
    update(rightNode);

    return rightNode;
}

Rope::Node *Rope::rotateRight(Node *y)
{
    Node *leftNode = y->left;
    Node *T2 = leftNode->right;

    y->left = T2;
    leftNode->right = y;

    update(y);
    update(leftNode);

    return leftNode;
}
Rope::Node *Rope::rebalance(Node *node)
{
    if (node == nullptr)
        return nullptr;

    update(node);

    if (node->balance == Node::LH)
    {
        if (node->left != nullptr && node->left->balance == Node::RH)
        {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }
    else if (node->balance == Node::RH)
    {
        if (node->right != nullptr && node->right->balance == Node::LH)
        {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }
    else
        return node;
}
void Rope::split(Node *node, int index, Node *&outLeft, Node *&outRight)
{
    if (!node)
    {
        outLeft = nullptr;
        outRight = nullptr;
        return;
    }

    if (node->isLeaf())
    {
        string leftData = node->data.substr(0, index);
        string rightData = node->data.substr(index);

        outLeft = leftData.empty() ? nullptr : new Node(leftData);
        outRight = rightData.empty() ? nullptr : new Node(rightData);
        return;
    }

    if (index < node->weight)
    {
        Node *l1 = nullptr, *l2 = nullptr;
        split(node->left, index, l1, l2);

        Node *newRight = nullptr;
        if (l2 || node->right)
        {
            newRight = new Node();
            newRight->left = l2;
            newRight->right = node->right;
            update(newRight);
        }

        outLeft = l1;
        outRight = newRight;
    }
    else if (index > node->weight)
    {
        Node *r1 = nullptr, *r2 = nullptr;
        split(node->right, index - node->weight, r1, r2);

        Node *newLeft = nullptr;
        if (node->left || r1)
        {
            newLeft = new Node();
            newLeft->left = node->left;
            newLeft->right = r1;
            update(newLeft);
        }

        outLeft = newLeft;
        outRight = r2;
    }
    else // index == weight
    {
        outLeft = node->left;
        outRight = node->right;

        if (outLeft)
        {
            Node *newLeft = new Node();
            newLeft->left = outLeft;
            newLeft->right = nullptr;
            update(newLeft);
            outLeft = newLeft;
        }

        if (outRight)
        {
            Node *newRight = new Node();
            newRight->left = nullptr;
            newRight->right = outRight;
            update(newRight);
            outRight = newRight;
        }
    }
}

Rope::Node *Rope::concatNodes(Node *left, Node *right)
{
    if (!left)
        return right;
    if (!right)
        return left;

    // Bước 1: Tạo node gốc mới
    Node *newRoot = new Node();
    newRoot->left = left;
    newRoot->right = right;

    // Bước 2: Cập nhật weight, height, balance
    update(newRoot);

    // Bước 3: Cân bằng nếu cần
    return rebalance(newRoot);
}
char Rope::charAt(Node *node, int index) const
{
    if (node == nullptr)
        throw ::out_of_range("Index is invalid!");
    if (node->isLeaf())
    {
        if (index < 0 || index >= (int)node->data.length())
            throw ::out_of_range("Index is Invalid!");
        return node->data[index];
    }
    if (index < node->weight)
    {
        return charAt(node->left, index);
    }
    else
    {
        return charAt(node->right, index - node->weight);
    }
}
void Rope::toStringHelper(Node *node, std::ostringstream &oss) const
{
    if (!node)
        return;
    if (node->isLeaf())
    {
        oss << node->data;
        return;
    }
    toStringHelper(node->left, oss);
    toStringHelper(node->right, oss);
}

string Rope::toString(Node *node) const
{
    std::ostringstream oss;
    toStringHelper(node, oss);
    return oss.str();
}
void Rope::destroy(Node *&node)
{
    if (node == nullptr)
        return;

    destroy(node->left);
    destroy(node->right);

    delete node;
    node = nullptr;
}
/* end private methods*/

// extra function
int Rope::dfsFindFirst(Node *node, char c, int &index) const
{
    if (!node)
        return -1;

    if (node->isLeaf())
    {
        for (size_t i = 0; i < node->data.length(); ++i)
        {
            if (node->data[i] == c)
                return index + i;
        }
        index += node->data.length(); // cộng số ký tự đã duyệt
        return -1;
    }

    int left = dfsFindFirst(node->left, c, index);
    if (left != -1)
        return left;
    return dfsFindFirst(node->right, c, index);
}
int Rope::findFirst(char c) const
{
    int index = 0;
    return dfsFindFirst(root, c, index);
}
int Rope::countChar(Node *node, char c) const
{
    if (!node)
        return 0;
    if (node->isLeaf())
    {
        int count = 0;
        for (size_t i = 0; i < node->data.length(); ++i)
            if (node->data[i] == c)
                count++;
        return count;
    }
    return countChar(node->left, c) + countChar(node->right, c);
}

void Rope::collectCharIndices(Node *node, char c, int *&result, int &idx, int &pos) const
{
    if (!node)
        return;
    if (node->isLeaf())
    {
        for (size_t i = 0; i < node->data.length(); ++i)
        {
            if (node->data[i] == c)
                result[idx++] = pos + i;
        }
        pos += node->data.length();
        return;
    }
    collectCharIndices(node->left, c, result, idx, pos);
    collectCharIndices(node->right, c, result, idx, pos);
}

int *Rope::findAll(char c) const
{
    int count = countChar(root, c);
    if (count == 0)
        return nullptr;

    int *result = new int[count];
    int idx = 0; // chỉ số trong mảng result
    int pos = 0; // vị trí tổng trong toàn bộ chuỗi
    collectCharIndices(root, c, result, idx, pos);
    return result;
}

// end extra function

// ----------------- RopeTextBuffer -----------------
RopeTextBuffer::RopeTextBuffer()
{
    // TODO
    this->cursorPos = 0;
    this->history = new HistoryManager();
}

RopeTextBuffer::~RopeTextBuffer()
{
    // TODO
    delete this->history;
}

void RopeTextBuffer::insert(const string &s)
{
    rope.insert(this->cursorPos, s);
    history->addAction(RopeTextBuffer::HistoryManager::Action("insert", cursorPos, cursorPos + (int)s.length(), s));
    this->cursorPos += s.length();
}
void RopeTextBuffer::deleteRange(int length)
{
    if (length < 0 || this->cursorPos + length > rope.length())
        throw ::out_of_range("Length is invalid!");
    string deleted = rope.substring(cursorPos, length);
    rope.deleteRange(this->cursorPos, length);
    history->addAction(RopeTextBuffer::HistoryManager::Action("delete", cursorPos, cursorPos, deleted));
}
void RopeTextBuffer::replace(int length, const string &s)
{
    if (length < 0 || this->cursorPos + length > rope.length())
        throw ::out_of_range("Length is invalid!");
    string replaced = rope.substring(cursorPos, length);
    rope.deleteRange(this->cursorPos, length);
    rope.insert(this->cursorPos, s);
    history->addAction(RopeTextBuffer::HistoryManager::Action("replace", cursorPos, cursorPos + (int)s.length(), replaced));
    this->cursorPos += s.length();
}
void RopeTextBuffer::moveCursorTo(int index)
{
    if (index < 0 || index >= rope.length())
        throw ::out_of_range("Index is invalid!");
    history->addAction(RopeTextBuffer::HistoryManager::Action("move", cursorPos, index, "J"));
    this->cursorPos = index;
}
void RopeTextBuffer::moveCursorLeft()
{
    if (this->cursorPos == 0)
        throw ::cursor_error();
    history->addAction(RopeTextBuffer::HistoryManager::Action("move", cursorPos, cursorPos - 1, "L"));
    this->cursorPos--;
}
void RopeTextBuffer::moveCursorRight()
{
    if (this->cursorPos == rope.length() - 1)
        throw ::cursor_error();
    history->addAction(RopeTextBuffer::HistoryManager::Action("move", cursorPos, cursorPos + 1, "R"));
    this->cursorPos++;
}
int RopeTextBuffer::getCursorPos() const
{
    return this->cursorPos;
}
string RopeTextBuffer::getContent() const
{
    return rope.toString();
}

int RopeTextBuffer::findFirst(char c) const
{
    return rope.findFirst(c);
}

int *RopeTextBuffer::findAll(char c) const
{
    return rope.findAll(c);
}
void RopeTextBuffer::clear()
{
    if (rope.length() > 0)
    {
        rope.deleteRange(0, rope.length());
    }
    cursorPos = 0;
    history->clear();
}
void RopeTextBuffer::undo()
{
    if (!history->canUndo())
        return;
    HistoryManager::Action act = history->popUndo();

    if (act.actionName == "insert")
    {
        rope.deleteRange(act.cursorBefore, act.data.length());
        cursorPos = act.cursorBefore;
    }
    else if (act.actionName == "delete")
    {
        rope.insert(act.cursorBefore, act.data);
        cursorPos = act.cursorAfter;
    }
    else if (act.actionName == "replace")
    {
        int insertedLen = act.cursorAfter - act.cursorBefore;
        rope.deleteRange(act.cursorBefore, insertedLen);
        rope.insert(act.cursorBefore, act.data);
        cursorPos = act.cursorBefore;
    }
    else if (act.actionName == "move")
    {
        cursorPos = act.cursorBefore;
    }
}
void RopeTextBuffer::redo()
{
    if (!history->canRedo())
        return;
    HistoryManager::Action act = history->popRedo();

    if (act.actionName == "insert")
    {
        rope.insert(act.cursorBefore, act.data);
        cursorPos = act.cursorAfter;
    }
    else if (act.actionName == "delete")
    {
        rope.deleteRange(act.cursorBefore, act.data.length());
        cursorPos = act.cursorBefore;
    }
    else if (act.actionName == "replace")
    {
        rope.deleteRange(act.cursorBefore, act.data.length());
        rope.insert(act.cursorBefore, rope.substring(act.cursorBefore, act.cursorAfter - act.cursorBefore));
        cursorPos = act.cursorAfter;
    }
    else if (act.actionName == "move")
    {
        cursorPos = act.cursorAfter;
    }
}
void RopeTextBuffer::printHistory() const
{
    history->printHistory();
}
// TODO: implement other methods of TextBuffer

// ----------------- HistoryManager -----------------
RopeTextBuffer::HistoryManager::HistoryManager()
{
    // TODO
    historyCap = 1000;
    redoCap = 1000;
    redoSize = 0;
    historySize = 0;
    history = new Action[historyCap];
    redoStack = new Action[redoCap];
}

RopeTextBuffer::HistoryManager::~HistoryManager()
{
    // TODO
    delete[] history;
    delete[] redoStack;
}
void RopeTextBuffer::HistoryManager::addAction(const Action &a)
{
    if (a.actionName == "delete" || a.actionName == "insert")
    {
        redoSize = 0;
    }
    history[historySize++] = a;
}
bool RopeTextBuffer::HistoryManager::canUndo() const
{
    return historySize > 0;
}
bool RopeTextBuffer::HistoryManager::canRedo() const
{
    return redoSize > 0;
}
void RopeTextBuffer::HistoryManager::printHistory() const
{
    cout << "[";
    for (int i = 0; i < historySize; i++)
    {
        cout << "(" << history[i].actionName << ", "
             << history[i].cursorBefore << ", "
             << history[i].cursorAfter << ", "
             << history[i].data
             << ")";
        if (i != historySize - 1)
            cout << ", ";
    }
    cout << "]" << endl;
}
RopeTextBuffer::HistoryManager::Action RopeTextBuffer::HistoryManager::popUndo()
{
    Action act = history[--historySize];
    redoStack[redoSize++] = act;
    return act;
}
RopeTextBuffer::HistoryManager::Action RopeTextBuffer::HistoryManager::popRedo()
{
    Action act = redoStack[--redoSize];
    history[historySize++] = act;
    return act;
}
void RopeTextBuffer::HistoryManager::clear()
{
    historySize = 0;
}
// TODO: implement other methods of HistoryManager
