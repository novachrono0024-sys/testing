#include "LibrarySystem.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath> // Added for fine rounding control if needed

using namespace std;

LibrarySystem::LibrarySystem() {
    // Record the exact real-world time this specific runtime session started
    sessionStartTime = time(0);

    // Persistent Database Clock Anchor
    ifstream inClock(clockFile);
    if (inClock >> baseSimulatedTime) {
        inClock.close();
    } else {
        inClock.close();
        baseSimulatedTime = time(0); // Anchor library creation exactly to right now
        ofstream outClock(clockFile);
        outClock << baseSimulatedTime;
        outClock.close();
    }

    handleBankStartup(); 
    loadUsersFromFile();
    loadBooksFromFile();
}

LibrarySystem::~LibrarySystem() {
    // Automatically save all states and current simulated clock on graceful shutdown
    saveAllToFiles(); 
    
    for (auto it = userDatabase.begin(); it != userDatabase.end(); ++it) {
        delete it->second;
    }
}

// TIME-DILATION SIMULATION ENGINE

time_t LibrarySystem::getSimulatedTime() const {
    // FIX: 1 Real Second = 1 Simulated Day (86,400 seconds) during active session.
    // This prevents massive multi-century leaps when restarting the program later.
    time_t realSecondsPassed = time(0) - sessionStartTime;
    return baseSimulatedTime + (realSecondsPassed * 86400);
}

string LibrarySystem::formatSimulatedTime(time_t t) const {
    if(t == 0) return "N/A";
    struct tm* timeinfo = localtime(&t);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return string(buffer);
}

// USER SUBSCRIPTION & REGISTRATION LOGIC
void LibrarySystem::registerUser(string id, string name) {
    if (userDatabase.find(id) != userDatabase.end()) {
        cout << "Error: User ID already registered!\n"; return;
    }
    userDatabase[id] = new User(id, name);
    transactionMap[id] = vector<BorrowedBook>();
    appendUserToFile(userDatabase[id]);
    cout << "User '" << name << "' registered successfully.\n";
}

void LibrarySystem::removeUser(string id) {
    if (userDatabase.find(id) == userDatabase.end()) {
        cout << "Error: User ID not found!\n"; return;
    }
    if (!transactionMap[id].empty()) {
        cout << "Error: Cannot remove user. They have outstanding book rentals/loans!\n"; return;
    }
    delete userDatabase[id];
    userDatabase.erase(id);
    transactionMap.erase(id);
    saveAllToFiles();
    cout << "User removed from system registry.\n";
}

void LibrarySystem::buyOrRenewLibraryCard(string userId) {
    if (userDatabase.find(userId) == userDatabase.end()) {
        cout << "Error: User not found.\n"; return;
    }
    User* u = userDatabase[userId];
    u->purchaseOrRenewCard(getSimulatedTime());
    
    bankBalance += CARD_COST;
    saveBankBalance();
    saveAllToFiles();
    
    cout << "Success! Library card issued/renewed for " << u->getName() << ".\n";
    cout << "Card Expiry Date (Simulated): " << formatSimulatedTime(u->getCardExpiryDate()) << "\n";
}


// CORE BOOK OPERATIONS
void LibrarySystem::addBook(int id, string title, string author) {
    if (bookDatabase.find(id) != bookDatabase.end()) {
        cout << "Error: Book ID already exists!\n"; return;
    }
    int f, s, r, c;
    if (!findEmptyCoordinate(f, s, r, c)) {
        cout << "Error: No physical room left in our blueprint layout!\n"; return;
    }
    bookDatabase[id] = Book(id, title, author, false, "NONE", f, s, r, c);
    appendBookToFile(bookDatabase[id]);
    cout << "Book added to Floor " << f << ", Shelf " << s << ", Row " << r << ", Col " << c << ".\n";
}

void LibrarySystem::removeBook(int id) {
    if (bookDatabase.find(id) == bookDatabase.end()) {
        cout << "Error: Book not found!\n"; return;
    }
    if (bookDatabase[id].isIssued) {
        cout << "Error: Book is actively checked out. Cannot delete tracking trace.\n"; return;
    }
    bookDatabase.erase(id);
    saveAllToFiles();
    cout << "Book trace deleted from databases.\n";
}

