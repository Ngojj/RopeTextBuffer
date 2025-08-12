#include <iostream>
#include "RopeTextBuffer.h"

using namespace std;

void testInsertAndToString()
{
    Rope r;
    r.insert(0, "anh");
    r.insert(3, "yeu");
    r.insert(6, "em");

    string expected = "anhyeuem";
    string result = r.toString();
    cout << "[Insert + toString] ";
    cout << (result == expected ? "PASSED" : "FAILED") << " → " << result << endl;
}

void testCharAt()
{
    Rope r;
    r.insert(0, "abcde");

    cout << "[charAt] ";
    try
    {
        bool passed = (r.charAt(0) == 'a' && r.charAt(4) == 'e');
        cout << (passed ? "PASSED" : "FAILED") << endl;
    }
    catch (...)
    {
        cout << "FAILED (Exception thrown)" << endl;
    }
}

void testSubstring()
{
    Rope r;
    r.insert(0, "abcdefgh");

    string sub = r.substring(2, 4); // should be "cdef"
    cout << "[substring] ";
    cout << (sub == "cdef" ? "PASSED" : "FAILED") << " → " << sub << endl;
}

void testDeleteRange()
{
    Rope r;
    r.insert(0, "abcdefgh");
    r.deleteRange(2, 4); // delete "cdef" → "abgh"

    string result = r.toString();
    cout << "[deleteRange] ";
    cout << (result == "abgh" ? "PASSED" : "FAILED") << " → " << result << endl;
}

void testLengthAndEmpty()
{
    Rope r;
    cout << "[empty + length] ";
    bool passed = r.empty() && r.length() == 0;
    r.insert(0, "abc");
    passed &= (!r.empty() && r.length() == 3);
    cout << (passed ? "PASSED" : "FAILED") << endl;
}

void test_LMS()
{
    RopeTextBuffer tb;
    tb.insert("A");
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.insert("CSE");
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.insert("HCMUT");
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.moveCursorLeft();
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.insert("123");
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.moveCursorTo(4);
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.deleteRange(3);
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.printHistory();
    tb.undo();
    tb.printHistory();
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.undo();
    tb.printHistory();
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.undo();
    tb.printHistory();
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.redo();
    tb.printHistory();
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.redo();
    tb.printHistory();
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
    tb.redo();
    tb.printHistory();
    cout << tb.getContent() << " cursor : " << tb.getCursorPos() << endl;
}

int main()
{
    // testInsertAndToString();
    // testCharAt();
    // testSubstring();
    // testDeleteRange();
    // testLengthAndEmpty();
    // RopeTextBuffer tb;
    // tb.insert("Hi");
    // tb.insert("llo");
    // cout << tb.getContent() << ", cursor=" << tb.getCursorPos() << endl;
    // tb.moveCursorLeft();
    // cout << tb.getContent() << ", cursor=" << tb.getCursorPos() << endl;
    // tb.deleteRange(1);
    // cout << tb.getContent() << ", cursor=" << tb.getCursorPos() << endl;
    // tb.printHistory();
    // RopeTextBuffer tb;
    // tb.insert("Hello");
    // tb.moveCursorTo(4);
    // tb.deleteRange(1);
    // cout << "sample_06: " << tb.getContent() << ", cursor=" << tb.getCursorPos() << endl;

    test_LMS();

    return 0;
}
