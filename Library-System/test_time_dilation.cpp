
#include "LibrarySystem.hpp"
#include <iostream>
#include <cassert>
using namespace std;

#ifdef _WIN32
    #include <windows.h>
    void sleepSeconds(int s) { Sleep(s * 1000); }
#else
    #include <unistd.h>
    void sleepSeconds(int s) { sleep(s); }
#endif

//  TEST 1: In-session dilation rate should be exactly 1 real second = 1 simulated day ----
void test_dilation_rate() {
    cout << "\n=== TEST 1: Dilation rate ===\n";
    LibrarySystem sys;
    time_t t0 = sys.getSimulatedTime();
    sleepSeconds(3);
    time_t t1 = sys.getSimulatedTime();
    cout << "Delta = " << (t1 - t0) << " simulated seconds (expected 259200 = 3*86400)\n";
    cout << ((t1 - t0) >= 259200 ? "PASS" : "FAIL") << "\n"; // >= since sleep can run slightly long
}

// TEST 2: Card-based issue + free 90-day window, no fine if returned within window ----
void test_card_within_window() {
    cout << "\n=== TEST 2: Card issue, return on time ===\n";
    LibrarySystem sys;
    sys.registerUser("u1", "Alice");
    sys.addBook(1, "Clean Code", "Robert Martin");
    sys.buyOrRenewLibraryCard("u1");
    sys.issueBook("u1", 1);
    sys.returnBook("u1", 1); // returned almost immediately -> 0 days -> no fine expected
}

// TEST 3: Non-card issue, returned on time, no fine ----
void test_nocard_within_window() {
    cout << "\n=== TEST 3: No-card issue, return on time ===\n";
    LibrarySystem sys;
    sys.registerUser("u2", "Bob");
    sys.addBook(2, "Pragmatic Programmer", "Hunt");
    sys.issueBook("u2", 2);
    sys.returnBook("u2", 2); // 0 days -> no fine expected
}

//  TEST 4: Double-checkout rejection ----
void test_double_issue_rejected() {
    cout << "\n=== TEST 4: Cannot issue an already-issued book ===\n";
    LibrarySystem sys;
    sys.registerUser("u3", "Carol");
    sys.registerUser("u4", "Dave");
    sys.addBook(3, "Design Patterns", "GoF");
    sys.issueBook("u3", 3);
    sys.issueBook("u4", 3); 
}

// ---- TEST 5: Card expiry correctly falls back to paid rental ----
// This needs TWO separate runs of the program with a manual edit in between,
// because within a single run almost no real time passes, so a freshly
// bought card can never appear expired.
//
//   Step 1: ./test_dilation 5a
//   Step 2: open users.txt, find Eve's line (starts with "u5,Eve,1,"),
//           and change the big number in the 4th comma-separated field
//           (the expiry timestamp) to "1". That guarantees it reads as
//           a date in the past, since "1" (1 sec after epoch, 1970) is
//           far earlier than any current simulated time.
//   Step 3: ./test_dilation 5b
//           -> expect "Notice: User's library card expired!" + a $2 charge
void test_expired_card_setup() {
    cout << "\n=== TEST 5a: Setup (register + buy card) ===\n";
    LibrarySystem sys;
    sys.registerUser("u5", "Eve");
    sys.addBook(4, "Refactoring", "Fowler");
    sys.buyOrRenewLibraryCard("u5");
    cout << "Now edit users.txt: set Eve's expiry field to 1, then run test 5b.\n";
}

void test_expired_card_verify() {
    cout << "\n=== TEST 5b: Verify expired card falls back to rental ===\n";
    LibrarySystem sys; // re-loads Eve from users.txt, including your edited expiry
    sys.issueBook("u5", 4); // expec : expiry notice + $2 rental charge, NOT free
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: ./test_dilation <test: 1, 2, 3, 4, 5a, 5b>\n";
        cout << "Run each test in a clean directory (no leftover .txt files) for predictable results.\n";
        return 1;
    }
    string n = argv[1];
    if (n == "1") test_dilation_rate();
    else if (n == "2") test_card_within_window();
    else if (n == "3") test_nocard_within_window();
    else if (n == "4") test_double_issue_rejected();
    else if (n == "5a") test_expired_card_setup();
    else if (n == "5b") test_expired_card_verify();
    else cout << "Unknown test number\n";
    return 0;
}
