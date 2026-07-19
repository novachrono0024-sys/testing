#include "Book.hpp"

Book::Book() : id(0), title(""), author(""), isIssued(false), borrowedByUserId("NONE"), 
               floor(1), shelf(1), row(1), col(1) {}

Book::Book(int id, std::string title, std::string author, bool isIssued, 
           std::string borrowedBy, int f, int s, int r, int c) {
    this->id = id;
    this->title = title;
    this->author = author;
    this->isIssued = isIssued;
    this->borrowedByUserId = borrowedBy;
    this->floor = f;
    this->shelf = s;
    this->row = r;
    this->col = c;
}