// TIME-AWARE CHECKOUTS & FINES ENGINE
void LibrarySystem::issueBook(string userId, int bookId) {
    if (userDatabase.find(userId) == userDatabase.end() || bookDatabase.find(bookId) == bookDatabase.end()) {
        cout << "Error: Invalid User or Book ID.\n"; return;
    }

    User* u = userDatabase[userId];
    Book& b = bookDatabase[bookId];

    if (b.isIssued) {
        cout << "Error: Book is already rented out to user " << b.borrowedByUserId << ".\n"; return;
    }
    time_t now = getSimulatedTime();

    if (u->checkAndApplyExpiry(now)) {
        cout << "Notice: User's library card expired! Reverting transaction to non-card tier.\n";
    }

    BorrowedBook tx;
    tx.bookId = bookId;
    tx.borrowDate = now;

    if (u->getHasLibraryCard()) {
        tx.issuedWithCard = true;
        cout << "Card Validated! Book issued for FREE (3 months window allocation).\n";
    } else {
        tx.issuedWithCard = false;
        bankBalance += RENTAL_COST;
        saveBankBalance();
        cout << "No Active Card. Charged upfront standard rent: $" << RENTAL_COST << " (1 month window allocation).\n";
    }

    b.isIssued = true;
    b.borrowedByUserId = userId;
    transactionMap[userId].push_back(tx);
    saveAllToFiles();
}

void LibrarySystem::returnBook(string userId, int bookId) {
    if (transactionMap.find(userId) == transactionMap.end()) {
        cout << "No active rentals recorded for this user.\n"; return;
    }

    time_t currentTime = getSimulatedTime();
    vector<BorrowedBook>& rentals = transactionMap[userId];

    for (auto it = rentals.begin(); it != rentals.end(); ++it) {
        if (it->bookId == bookId) {
            double secondsElapsed = difftime(currentTime, it->borrowDate);
            int daysElapsed = static_cast<int>(secondsElapsed / 86400);
            if (daysElapsed < 0) daysElapsed = 0;

            double fine = 0.0;

            if (it->issuedWithCard) {
                if (daysElapsed > 90) fine = (daysElapsed - 90) * 0.25;
            } else {
                if (daysElapsed > 30) fine = (daysElapsed - 30) * 0.01;
            }

            cout << "\n--- Return Process Report ---\n";
            cout << "Days Borrowed (Simulated): " << daysElapsed << " days.\n";
            if (fine > 0) {
                cout << "WARNING: Book is OVERDUE! Fine Accumulated: $" << fine << "\n";
                bankBalance += static_cast<int>(ceil(fine)); 
                saveBankBalance();
            } else {
                cout << "Book returned on time. No fines added.\n";
            }

            bookDatabase[bookId].isIssued = false;
            bookDatabase[bookId].borrowedByUserId = "NONE";
            rentals.erase(it);
            saveAllToFiles();
            return;
        }
    }
    cout << "Error: This specific book was not flagged under this user's rental ledger.\n";
}


// REORDER PROCUREMENT & SPACE SEARCHES
void LibrarySystem::locateBookByTitle(string title) {
    bool foundAny = false;
    bool foundAvailable = false;
    cout << "\n--- Front Desk Locator System ---\n";
    
    for (auto it = bookDatabase.begin(); it != bookDatabase.end(); ++it) {
        if (it->second.title == title) {
            foundAny = true;
            if (!it->second.isIssued) {
                cout << "[AVAILABLE] ID: " << it->second.id 
                     << " -> Floor: " << it->second.floor << ", Shelf: " << it->second.shelf 
                     << ", Row: " << it->second.row << ", Col: " << it->second.col << "\n";
                foundAvailable = true;
            }
        }
    }
    if (!foundAny) {
        cout << "We do not carry that title in our inventory archives.\n";
    } else if (!foundAvailable) {
        cout << "All copies matching this title are currently rented out.\n";
    }
}

