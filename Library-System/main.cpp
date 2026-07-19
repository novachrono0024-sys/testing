#include <iostream>
#include "LibrarySystem.hpp"

using namespace std;

int main() {
    LibrarySystem sys;
    int choice;

    while (true) {
        cout << "\n=============================================\n";
        cout << "  PUBLIC LOGISTICS LIBRARY & SUBSCRIPTION SYSTEM\n";
        cout << "  Simulated Current Date: " << sys.formatSimulatedTime(sys.getSimulatedTime()) << "\n";
        cout << "=============================================\n";
        cout << "1.  Front Desk: Locate Shelf Coordinates (By Title)\n";
        cout << "2.  Patron: Register New Citizen Account\n";
        cout << "3.  Patron: Buy / Renew 3-Month Library Card ($15)\n";
        cout << "4.  Patron: Cancel / Remove Citizen Account\n";
        cout << "5.  Circulation: Rent / Issue Book Out\n";
        cout << "6.  Circulation: Return Book (Process Fines)\n";
        cout << "7.  Procurement: Buy Out-of-Stock Book from Factory ($300)\n";
        cout << "8.  Inventory: Add Book Manually to Grid Layout\n";
        cout << "9.  Inventory: Delete Book Trace from Shelves\n";
        cout << "10. View System Matrix (Full Inventory & Patrons)\n";
        cout << "11. Treasury: View Bank Ledger Balance\n";
        cout << "12. Exit & Save Database States\n";
        cout << "Enter Choice (1-12): ";
        
        if (!(cin >> choice)) {
            cout << "Invalid numeric input structure. Forcing a safe shutdown sequence...\n";
            break;
        }

        if (choice == 1) {
            string t; cout << "Enter Book Title: "; cin.ignore(); getline(cin, t);
            sys.locateBookByTitle(t);
        }
        else if (choice == 2) {
            string uid, name;
            cout << "Enter New Patron User ID: "; cin >> uid; cin.ignore();
            cout << "Enter Patron Name: "; getline(cin, name);
            sys.registerUser(uid, name);
        }
        else if (choice == 3) {
            string uid; cout << "Enter Patron User ID for Card processing: "; cin >> uid;
            sys.buyOrRenewLibraryCard(uid);
        }
        else if (choice == 4) {
            string uid; cout << "Enter Patron User ID to purge: "; cin >> uid;
            sys.removeUser(uid);
        }
        else if (choice == 5) {
            string uid; int bid;
            cout << "Enter Patron User ID: "; cin >> uid;
            cout << "Enter Target Book ID: "; cin >> bid;
            sys.issueBook(uid, bid);
        }
        else if (choice == 6) {
            string uid; int bid;
            cout << "Enter Patron User ID: "; cin >> uid;
            cout << "Enter Returning Book ID: "; cin >> bid;
            sys.returnBook(uid, bid);
        }
        else if (choice == 7) {
            int id; string t, a;
            cout << "Enter New Catalog ID to assign: "; cin >> id; cin.ignore();
            cout << "Enter Title: "; getline(cin, t);
            cout << "Enter Author: "; getline(cin, a);
            sys.buyBookFromFactory(id, t, a);
        }
        else if (choice == 8) {
            int id; string t, a;
            cout << "Enter Manual Book ID: "; cin >> id; cin.ignore();
            cout << "Enter Title: "; getline(cin, t);
            cout << "Enter Author: "; getline(cin, a);
            sys.addBook(id, t, a);
        }
        else if (choice == 9) {
            int bid; cout << "Enter Book ID to destroy: "; cin >> bid;
            sys.removeBook(bid);
        }
        else if (choice == 10) {
            sys.displayInventory();
            sys.displayUsers();
        }
        else if (choice == 11) {
            sys.displayBankBalance();
        }
        else if (choice == 12) {
            cout << "Database sync completed successfully. Shutting down system engines...\n";
            break;
        }
        else {
            cout << "Invalid Option Selection.\n";
        }
    }
    return 0;
}