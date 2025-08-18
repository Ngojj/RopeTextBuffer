#include "RopeTextBuffer.h"

Rope::Node::Node() : left(nullptr), right(nullptr), data(""), weight(0), height(1), balance(EH) {}
Rope::Node::Node(const std::string &s)
    : left(nullptr), right(nullptr), data(s), weight(s.length()), height(1), balance(EH) {}

bool Rope::Node::isLeaf() const
{
    return left == nullptr && right == nullptr;
}
// ----------------- Rope -----------------
Rope::Rope()
{
    // TODO
    root = nullptr;
    // totalLength = 0;
}

Rope::~Rope()
{
    // TODO
    destroy(root);
    // totalLength = 0;
}
/* public methods*/
int Rope::length() const
{
    return getTotalLength(root);
    // return this->totalLength;
}
bool Rope::empty() const
{
    return this->root == nullptr;
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
    if (start < 0 || start > getTotalLength(root))
        throw ::out_of_range("Index is invalid!");
    if (length < 0 || start + length > getTotalLength(root))
        throw ::out_of_range("Length is invalid!");

    string result;
    int currentIndex = 0;

    collectSubstring(root, start, length, currentIndex, result);

    return result;
}
void Rope::insert(int index, const string &s)
{
    if (index < 0 || index > getTotalLength(root))
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

    // this->totalLength += s.length();
}
void Rope::deleteRange(int start, int length)
{
    if (start < 0 || start > getTotalLength(root))
        throw ::out_of_range("Index is invalid!");
    if (length < 0 || start + length > getTotalLength(root))
        throw ::out_of_range("Length is invalid!");
    Node *left = nullptr;
    Node *right = nullptr;
    split(root, start, left, right);

    Node *middle = nullptr;
    Node *right2 = nullptr;
    split(right, length, middle, right2);

    destroy(middle);

    root = concatNodes(left, right2);
    // this->totalLength -= length;
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
    if (!node)
        return;

    // Cập nhật weight
    if (node->left)
        node->weight = getTotalLength(node->left);
    else
        node->weight = (node->isLeaf() ? node->data.length() : 0);

    // Cập nhật height
    int leftHeight = node->left ? node->left->height : 0;
    int rightHeight = node->right ? node->right->height : 0;
    node->height = 1 + std::max(leftHeight, rightHeight);

    // Cập nhật balance factor
    int diff = leftHeight - rightHeight;
    if (diff > 0)
        node->balance = Node::LH;
    else if (diff < 0)
        node->balance = Node::RH;
    else
        node->balance = Node::EH;
}

Rope::Node *Rope::rotateLeft(Node *x)
{
    if (!x || !x->right)
        return x; // check null an toàn

    Node *y = x->right;
    Node *T2 = y->left;

    // Thực hiện xoay
    x->right = T2;
    y->left = x;

    // Nếu có parent pointer thì cập nhật ở đây
    // if (T2) T2->parent = x;
    // y->parent = x->parent;
    // x->parent = y;

    // Cập nhật lại thông tin node
    update(x);
    update(y);

    return y;
}

Rope::Node *Rope::rotateRight(Node *y)
{
    if (!y || !y->left)
        return y; // check null an toàn

    Node *x = y->left;
    Node *T2 = x->right;

    // Thực hiện xoay
    y->left = T2;
    x->right = y;

    // Nếu có parent pointer thì cập nhật ở đây
    // if (T2) T2->parent = y;
    // x->parent = y->parent;
    // y->parent = x;

    // Cập nhật lại thông tin node
    update(y);
    update(x);

    return x;
}
Rope::Node *Rope::rebalance(Node *node)
{
    if (!node)
        return nullptr;

    update(node);

    int bf = height(node->left) - height(node->right);

    if (bf > 1)
    {
        if (height(node->left->left) >= height(node->left->right))
        {
            // LL
            return rotateRight(node);
        }
        else
        {
            // LR
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
    }
    else if (bf < -1)
    {
        if (height(node->right->right) >= height(node->right->left))
        {
            // RR
            return rotateLeft(node);
        }
        else
        {
            // RL
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
    }

    return node; // không cần xoay
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
        // Leaf node rìa: trả trực tiếp nếu index ngoài phạm vi
        if (index <= 0)
        {
            outLeft = nullptr;
            outRight = node;
            return;
        }
        if (index >= (int)node->data.length())
        {
            outLeft = node;
            outRight = nullptr;
            return;
        }

        // Tách leaf node
        outLeft = new Node(node->data.substr(0, index));
        outRight = new Node(node->data.substr(index));
        return;
    }

    // Index nằm trong cây nội bộ
    if (index < node->weight)
    {
        Node *l, *r;
        split(node->left, index, l, r);

        outLeft = l;

        if (r || node->right)
        {
            Node *newRight = new Node();
            newRight->left = r;
            newRight->right = node->right;
            update(newRight);
            outRight = rebalance(newRight);
        }
        else
            outRight = nullptr;
    }
    else
    {
        Node *l, *r;
        split(node->right, index - node->weight, l, r);

        outRight = r;

        if (node->left || l)
        {
            Node *newLeft = new Node();
            newLeft->left = node->left;
            newLeft->right = l;
            update(newLeft);
            outLeft = rebalance(newLeft);
        }
        else
            outLeft = nullptr;
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

    string oldStr = rope.substring(cursorPos, length);
    int before = cursorPos;
    rope.deleteRange(cursorPos, length);
    rope.insert(cursorPos, s);
    cursorPos += (int)s.size();
    history->addAction(HistoryManager::Action("replace", before, cursorPos, oldStr, s));
}
void RopeTextBuffer::moveCursorTo(int index)
{
    if (index < 0 || index > rope.length())
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
    if (this->cursorPos == rope.length())
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
        rope.deleteRange(act.cursorBefore, (int)act.newData.size());
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
        rope.deleteRange(act.cursorBefore, (int)act.data.size());
        rope.insert(act.cursorBefore, act.newData);
        cursorPos = act.cursorBefore + (int)act.newData.size();
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
void RopeTextBuffer::HistoryManager::ensureCapacity(Action *&arr, int &cap, int size)
{
    if (size < cap)
        return; // còn chỗ, không cần tăng

    // tăng gấp đôi capacity
    int newCap = cap * 2;
    Action *newArr = new Action[newCap];

    // copy dữ liệu cũ sang
    for (int i = 0; i < size; i++)
    {
        newArr[i] = arr[i];
    }

    // giải phóng mảng cũ
    delete[] arr;
    arr = newArr;
    cap = newCap;
}

void RopeTextBuffer::HistoryManager::addAction(const Action &a)
{
    if (a.actionName == "delete" || a.actionName == "insert")
    {
        redoSize = 0;
    }

    ensureCapacity(history, historyCap, historySize);
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
    cout << "]";
}
RopeTextBuffer::HistoryManager::Action RopeTextBuffer::HistoryManager::popUndo()
{
    Action act = history[--historySize];

    ensureCapacity(redoStack, redoCap, redoSize);
    redoStack[redoSize++] = act;
    return act;
}
RopeTextBuffer::HistoryManager::Action RopeTextBuffer::HistoryManager::popRedo()
{
    Action act = redoStack[--redoSize];

    ensureCapacity(history, historyCap, historySize);
    history[historySize++] = act;
    return act;
}
void RopeTextBuffer::HistoryManager::clear()
{
    historySize = 0;
    redoSize = 0;
}
// TODO: implement other methods of HistoryManager