void LibrarySystem::buyBookFromFactory(int newId, string title, string author) {
    if (bookDatabase.find(newId) != bookDatabase.end()) {
        cout << "Error: ID collision detected.\n"; return;
    }
    for (auto it = bookDatabase.begin(); it != bookDatabase.end(); ++it) {
        if (it->second.title == title && !it->second.isIssued) {
            cout << "Factory Purchase Denied: You still have available shelf copies of this title!\n"; return;
        }
    }
    if (bankBalance < FACTORY_COST) {
        cout << "Factory Purchase Denied: Treasury contains insufficient funding.\n"; return;
    }
    int f, s, r, c;
    if (!findEmptyCoordinate(f, s, r, c)) {
        cout << "Factory Purchase Denied: Physical building allocation matrix is full.\n"; return;
    }

    bankBalance -= FACTORY_COST;
    saveBankBalance();

    bookDatabase[newId] = Book(newId, title, author, false, "NONE", f, s, r, c);
    appendBookToFile(bookDatabase[newId]);
    cout << "Success! Procured new copy from factory. Placed on Floor " << f << ", Shelf " << s << ".\n";
}


// SYSTEM SUBSIDIARY HELPERS
void LibrarySystem::handleBankStartup() {
    ifstream inBank(bankFile);
    if (inBank >> bankBalance) {
    } else {
        bankBalance = 1000; 
    }
    inBank.close();
    saveBankBalance();
}

void LibrarySystem::saveBankBalance() {
    ofstream outBank(bankFile); outBank << bankBalance; outBank.close();
}

void LibrarySystem::displayBankBalance() {
    cout << "\n--- Consolidated Library Financial Ledger: $" << bankBalance << " ---\n";
}

bool LibrarySystem::isCoordinateOccupied(int f, int s, int r, int c) {
    for (auto it = bookDatabase.begin(); it != bookDatabase.end(); ++it) {
        if (it->second.floor == f && it->second.shelf == s && it->second.row == r && it->second.col == c) return true;
    }
    return false;
}

bool LibrarySystem::findEmptyCoordinate(int &f, int &s, int &r, int &c) {
    for (f = 1; f <= 3; f++) {
        for (s = 1; s <= 10; s++) {
            for (r = 1; r <= 7; r++) {
                for (c = 1; c <= 7; c++) {
                    if (!isCoordinateOccupied(f, s, r, c)) return true;
                }
            }
        }
    }
    return false;
}

void LibrarySystem::displayInventory() {
    cout << "\n--- System Inventory Matrix ---\n";
    for (auto it = bookDatabase.begin(); it != bookDatabase.end(); ++it) {
        Book b = it->second;
        cout << "ID: " << b.id << " | Title: " << b.title << " | Coordinates: F" << b.floor << "-S" << b.shelf << "-R" << b.row << "-C" << b.col;
        if (b.isIssued) cout << " | [RENTED OUT -> User: " << b.borrowedByUserId << "]\n";
        else cout << " | [AVAILABLE ON SHELF]\n";
    }
}

void LibrarySystem::displayUsers() {
    time_t currentTime = getSimulatedTime();
    cout << "\n--- Public Patron Registry ---\n";
    for (auto it = userDatabase.begin(); it != userDatabase.end(); ++it) {
        User* u = it->second;
        u->checkAndApplyExpiry(currentTime); 
        cout << "ID: " << u->getUserId() << " | Name: " << u->getName() 
             << " | Subscription Card: " << (u->getHasLibraryCard() ? "ACTIVE" : "NONE")
             << " | Card Expiry: " << formatSimulatedTime(u->getCardExpiryDate()) << "\n";
    }
}


// COMPLEX FILE DATABASE PARSING
void LibrarySystem::appendBookToFile(const Book& b) {
    ofstream outBooks(bookFile, ios::app); 
    outBooks << b.id << "," << b.title << "," << b.author << "," << b.isIssued << "," 
             << b.borrowedByUserId << "," << b.floor << "," << b.shelf << "," << b.row << "," << b.col << "\n";
    outBooks.close();
}

