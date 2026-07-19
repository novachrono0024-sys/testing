#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <ctime>

class User {
private:
    std::string userId;
    std::string name;
    bool hasLibraryCard;
    time_t cardExpiryDate; // Stored as a Unix timestamp

public:
    User(std::string id, std::string name, bool hasCard = false, time_t expiry = 0);

    std::string getUserId() const;
    std::string getName() const;
    bool getHasLibraryCard() const;
    time_t getCardExpiryDate() const;

    void purchaseOrRenewCard(time_t currentSimulatedTime);
    bool checkAndApplyExpiry(time_t currentSimulatedTime);
};

#endif