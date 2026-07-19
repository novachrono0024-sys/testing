#ifndef LIBRARYSYSTEM_HPP
#define LIBRARYSYSTEM_HPP

#include <unordered_map>
#include <vector>
#include <string>
#include <ctime>
#include "Book.hpp"
#include "User.hpp"

// Transaction structure keeping track of temporal realities
struct BorrowedBook {
    int bookId;
    time_t borrowDate;   
    bool issuedWithCard; 
};

class LibrarySystem {
private:
    std::unordered_map<int, Book> bookDatabase;
    std::unordered_map<std::string, User*> userDatabase;
    std::unordered_map<std::string, std::vector<BorrowedBook>> transactionMap; 

    // Time Tracking Anchors
    time_t baseSimulatedTime; // Permanent anchor point saved inside system_clock.txt
    time_t sessionStartTime;   // Real-world time when the active program execution started

    const std::string bookFile = "books.txt";
    const std::string userFile = "users.txt";
    const std::string bankFile = "bank.txt"; 
    const std::string clockFile = "system_clock.txt"; 

    int bankBalance; 
    const int CARD_COST = 15;     
    const int RENTAL_COST = 2;    
    const int FACTORY_COST = 300; 

    void loadUsersFromFile();
    void loadBooksFromFile();
    void saveAllToFiles();
    void appendBookToFile(const Book& b);
    void appendUserToFile(User* u);
    
    void handleBankStartup();
    void saveBankBalance();
    bool isCoordinateOccupied(int f, int s, int r, int c);
    bool findEmptyCoordinate(int &f, int &s, int &r, int &c);

public:
    LibrarySystem();
    ~LibrarySystem();

    time_t getSimulatedTime() const;
    std::string formatSimulatedTime(time_t t) const;

    void registerUser(std::string id, std::string name);
    void removeUser(std::string id);
    void buyOrRenewLibraryCard(std::string userId);

    void addBook(int id, std::string title, std::string author);
    void removeBook(int id);
    
    void issueBook(std::string userId, int bookId);
    void returnBook(std::string userId, int bookId);

    void locateBookByTitle(std::string title);
    void buyBookFromFactory(int newId, std::string title, std::string author);

    void displayInventory();
    void displayUsers();
    void displayBankBalance();
};

#endif