void LibrarySystem::appendUserToFile(User* u) {
    ofstream outUsers(userFile, ios::app); 
    outUsers << u->getUserId() << "," << u->getName() << "," << u->getHasLibraryCard() << "," << u->getCardExpiryDate() << ",NONE\n"; 
    outUsers.close();
}

void LibrarySystem::saveAllToFiles() {
    // Overwrite the clockFile with the current simulated timeline 
    // before closing out to lock down active session progression.
    ofstream outClock(clockFile);
    outClock << getSimulatedTime();
    outClock.close();

    ofstream outBooks(bookFile);
    for (auto it = bookDatabase.begin(); it != bookDatabase.end(); ++it) {
        Book b = it->second;
        outBooks << b.id << "," << b.title << "," << b.author << "," << b.isIssued << "," 
                 << b.borrowedByUserId << "," << b.floor << "," << b.shelf << "," << b.row << "," << b.col << "\n";
    }
    outBooks.close();

    ofstream outUsers(userFile);
    for (auto it = userDatabase.begin(); it != userDatabase.end(); ++it) {
        User* u = it->second;
        outUsers << u->getUserId() << "," << u->getName() << "," << u->getHasLibraryCard() << "," << u->getCardExpiryDate() << ",";
        
        vector<BorrowedBook> rentals = transactionMap[u->getUserId()];
        if (rentals.empty()) outUsers << "NONE";
        else {
            for (size_t i = 0; i < rentals.size(); i++) {
                outUsers << rentals[i].bookId << ":" << rentals[i].borrowDate << ":" << rentals[i].issuedWithCard
                         << (i == rentals.size() - 1 ? "" : " ");
            }
        }
        outUsers << "\n";
    }
    outUsers.close();
}

void LibrarySystem::loadUsersFromFile() {
    ifstream inFile(userFile); if (!inFile) return;
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string uid, name, hasCardStr, expiryStr, bookTokens;
        
        getline(ss, uid, ','); getline(ss, name, ','); getline(ss, hasCardStr, ','); 
        getline(ss, expiryStr, ','); getline(ss, bookTokens);
        
        if (uid.empty()) continue;

        bool hasCard = (hasCardStr == "1");
        time_t expiry = static_cast<time_t>(stoll(expiryStr));

        userDatabase[uid] = new User(uid, name, hasCard, expiry);
        transactionMap[uid] = vector<BorrowedBook>();

        if (bookTokens != "NONE" && !bookTokens.empty()) {
            stringstream bookSS(bookTokens);
            string token;
            while (bookSS >> token) {
                stringstream tokenSS(token);
                string bidStr, bDateStr, cardIssuedStr;
                getline(tokenSS, bidStr, ':');
                getline(tokenSS, bDateStr, ':');
                getline(tokenSS, cardIssuedStr, ':');

                BorrowedBook rx;
                rx.bookId = stoi(bidStr);
                rx.borrowDate = static_cast<time_t>(stoll(bDateStr));
                rx.issuedWithCard = (cardIssuedStr == "1");
                transactionMap[uid].push_back(rx);
            }
        }
    }
    inFile.close();
}

void LibrarySystem::loadBooksFromFile() {
    ifstream inFile(bookFile); if (!inFile) return;
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string idStr, title, author, statusStr, borrowedBy, fStr, sStr, rStr, cStr;
        
        getline(ss, idStr, ','); getline(ss, title, ','); getline(ss, author, ',');
        getline(ss, statusStr, ','); getline(ss, borrowedBy, ',');
        getline(ss, fStr, ','); getline(ss, sStr, ','); getline(ss, rStr, ','); getline(ss, cStr, ',');

        if (!idStr.empty()) {
            bookDatabase[stoi(idStr)] = Book(stoi(idStr), title, author, (statusStr == "1"), borrowedBy, 
                                             stoi(fStr), stoi(sStr), stoi(rStr), stoi(cStr));
        }
    }
    inFile.close();
}