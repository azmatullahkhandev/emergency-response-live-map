#include<iostream>   // For input/output (cout, cin)
#include<string>     // For string
#include<cstdlib>    // For rand, srand, system, max (needed by <algorithm>)
#include<ctime>      // For time, time_t, strftime, localtime
#include<iomanip>    // For setw, left, right, setfill
#include<algorithm>  // For transform
#include<windows.h>  // For Windows specific console functions (colors, sleep)
#include<conio.h>    // For _getch()
#include<stdexcept>  // For runtime_error
#include<limits>     // For numeric_limits
#include<utility>    // For pair
#include<locale>     // For tolower with transform

using namespace std;

// Color Control

#define CC_RESET 7
#define CC_BOLD 15
#define CC_RED 12
#define CC_GREEN 10
#define CC_CYAN 11
#define CC_YELLOW 14
#define CC_MAGENTA 13
#define CC_WHITE 15

// Sets console text color (Windows specific)
void set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Custom Linked List Node
template <typename T>
struct Node {
    T data;
    Node* next;
    Node(T val) : data(val), next(nullptr) {}
};

// Linked List Class
template <typename T>
class LinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    int count;
    void deleteAllNodes() {
        Node<T>* current = head;
        while (current) {
            Node<T>* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }
public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~LinkedList() { deleteAllNodes(); }
    void add(T data) {
        Node<T>* newNode = new Node<T>(data);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }
    void addToFront(T data) {
        Node<T>* newNode = new Node<T>(data);
        newNode->next = head;
        head = newNode;
        if (!tail) tail = head;
        count++;
    }
    bool removeByValue(const T& dataToRemove) {
        if (!head) return false;
        if (head->data == dataToRemove) {
            return removeFront();
        }
        Node<T>* current = head;
        while (current->next && !(current->next->data == dataToRemove)) {
            current = current->next;
        }
        if (current->next) {
            Node<T>* nodeToDelete = current->next;
            if (nodeToDelete == tail) tail = current;
            current->next = nodeToDelete->next;
            delete nodeToDelete;
            count--;
            return true;
        }
        return false;
    }
    bool removeLast() {
        if (!head) return false;
        if (head == tail) {
            delete head;
            head = tail = nullptr;
        } else {
            Node<T>* current = head;
            while (current->next != tail)
                current = current->next;
            delete tail;
            tail = current;
            tail->next = nullptr;
        }
        count--;
        return true;
    }
    bool removeFront() {
        if (!head) return false;
        Node<T>* nodeToDelete = head;
        head = head->next;
        if (!head) tail = nullptr;
        delete nodeToDelete;
        count--;
        return true;
    }
    void clear() { deleteAllNodes(); }
    Node<T>* getHead() const {
        return head; }
    Node<T>* getTail() const {
        return tail; }
    bool isEmpty() const {
        return head == nullptr; }
    int getSize() const {
        return count; }
    T front() const {
        if (!head) throw runtime_error("Cannot get front() from empty list");
        return head->data;
    }
    Node<T>* getNodeAt(int index) const {
        if (index < 0 || index >= count) return nullptr;
        Node<T>* current = head;
        for (int i = 0; i < index; ++i) {
            if (!current) return nullptr;
            current = current->next;
        }
        return current;
    }
};

// Custom Queue (FIFO)
template <typename T>
class Queue {
private:
    LinkedList<T> list;
public:
    void enqueue(T data) { list.add(data); }
    T dequeue() {
        if (list.isEmpty()) throw runtime_error("Queue is empty");
        T data = list.front();
        list.removeFront();
        return data;
    }
    T peek() const { return list.front(); }
    bool isEmpty() const {
        return list.isEmpty(); }
    int getSize() const {
        return list.getSize(); }
};

// Custom Stack (LIFO)
template <typename T>
class Stack {
private:
    LinkedList<T> list;
public:
    void push(T data) { list.addToFront(data); }
    T pop() {
        if (list.isEmpty()) throw runtime_error("Stack is empty");
        T data = list.front();
        list.removeFront();
        return data;
    }
    T peek() const { return list.front(); }
    bool isEmpty() const { return list.isEmpty(); }
    int getSize() const { return list.getSize(); }
};

