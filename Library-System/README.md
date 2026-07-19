# 🏛️ Time-Aware Public Library & Logistics Engine

![C++](https://img.shields.io/badge/C++-11/14/17-blue.svg)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)

A comprehensive, command-line public library logistics system built in C++. This project goes beyond standard CRUD operations by implementing **spatial inventory tracking**, a **dynamic financial ledger**, and a custom **Time-Dilation Simulation Engine** to test long-term subscription and fine-accrual logic in real-time.

## ✨ Key Features

* **⏱️ Time-Dilation Engine:** Built a custom simulation clock where **1 real-world second = 1 simulated day**. This allows for the immediate QA testing of 90-day library card expirations and daily overdue fines without needing to manipulate system clocks or wait real-world time.
* **📦 Spatial Grid Inventory:** Books aren't just stored in a list; they are allocated physical space. The system searches a 3D building matrix (`Floor -> Shelf -> Row -> Column`) to prevent physical collisions when new stock is purchased from the factory.
* **💰 Dynamic Financial Ledger:** The library operates as a business. It maintains a persistent bank balance used to procure factory stock ($300/book), processes subscription renewals ($15/card), and auto-calculates variable overdue fines based on the user's membership tier.
* **💾 Persistent State Management:** Utilizes standard file streams (`std::ifstream`/`std::ofstream`) to seamlessly save and load user structures, complex transaction histories, and the simulated epoch anchor, ensuring the database survives system reboots.

## 🛠️ Technical Stack
* **Language:** C++ (Object-Oriented Programming)
* **Data Structures:** `std::unordered_map`, `std::vector` (achieving O(1) lookups for active users and inventory traces).
* **Libraries Used:** `<ctime>` (Epoch parsing), `<fstream>` (Database serialization), `<sstream>` (Delimited token parsing).

---

## 🚀 Installation & Compilation

To run this simulation on your local machine, ensure you have a C++ compiler (like GCC/G++) installed.

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/YOUR-USERNAME/Library-Simulation-Engine.git](https://github.com/YOUR-USERNAME/Library-Simulation-Engine.git)
   cd Library-Simulation-Engine

```

2. **Compile the source code:**
```bash
g++ src/*.cpp -I include -o library_system

```


3. **Run the executable:**
* **Windows:** `.\library_system.exe`
* **Mac/Linux:** `./library_system`



---

## 🖥️ System Dashboard Overview

Upon booting the engine, the user is presented with a real-time dashboard linking to 12 core subsystems:

1. **Front Desk:** Locate shelf coordinates of a book via Title search.
2. **Patron Services:** Register/Remove local user accounts.
3. **Subscription Terminal:** Purchase or renew 3-Month Library Cards.
4. **Circulation Desk:** Issue books, process returns, and calculate simulated late fines.
5. **Procurement:** Spend library bank funds to buy newly printed books from the factory.
6. **Inventory Matrix:** View a total top-down map of all shelves and currently rented out stock.
7. **Treasury:** Monitor the centralized library bank balance.

---

## 🧠 Architectural Highlight: The Simulation Clock

Instead of saving the exact expiration dates of every item, the system permanently anchors the library's "Creation Date" to `system_clock.txt`.

When the system boots, it calculates the difference between the absolute real-world time and the anchor time. It multiplies the total seconds elapsed by `86,400` (the number of seconds in a real day), effectively transforming the program into a time machine where months of library activity can be simulated and tested in a matter of minutes.

```

### 💡 Don't forget:
Before you push this, remember to change `YOUR-USERNAME` in the **Installation** section to your actual GitHub username!

```