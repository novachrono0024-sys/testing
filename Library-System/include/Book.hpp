#ifndef BOOK_HPP
#define BOOK_HPP

#include <string>

class Book {
public:
    int id;
    std::string title;
    std::string author;
    bool isIssued;
    std::string borrowedByUserId;
    
    // Spatial Coordinates
    int floor;
    int shelf;
    int row;
    int col;

    Book();
    Book(int id, std::string title, std::string author, bool isIssued = false, 
         std::string borrowedBy = "NONE", int f = 1, int s = 1, int r = 1, int c = 1);
};

#endif