// Core Data Structure Definitions
struct Emergency {
    int id;
    string type;
    string at;
    bool handled;
    Emergency(int _id, string _type, string _at)
        : id(_id), type(_type), at(_at), handled(false) {}
    bool operator==(const Emergency& other) const { return id == other.id; }
};
struct Unit {
    int id;
    string type;
    string at;
    bool available;
    bool operator==(const Unit& other) const {
        return id == other.id; }
};
struct Location {
    string name;
    LinkedList<string> neighbors;
    int gridRow, gridCol;
    LinkedList<Unit*> unitsHere;
    LinkedList<Emergency*> emergenciesHere;
    bool operator==(const Location& other) const {
        return name == other.name; }
};
struct EmergencyLog {
    int emergId;
    string type, loc, unitType;
    int unitId;
    time_t handledAt;
    bool operator==(const EmergencyLog& other) const {
        return emergId == other.emergId && handledAt == other.handledAt;
    }
};

// CityMap Class
class CityMap {
public:
    LinkedList<Location> locations;
    void addLocation(const string& name, int row, int col) {
        locations.add(Location{ name, LinkedList<string>(), row, col, LinkedList<Unit*>(), LinkedList<Emergency*>() });
    }
    Location* findLocation(const string& name) {
        Node<Location>* current = locations.getHead();
        while(current) {
            if (current->data.name == name) return &(current->data);
            current = current->next;
        }
        return nullptr;
    }
    void connect(const string& a, const string& b) {
        Location* locA = findLocation(a);
        Location* locB = findLocation(b);
        if (locA && locB) {
            locA->neighbors.add(b);
            locB->neighbors.add(a);
        } else {
            cerr << "Error connecting locations: Not found (" << a << " or " << b << ")" << endl;
        }
    }
    Location* get(const string& name) {
        return findLocation(name);
    }
    string findByPos(int r, int c) const {
        Node<Location>* current = locations.getHead();
        while(current) {
            if (current->data.gridRow == r && current->data.gridCol == c)
                return current->data.name;
            current = current->next;
        }
        return "";
    }
    int maxRow() const {
        int mx = 0;
        Node<Location>* current = locations.getHead();
        while(current) {
                mx = max(mx, current->data.gridRow); current = current->next; }
        return mx;
    }
    int maxCol() const {
        int mx = 0;
        Node<Location>* current = locations.getHead();
        while(current) {
                mx = max(mx, current->data.gridCol); current = current->next; }
        return mx;
    }
};

