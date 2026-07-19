#include "User.hpp"

User::User(std::string id, std::string name, bool hasCard, time_t expiry) 
    : userId(id), name(name), hasLibraryCard(hasCard), cardExpiryDate(expiry) {}

std::string User::getUserId() const { return userId; }
std::string User::getName() const { return name; }
bool User::getHasLibraryCard() const { return hasLibraryCard; }
time_t User::getCardExpiryDate() const { return cardExpiryDate; }

void User::purchaseOrRenewCard(time_t currentSimulatedTime) {
    hasLibraryCard = true;
    // 3 Months = 90 days. In our dilated engine, 90 days = 90 seconds.
    cardExpiryDate = currentSimulatedTime + (90 * 86400); 
}

bool User::checkAndApplyExpiry(time_t currentSimulatedTime) {
    if (hasLibraryCard && currentSimulatedTime > cardExpiryDate) {
        hasLibraryCard = false;
        cardExpiryDate = 0;
        return true; // Card just expired
    }
    return false;
}