// Main Simulation Class
class DRSS_Sim {
    CityMap city;
    LinkedList<Unit> units;
    LinkedList<Emergency> emergencies;
    LinkedList<EmergencyLog> logs;
    Queue<Emergency*> emergQueue;
    LinkedList<int> resolvedEmergIDs;
    Stack<pair<EmergencyLog, Emergency*>> undoStack;
    int nextEmerg = 1001;
    int nextUnit = 1;
    int ticks = 0;
    int loginLimit = 3;
public:
    DRSS_Sim() { srand((unsigned)time(0)); }
    void run() {
        if (!login()) return;
        seedMap();
        seedUnits();
        bool running = true;
        while (running) {
            clearScreen();
            placeUnitsEmergenciesOnMap();
            showMapLive();
            showStatus();
            if (ticks > 0 && (ticks % (3 + rand() % 3) == 0)) {
                spawnEmergency();
                set_color(CC_RED | CC_BOLD);
                cout << "\n*** New Emergency has occurred! ***" << endl;
                set_color(CC_RESET);
                Sleep(1200);
            }
            ticks++;
            displayMenu();
            running = handleMenuInput();
        }
        set_color(CC_RESET);
        cout << "Simulation ended. Stay safe!" << endl;
    }
private:
    void clearScreen() { system("cls"); }
    bool login() {
        string username, password;
        int attempts = 0;
        while(attempts < loginLimit){
            clearScreen();
            set_color(CC_CYAN);
            cout << R"(
    +===================================+
    | Welcome to Peshawar Emergency SIM |
    +===================================+)" << endl;
            set_color(CC_RESET);
            cout << "\nAdmin Login Required (" << loginLimit - attempts << " tries left)" << endl;
            cout << "Username: ";
            getline(cin, username);
            cout << "Password: ";
            char ch;
            password = "";
            while ((ch = _getch()) != '\r') {
                if (ch == '\b') {
                    if (!password.empty()) {
                        password.pop_back();
                        cout << "\b \b";
                    }
                } else if (ch >= 32 && ch <= 126) {
                    password += ch;
                    cout << '*';
                }
            }
            cout << endl;
            if(username == "admin" && password == "admin123") {
                set_color(CC_GREEN);
                cout << "Access Granted. Loading Simulation..." << endl;
                set_color(CC_RESET);
                Sleep(1000);
                return true;
            }
            else {
                set_color(CC_RED);
                cout << "Invalid credentials." << endl;
                set_color(CC_RESET);
                attempts++;
                Sleep(1000);
            }
        }
        set_color(CC_RED|CC_BOLD);
        cout << "Access Denied. Too many failed attempts. Shutting down..." << endl;
        set_color(CC_RESET);
        Sleep(2000);
        return false;
    }
    void seedMap() {
        city.addLocation("Saddar", 3, 4); city.addLocation("Hayatabad", 1, 3);
        city.addLocation("Karkhano", 0, 2); city.addLocation("Board Bazaar", 1, 5);
        city.addLocation("Tehkal", 0, 6); city.addLocation("Gulbahar", 3, 2);
        city.addLocation("GT Road", 3, 0); city.addLocation("Univ.", 5, 1);
        city.addLocation("Ring Road", 0, 0); city.addLocation("UET", 3, 6);
        city.addLocation("Cantt", 5, 7);
        city.connect("Saddar", "Gulbahar"); city.connect("Saddar", "Hayatabad");
        city.connect("Saddar", "UET"); city.connect("Saddar", "Board Bazaar");
        city.connect("Gulbahar", "GT Road"); city.connect("GT Road", "Univ.");
        city.connect("Hayatabad", "Karkhano"); city.connect("Board Bazaar", "Tehkal");
        city.connect("Univ.", "Ring Road"); city.connect("UET", "Cantt");
        city.connect("Board Bazaar", "Ring Road"); city.connect("Tehkal", "Cantt");
        city.connect("Board Bazaar", "Karkhano"); city.connect("Hayatabad", "Board Bazaar");
        city.connect("UET", "Tehkal");
    }
    void seedUnits() {
        units.add(Unit{ nextUnit++, "FireTruck", "Saddar", true });
        units.add(Unit{ nextUnit++, "FireTruck", "Hayatabad", true });
        units.add(Unit{ nextUnit++, "FireTruck", "Tehkal", true });
        units.add(Unit{ nextUnit++, "Ambulance", "GT Road", true });
        units.add(Unit{ nextUnit++, "Ambulance", "Univ.", true });
        units.add(Unit{ nextUnit++, "Ambulance", "Cantt", true });
        units.add(Unit{ nextUnit++, "Police", "Saddar", true });
        units.add(Unit{ nextUnit++, "Police", "Board Bazaar", true });
        units.add(Unit{ nextUnit++, "Police", "Ring Road", true });
    }
    void placeUnitsEmergenciesOnMap() {
        Node<Location>* locNode = city.locations.getHead();
        while (locNode) {
            locNode->data.unitsHere.clear();
            locNode->data.emergenciesHere.clear();
            locNode = locNode->next;
        }
        Node<Unit>* unitNode = units.getHead();
        while(unitNode) {
            Location* loc = city.get(unitNode->data.at);
            if (loc) {
                loc->unitsHere.add(&(unitNode->data));
            } else {
                cerr << "Error: Unit #" << unitNode->data.id << " loc '" << unitNode->data.at << "' not found\n";
            }
            unitNode = unitNode->next;
        }
        Node<Emergency>* emergNode = emergencies.getHead();
        while (emergNode) {
            if (!emergNode->data.handled) {
                Location* loc = city.get(emergNode->data.at);
                if (loc) {
                    loc->emergenciesHere.add(&(emergNode->data));
                } else {
                    cerr << "Error: Emerg #" << emergNode->data.id << " loc '" << emergNode->data.at << "' not found\n";
                }
            }
            emergNode = emergNode->next;
        }
    }
    void showMapLive() {
        const int ROWS = city.maxRow() + 1;
        const int COLS = city.maxCol() + 1;
        set_color(CC_BOLD | CC_WHITE);
        cout << "       ==== PESHAWAR CITY EMERGENCY MAP ====" << endl;
        set_color(CC_RESET);
        cout << setfill(' ');
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                string name = city.findByPos(r, c);
                if (name.empty()) { cout << setw(15) << " "; continue; }
                Location* loc = city.get(name);
                if (!loc) { cout << setw(15) << " ERR "; continue; }
                int u_count = loc->unitsHere.getSize();
                int e_count = loc->emergenciesHere.getSize();
                string marker = "  "; int color = CC_WHITE;
                if (e_count > 0 && u_count > 0) { color = CC_YELLOW | CC_BOLD; marker = "U!"; }
                else if (e_count > 0) { color = CC_RED | CC_BOLD; marker = " !"; }
                else if (u_count > 0) { color = CC_CYAN | CC_BOLD; marker = " U"; }
                set_color(color);
                cout << left << setw(12) << name.substr(0, 11) << marker;
                set_color(CC_RESET);
            }
            cout << endl;
        }
        set_color(CC_CYAN); cout << "U=Unit ";
        set_color(CC_RED); cout << "!=Emergency ";
        set_color(CC_YELLOW | CC_BOLD); cout << "U!=Both";
        set_color(CC_RESET); cout << endl;
    }
    void showStatus() {
        int activeE = 0, availFire = 0, availAmb = 0, availPol = 0;
        int totalFire=0, totalAmb=0, totalPol=0;
        Node<Emergency>* eNode = emergencies.getHead();
        while(eNode) {
                if (!eNode->data.handled) activeE++; eNode = eNode->next; }
        Node<Unit>* uNode = units.getHead();
        while(uNode) {
            Unit& u = uNode->data;
            if (u.type == "FireTruck") {
                totalFire++; if (u.available) availFire++; }
            else if (u.type == "Ambulance") {
                totalAmb++; if (u.available) availAmb++; }
            else if (u.type == "Police") {
                totalPol++; if (u.available) availPol++; }
            uNode = uNode->next;
        }
        set_color(CC_BOLD | CC_RED);
        cout << "\nActive Emergencies: " << activeE; set_color(CC_RESET); cout << "   |   ";
        set_color(CC_BOLD | CC_WHITE);
        cout << "Units Available / Total:" << endl;
        set_color(CC_YELLOW);
        cout << "  FireTrucks: " << availFire << "/" << totalFire;
        set_color(CC_CYAN);
        cout << "    Ambulances: " << availAmb << "/" << totalAmb;
        set_color(CC_MAGENTA);
        cout << "    Police: " << availPol << "/" << totalPol << endl;
        set_color(CC_RESET);
        cout << "Emergencies in Dispatch Queue: ";
        set_color(CC_YELLOW|CC_BOLD);
        cout << emergQueue.getSize() << endl; set_color(CC_RESET);
    }
    void displayMenu() {
        set_color(CC_YELLOW | CC_BOLD);
        cout << "\n[1] Dispatch      [2] Dispatch ALL      [3] Undo         [4] Logs\n";
        cout << "[5] Units         [6] Total Resolved    [7] Pass Time    [8] Quit\n";
        set_color(CC_RESET);
        cout << "> ";
    }
    bool handleMenuInput() {
        string input;
        getline(cin, input);
        if (input.empty()) return true;
        switch (input[0]) {
            case '1': dispatchMenu(); break;
            case '2': dispatchAllMenu(); break;
            case '3': undoDispatch(); break;
            case '4': showLogs(); pauseMsg(); break;
            case '5': showAllUnits(); pauseMsg(); break;
            case '6': showTotalResolved(); pauseMsg(); break;
            case '7': simulateTimePass(); break;
            case '8': return false;
            default:
                set_color(CC_RED); cout << "Invalid choice." << endl; set_color(CC_RESET);
                Sleep(800); break;
        }
        return true;
    }
    void dispatchMenu() {
        placeUnitsEmergenciesOnMap();
        set_color(CC_BOLD);
        cout << "\nAvailable Locations:" << endl; set_color(CC_RESET);
        Node<Location>* node = city.locations.getHead();
        if (!node) {
                cout << "(No locations defined)" << endl; return; }
        while(node) {
            cout << " - " << node->data.name << endl;
            node = node->next;
        }
        cout << "\nEnter location name to dispatch to (or blank to cancel): ";
        string locNameInput;
        getline(cin, locNameInput);
        if (locNameInput.empty()) return;
        string locNameLower = locNameInput;
        transform(locNameLower.begin(), locNameLower.end(), locNameLower.begin(),
                  [](unsigned char c){ return tolower(c); });
        Location* targetLoc = nullptr;
        node = city.locations.getHead();
        while(node) {
            string currentNameLower = node->data.name;
            transform(currentNameLower.begin(), currentNameLower.end(), currentNameLower.begin(),
                  [](unsigned char c){ return tolower(c); });
            if (currentNameLower == locNameLower) {
                targetLoc = &(node->data);
                break;
            }
            node = node->next;
        }
        if (!targetLoc) {
            set_color(CC_RED); cout << "Location '" << locNameInput << "' not found." << endl; set_color(CC_RESET);
            pauseMsg(); return;
        }
        if (targetLoc->emergenciesHere.isEmpty()) {
            set_color(CC_GREEN); cout << "No active emergencies at " << targetLoc->name << "." << endl; set_color(CC_RESET);
            pauseMsg(); return;
        }
        Node<Emergency*>* emergPtrNode = targetLoc->emergenciesHere.getHead();
        bool dispatched = false;
        while(emergPtrNode && !dispatched) {
            Emergency* pe = emergPtrNode->data;
            if(pe->handled) { emergPtrNode = emergPtrNode->next; continue; }
            int eColor = (pe->type == "Fire") ? CC_YELLOW : ((pe->type == "Medical") ? CC_CYAN : CC_MAGENTA);
            set_color(CC_BOLD); cout << "\nAt " << targetLoc->name << ":" << endl; set_color(eColor | CC_BOLD);
            cout << "Emergency #" << pe->id << " [" << pe->type << "]" << endl; set_color(CC_RESET);
            string expectedUnitType = (pe->type == "Fire") ? "FireTruck" : ((pe->type == "Medical") ? "Ambulance" : "Police");
            cout << "Dispatch available " << expectedUnitType << "? (Y/N): ";
            string yn; getline(cin, yn);
            if (yn.empty() || tolower(yn[0]) != 'y') {
                emergPtrNode = emergPtrNode->next; continue;
            }
            Unit* availableUnitPtr = nullptr;
            Node<Unit>* unitNode = units.getHead();
            while(unitNode) {
                if (unitNode->data.type == expectedUnitType && unitNode->data.available) {
                    availableUnitPtr = &(unitNode->data); break;
                }
                unitNode = unitNode->next;
            }
            if (!availableUnitPtr) {
                set_color(CC_RED); cout << "No " << expectedUnitType << " available." << endl; set_color(CC_RESET);
                pauseMsg(); return;
            }
            resolveEmergency(pe, availableUnitPtr);
            dispatched = true;
            pauseMsg();
            emergPtrNode = emergPtrNode->next;
        }
        if(!dispatched && !targetLoc->emergenciesHere.isEmpty()){
            cout << "No dispatch performed for emergencies at " << targetLoc->name << "." << endl;
            pauseMsg();
        }
    }
    void dispatchAllMenu() {
        emergQueue = Queue<Emergency*>();
        int enqueuedCount = 0;
        Node<Emergency>* eNode = emergencies.getHead();
        while(eNode) {
            if (!eNode->data.handled) {
                emergQueue.enqueue(&(eNode->data)); enqueuedCount++;
            }
            eNode = eNode->next;
        }
        if (emergQueue.isEmpty()) {
            set_color(CC_GREEN); cout << "No unresolved emergencies." << endl; set_color(CC_RESET);
            pauseMsg(); return;
        }
        set_color(CC_YELLOW);
        cout << "\nAttempting auto-dispatch for " << enqueuedCount << " emergencies (FIFO)..." << endl;
        set_color(CC_RESET);
        Sleep(1000);
        int processedCount = 0, skippedCount = 0;
        while (!emergQueue.isEmpty()) {
            Emergency* pe = emergQueue.dequeue();
            processedCount++;
            string expectedUnitType = (pe->type == "Fire") ? "FireTruck" : ((pe->type == "Medical") ? "Ambulance" : "Police");
            set_color(CC_YELLOW | CC_BOLD);
            cout << "\nProcessing #" << processedCount << ": Emerg ID " << pe->id << " (" << pe->type << ") at " << pe->at << "..." << endl;
            set_color(CC_RESET);
            Unit* availableUnitPtr = nullptr;
            Node<Unit>* unitNode = units.getHead();
            while(unitNode) {
                if (unitNode->data.type == expectedUnitType && unitNode->data.available) {
                    availableUnitPtr = &(unitNode->data); break;
                }
                unitNode = unitNode->next;
            }
            if (!availableUnitPtr) {
                set_color(CC_RED);
                cout << "No " << expectedUnitType << " available! Skipping..." << endl; set_color(CC_RESET);
                skippedCount++; Sleep(200); continue;
            }
            cout << "Auto-dispatching " << availableUnitPtr->type << " #" << availableUnitPtr->id << "..." << endl;
            resolveEmergency(pe, availableUnitPtr);
            Sleep(500);
        }
        cout << "\nFinished queue processing." << endl;
        if (skippedCount > 0) { set_color(CC_YELLOW);
        cout << skippedCount << " skipped (no units)." << endl; set_color(CC_RESET); }
        else { set_color(CC_GREEN);
        cout << "All queued emergencies processed or attempted." << endl; set_color(CC_RESET); }
        pauseMsg();
    }
    void resolveEmergency(Emergency* emergency, Unit* unit) {
        if (!emergency || !unit) return;
        string targetLocation = emergency->at;
        set_color(CC_WHITE | CC_BOLD);
        cout << unit->type << " #" << unit->id << " dispatched from " << unit->at << " to " << targetLocation << endl;
        set_color(CC_RESET);
        sleepWithMsg(rand() % 2 + 1, "Travelling...");
        unit->at = targetLocation; unit->available = false;
        cout << "Arrived at " << targetLocation << "." << endl;
        sleepWithMsg(rand() % 2 + 1, "Resolving...");
        emergency->handled = true; unit->available = true;
        EmergencyLog logEntry = { emergency->id, emergency->type, emergency->at, unit->type, unit->id, time(0) };
        logs.add(logEntry);
        undoStack.push({logEntry, emergency});
        resolvedEmergIDs.removeByValue(emergency->id);
        resolvedEmergIDs.add(emergency->id);
        set_color(CC_GREEN | CC_BOLD);
        cout << "*** Emerg #" << emergency->id << " resolved by Unit #" << unit->id << "! ***" << endl; set_color(CC_RESET);
    }
    void undoDispatch() {
        if (undoStack.isEmpty()) {
            set_color(CC_RED); cout << "No actions to undo." << endl; set_color(CC_RESET); pauseMsg(); return;
        }
        pair<EmergencyLog, Emergency*> lastAction = undoStack.pop();
        EmergencyLog log = lastAction.first;
        Emergency* emerg = lastAction.second;
        if (emerg) {
            emerg->handled = false;
            set_color(CC_YELLOW | CC_BOLD);
            cout << "Undo: Emerg #" << emerg->id << " [" << emerg->type << "] at " << emerg->at << " reactivated!" << endl; set_color(CC_RESET);
            resolvedEmergIDs.removeByValue(log.emergId);
        } else {
            set_color(CC_RED); cout << "Undo Error: Original emergency pointer invalid?" << endl; set_color(CC_RESET);
            resolvedEmergIDs.removeByValue(log.emergId);
        }
        logs.removeLast();
        pauseMsg();
    }
    void spawnEmergency() {
        static const string types[] = { "Fire", "Medical", "Crime" };
        int locCount = city.locations.getSize();
        if (locCount == 0) return;
        int randomIndex = rand() % locCount;
        Node<Location>* locNode = city.locations.getNodeAt(randomIndex);
        if (!locNode) {
            cerr << "Error: Could not find location node at index " << randomIndex << " to spawn emergency." << endl;
            return;
        }
        string randomLoc = locNode->data.name;
        string randomType = types[rand() % 3];
        emergencies.add(Emergency{ nextEmerg++, randomType, randomLoc });
    }
    void simulateTimePass() {
        set_color(CC_CYAN); cout << "\nSimulating time passing..." << endl; set_color(CC_RESET);
        Sleep(700);
        if (rand() % 2 == 0) {
            spawnEmergency();
            set_color(CC_RED | CC_BOLD); cout << "*** New Emergency occurred! ***" << endl; set_color(CC_RESET);
            Sleep(1200);
        } else {
            cout << "No new major events." << endl; Sleep(500);
        }
    }
    void showLogs() {
        set_color(CC_BOLD); cout << "\n--- Emergency History (Oldest First, Max 15) ---" << endl; set_color(CC_RESET);
        if (logs.isEmpty()) {
            cout << "(No emergencies handled yet)" << endl;
        } else {
            int shown = 0;
            Node<EmergencyLog>* logNode = logs.getHead();
            char time_buf[80];
            while(logNode != nullptr && shown < 15) {
                EmergencyLog& l = logNode->data;
                int color = (l.type == "Fire") ? CC_YELLOW : ((l.type=="Medical")?CC_CYAN:CC_MAGENTA);
                set_color(color);
                tm timeinfo;
                localtime_s(&timeinfo, &l.handledAt);
                strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
                cout << "#" << setw(4) << left << l.emergId
                     << " [" << setw(7) << left << l.type << "]"
                     << " at " << setw(12) << left << l.loc
                     << " | Unit " << setw(9) << left << l.unitType << " #" << setw(2) << left << l.unitId
                     << " | " << time_buf;
                set_color(CC_RESET); cout << endl;
                logNode = logNode->next; shown++;
            }
            if (logs.getSize() > shown) { cout << "... (+" << (logs.getSize() - shown) << " older logs not shown)" << endl; }
        }
        set_color(CC_BOLD); cout << "----------------------------------------------------" << endl; set_color(CC_RESET);
    }
    void showAllUnits() {
        set_color(CC_BOLD); cout << "\n---------- Units Status ----------" << endl;
        cout << "ID | Type        | Location     | Status" << endl;
        cout << "---+-------------+--------------+----------" << endl; set_color(CC_RESET);
        if (units.isEmpty()) { cout << "       (No units deployed)" << endl; }
        else {
            Node<Unit>* uNode = units.getHead();
            while(uNode) {
                Unit& u = uNode->data;
                int color = (u.type == "FireTruck") ? CC_YELLOW : ((u.type == "Ambulance") ? CC_CYAN : CC_MAGENTA);
                set_color(color);
                cout << setw(2) << right << u.id << " | " << setw(11) << left << u.type << " | "
                     << setw(12) << left << u.at << " | ";
                set_color(u.available ? (CC_GREEN | CC_BOLD) : (CC_RED | CC_BOLD));
                cout << (u.available ? "Ready" : "On Mission") << endl;
                set_color(CC_RESET); uNode = uNode->next;
            }
        }
        set_color(CC_BOLD); cout << "----------------------------------" << endl; set_color(CC_RESET);
    }
    void showTotalResolved() {
        set_color(CC_WHITE|CC_BOLD); cout << "\nTotal unique emergencies resolved: ";
        set_color(CC_GREEN|CC_BOLD); cout << resolvedEmergIDs.getSize() << endl; set_color(CC_RESET);
    }
    void pauseMsg() {
        set_color(CC_CYAN); cout << "\n<Press ENTER to continue>"; set_color(CC_RESET);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        _getch();
        cout << endl;
    }
    void sleepWithMsg(int sec, const string& msg) {
        cout << msg << " " << flush;
        for (int t = 0; t < sec; ++t) {
            Sleep(400); cout << "." << flush; Sleep(600);
        }
        cout << "\r" << string(msg.length() + sec + 7, ' ') << "\r" << flush;
    }
};

// Main Function
int main() {
    DRSS_Sim sim;
    sim.run();
    return 0;
}
