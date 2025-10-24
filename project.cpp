#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <sstream>
#include <limits>
#include <fstream>
#include <cstdlib>

using namespace std;

// --- Constants and Global Settings ---
const double TICKET_PRICE_STANDARD = 250.00;
const double TICKET_PRICE_PREMIUM = 450.00;
const string APP_NAME = "CineSphere Booking Console";
const string LINE_SEPARATOR = string(70, '-');
const string BOOKING_DATA_FILE = "bookings.txt"; // File for persistence

// --- Helper Functions for Formatting and User Input ---

/**
 * @brief Prints a formatted header for application sections.
 * @param title The title of the section.
 */
void printHeader(const string& title) {
    cout << "\n" << string(10, '=') << " " << title << " " << string(10, '=') << endl;
}

/**
 * @brief Handles integer input from the user with validation.
 * @param prompt The message to display to the user.
 * @return The validated integer input.
 */
int getValidatedIntInput(const string& prompt) {
    int input;
    while (true) {
        cout << prompt;
        if (cin >> input) {
            // Check if there's any garbage left in the buffer (e.g., "12a")
            if (cin.peek() == '\n') {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return input;
            }
        }
        // If input failed or garbage was found
        cout << "Invalid input. Please enter a valid number." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

/**
 * @brief Formats currency for display.
 * @param amount The monetary value.
 * @return A string representation of the currency.
 */
string formatCurrency(double amount) {
    stringstream ss;
    ss << fixed << setprecision(2) << amount;
    return ss.str();
}

/**
 * @brief Clears the console screen.
 */
void clearScreen() {
    // Platform-independent way to attempt screen clearing
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// --- 1. OOP Core Classes ---

// --- 1.1 MenuItem Class (For Food & Beverages) ---
class MenuItem {
private:
    string name;
    double price;
    string category; // E.g., Popcorn, Beverage, Snack

public:
    // Constructor
    MenuItem(string n, double p, string c) 
        : name(n), price(p), category(c) {}

    // Getters (Encapsulation)
    string getName() const { return name; }
    double getPrice() const { return price; }
    string getCategory() const { return category; }

    /**
     * @brief Displays the menu item details for selection.
     */
    void displayItem(int index) const {
        cout << "  [" << index << "] " << left << setw(30) << name 
             << " - " << left << setw(10) << category 
             << " @ Rs " << formatCurrency(price) << endl;
    }
};


// --- 1.2 Seat Class ---
class Seat {
public:
    enum Status { AVAILABLE, BOOKED, SELECTED };
    enum Type { STANDARD, PREMIUM };

private:
    string seatId; // E.g., "A5", "P2"
    Status status;
    Type type;

public:
    // Constructor
    Seat(string id, Type t) : seatId(id), status(AVAILABLE), type(t) {}

    // Getters
    string getId() const { return seatId; }
    Status getStatus() const { return status; }
    Type getType() const { return type; }
    double getPrice() const { return (type == PREMIUM ? TICKET_PRICE_PREMIUM : TICKET_PRICE_STANDARD); }
    string getStatusString() const {
        if (status == BOOKED) return "BOOKED";
        if (status == SELECTED) return "SELECTED";
        return "AVAILABLE";
    }

    /**
     * @brief Mutator method to change status.
     * @param newStatus The new status to set.
     */
    void setStatus(Status newStatus) {
        status = newStatus;
    }

    /**
     * @brief Displays the seat status visually.
     */
    void displaySeat() const {
        string displayChar = (type == PREMIUM ? "P" : "S");
        if (status == BOOKED) {
            displayChar = "X"; // Booked
        } else if (status == SELECTED) {
            displayChar = "V"; // Selected (V for Voted/Selected)
        }
        cout << "[" << displayChar << setw(4) << right << seatId << "]";
    }
};


// --- 1.3 Movie Class ---
class Movie {
private:
    string title;
    string genre;
    int durationMinutes;

public:
    Movie(string t, string g, int d) : title(t), genre(g), durationMinutes(d) {}

    string getTitle() const { return title; }
    int getDuration() const { return durationMinutes; }
    string getGenre() const { return genre; }

    void displayDetails(int index) const {
        cout << "  [" << index << "] " << left << setw(40) << title 
             << " (" << genre << ", " << durationMinutes << " mins)" << endl;
    }
};


// --- 1.4 Theater Class (Composition: A theater HAS a menu and HAS a seat map) ---
class Theater {
private:
    string name;
    string city;
    string state;
    vector<MenuItem> menu;
    vector<vector<Seat>> seatMap; // 2D vector for seat arrangement

    /**
     * @brief Internal method to initialize the seat map (rows and columns).
     */
    void initializeSeatMap(int standardRows, int premiumRows, int seatsPerRow) {
        char currentRow = 'A';
        // 1. Premium Seats
        for (int r = 0; r < premiumRows; ++r) {
            vector<Seat> row;
            for (int c = 1; c <= seatsPerRow; ++c) {
                string seatId = string(1, currentRow) + to_string(c);
                row.emplace_back(seatId, Seat::PREMIUM);
            }
            seatMap.push_back(row);
            currentRow++;
        }

        // 2. Standard Seats
        for (int r = 0; r < standardRows; ++r) {
            vector<Seat> row;
            for (int c = 1; c <= seatsPerRow; ++c) {
                string seatId = string(1, currentRow) + to_string(c);
                row.emplace_back(seatId, Seat::STANDARD);
            }
            seatMap.push_back(row);
            currentRow++;
        }
    }
    
    /**
     * @brief Internal method to initialize the menu.
     */
    void initializeMenu() {
        menu.emplace_back("Caramel Popcorn (Large)", 350.00, "Popcorn");
        menu.emplace_back("Salty Popcorn (Medium)", 250.00, "Popcorn");
        menu.emplace_back("Coca-Cola (500ml)", 150.00, "Beverage");
        menu.emplace_back("Fresh Lime Soda", 180.00, "Beverage");
        menu.emplace_back("Nachos with Cheese Dip", 290.00, "Snack");
        menu.emplace_back("Veg Burger", 220.00, "Snack");
    }

public:
    // Constructor
    Theater(string n, string c, string s, int stdRows, int premRows, int seatsPer)
        : name(n), city(c), state(s) {
        initializeSeatMap(stdRows, premRows, seatsPer);
        initializeMenu();
    }

    // Getters
    string getName() const { return name; }
    string getCity() const { return city; }
    string getState() const { return state; }
    const vector<MenuItem>& getMenu() const { return menu; }
    vector<vector<Seat>>& getSeatMap() { return seatMap; }
    const vector<vector<Seat>>& getConstSeatMap() const { return seatMap; }

    void displayDetails(int index) const {
        cout << "  [" << index << "] " << name << ", " << city << endl;
    }
};


// --- 1.5 Showtime Class (Association: Showtime has a Movie and a Theater) ---
class Showtime {
private:
    const Movie& movie;     // Reference to the Movie object
    Theater& theater;       // Reference to the Theater object
    string time;            // E.g., "10:00 AM", "06:30 PM"
    string date;            // E.g., "2025-12-15"
    // Unique ID combining Theater, Date, and Time for persistence lookup
    string uniqueShowId; 

    string createUniqueId() const {
        return theater.getName() + "|" + date + "|" + time + "|" + movie.getTitle();
    }

public:
    // Constructor
    Showtime(const Movie& m, Theater& t, string tm, string d)
        : movie(m), theater(t), time(tm), date(d) {
            uniqueShowId = createUniqueId();
        }

    // Getters
    const Movie& getMovie() const { return movie; }
    Theater& getTheater() const { return theater; }
    string getTime() const { return time; }
    string getDate() const { return date; }
    string getUniqueShowId() const { return uniqueShowId; }

    void displayDetails(int index) const {
        cout << "  [" << index << "] ";
        cout << left << setw(10) << time;
        cout << " - " << left << setw(30) << movie.getTitle();
        cout << " (" << movie.getDuration() << " mins) on " << date << endl;
    }
};


// --- 1.6 FoodOrder Class ---
class FoodOrder {
private:
    map<string, pair<int, double>> orderItems; // <ItemName, <Quantity, PricePerItem>>
    double totalFoodPrice = 0.0;

public:
    /**
     * @brief Adds or updates a menu item in the order.
     */
    void addItem(const MenuItem& item, int quantity) {
        if (quantity <= 0) return;

        // If the item already exists in the order
        if (orderItems.count(item.getName())) {
            int currentQty = orderItems[item.getName()].first;
            orderItems[item.getName()].first = currentQty + quantity;
        } else {
            // New item
            orderItems[item.getName()] = {quantity, item.getPrice()};
        }
        totalFoodPrice += item.getPrice() * quantity;
    }

    double getTotalPrice() const {
        return totalFoodPrice;
    }

    bool isEmpty() const {
        return orderItems.empty();
    }

    /**
     * @brief Displays the consolidated food order.
     */
    void displayOrder() const {
        cout << "\n    --- Food Order Details ---" << endl;
        if (isEmpty()) {
            cout << "    (No food items ordered)" << endl;
            return;
        }
        for (const auto& entry : orderItems) {
            const string& name = entry.first;
            int quantity = entry.second.first;
            double pricePerItem = entry.second.second;
            double subtotal = quantity * pricePerItem;

            cout << "    * " << left << setw(30) << name 
                 << " x" << setw(3) << right << quantity 
                 << " @ Rs " << formatCurrency(pricePerItem) 
                 << " = Rs " << formatCurrency(subtotal) << endl;
        }
        cout << "    Total Food Cost: Rs " << formatCurrency(totalFoodPrice) << endl;
    }
};


// --- 1.7 Booking Class (The final transaction record) ---
class Booking {
private:
    static int nextBookingId;
    int bookingId;
    // FIX: Changed reference to pointer to allow vector operations
    const Showtime* showtimePtr; 
    vector<string> bookedSeatIds;
    FoodOrder foodOrder; // FoodOrder is copied into the booking
    double ticketTotal;
    double grandTotal;

    /**
     * @brief Calculates the total price of the tickets based on seat type.
     */
    void calculateTicketTotal(Theater& theater) {
        ticketTotal = 0.0;
        const auto& seatMap = theater.getConstSeatMap();
        for (const string& seatId : bookedSeatIds) {
            // Find the corresponding seat object
            for (const auto& row : seatMap) {
                for (const auto& seat : row) {
                    if (seat.getId() == seatId) {
                        ticketTotal += seat.getPrice();
                        goto seat_found; // Jump out of nested loops once found
                    }
                }
            }
            seat_found:;
        }
        grandTotal = ticketTotal + foodOrder.getTotalPrice();
    }

public:
    // Constructor (Booking) - Accepts reference, stores address
    Booking(const Showtime& s, const vector<string>& seats, const FoodOrder& order)
        : showtimePtr(&s), bookedSeatIds(seats), foodOrder(order) {
        bookingId = nextBookingId++;
        calculateTicketTotal(s.getTheater());
    }
    
    // Constructor for loading from file (minimal data) - Accepts reference, stores address
    Booking(int id, const Showtime& s, const vector<string>& seats)
        : showtimePtr(&s), bookedSeatIds(seats), foodOrder({}) {
        bookingId = id;
        calculateTicketTotal(s.getTheater()); // Recalculate based on seat prices
        // Ensure the static counter is ahead of any loaded ID
        if (id >= nextBookingId) {
            nextBookingId = id + 1;
        }
    }

    // Getters
    int getId() const { return bookingId; }
    // FIX: Returns the dereferenced pointer
    const Showtime& getShowtime() const { return *showtimePtr; } 
    const vector<string>& getBookedSeatIds() const { return bookedSeatIds; }

    /**
     * @brief Generates the final bill and confirmation.
     */
    void generateBill() const {
        printHeader("BOOKING CONFIRMATION & BILL");
        cout << "Reference ID: " << bookingId << endl;
        cout << LINE_SEPARATOR << endl;

        // Movie & Show Details
        cout << left << setw(20) << "Movie:" << getShowtime().getMovie().getTitle() << endl;
        cout << left << setw(20) << "Theater:" << getShowtime().getTheater().getName() 
             << " (" << getShowtime().getTheater().getCity() << ")" << endl;
        cout << left << setw(20) << "Show Time:" << getShowtime().getDate() << " at " << getShowtime().getTime() << endl;
        cout << LINE_SEPARATOR << endl;

        // Ticket Details
        cout << "Ticket Details:" << endl;
        cout << "  Seats Reserved (" << bookedSeatIds.size() << "): ";
        for (size_t i = 0; i < bookedSeatIds.size(); ++i) {
            cout << bookedSeatIds[i] << (i < bookedSeatIds.size() - 1 ? ", " : "");
        }
        cout << endl;
        cout << left << setw(20) << "  Ticket Subtotal:" << "Rs " << formatCurrency(ticketTotal) << endl;
        
        // Food Details
        foodOrder.displayOrder();

        // Grand Total
        cout << LINE_SEPARATOR << endl;
        cout << ">> " << left << setw(20) << "GRAND TOTAL:" << "Rs " << formatCurrency(grandTotal) << endl;
        cout << LINE_SEPARATOR << endl;
        cout << "Enjoy your movie! Seats are confirmed." << endl;
    }

    /**
     * @brief Reverses the seat statuses for cancellation.
     */
    void cancel() const {
        Theater& theater = getShowtime().getTheater();
        auto& seatMap = theater.getSeatMap();

        for (const string& seatId : bookedSeatIds) {
            for (auto& row : seatMap) {
                for (auto& seat : row) {
                    if (seat.getId() == seatId && seat.getStatus() == Seat::BOOKED) {
                        seat.setStatus(Seat::AVAILABLE);
                        break;
                    }
                }
            }
        }
    }

    /**
     * @brief Helper to format booking data for saving to file.
     * Format: BookingID|ShowtimeUniqueID|SeatID1,SeatID2,...
     */
    string toFileString() const {
        string seatList;
        for (size_t i = 0; i < bookedSeatIds.size(); ++i) {
            seatList += bookedSeatIds[i];
            if (i < bookedSeatIds.size() - 1) {
                seatList += ",";
            }
        }
        return to_string(bookingId) + "|" + getShowtime().getUniqueShowId() + "|" + seatList;
    }

    /**
     * @brief Displays brief details for cancellation/lookup.
     */
    void displayBriefDetails() const {
        cout << "  [ID: " << bookingId << "] " 
             << getShowtime().getMovie().getTitle() 
             << " at " << getShowtime().getTime() 
             << " on " << getShowtime().getDate() 
             << " (" << getShowtime().getTheater().getName() << ")" << endl;
        cout << "    Seats: ";
        for (size_t i = 0; i < bookedSeatIds.size(); ++i) {
            cout << bookedSeatIds[i] << (i < bookedSeatIds.size() - 1 ? ", " : "");
        }
        cout << endl;
    }
};
// Initialize static member
int Booking::nextBookingId = 5001;


// --- 2. SystemManager Class (The orchestrator of the entire application) ---
class SystemManager {
private:
    // Core data structures (The 'database' of the application)
    vector<Movie> movies;
    vector<Theater> theaters;
    vector<Showtime> showtimes;
    vector<Booking> allBookings; // New: To store all successful bookings
    
    // Expanded list of states
    vector<string> states = {"Maharashtra", "Karnataka", "Delhi", "Tamil Nadu", "West Bengal", "Gujarat", "Uttar Pradesh"};

    /**
     * @brief Populates initial data for the system.
     */
    void initializeData() {
        // --- Movies (Expanded to 6 movies) ---
        movies.emplace_back("The AI Architect", "Sci-Fi/Action", 145);
        movies.emplace_back("Eternal Sun", "Romantic Drama", 120);
        movies.emplace_back("Rogue Agent 7", "Spy Thriller", 130);
        movies.emplace_back("Jungle Quest", "Family Animation", 95);
        movies.emplace_back("Desert Storm", "War Epic", 160);       
        movies.emplace_back("The Last Voyage", "Mystery", 110);    
        
        // --- Theaters (StandardRows, PremiumRows, SeatsPerRow) ---
        
        // 1. Maharashtra (Mumbai, Pune, Nagpur)
        theaters.emplace_back("PVR Phoenix", "Mumbai", "Maharashtra", 6, 4, 10); // 0
        theaters.emplace_back("Cinepolis Amanora", "Pune", "Maharashtra", 5, 5, 8); // 1
        theaters.emplace_back("INOX Empress", "Nagpur", "Maharashtra", 7, 3, 12); // 2
        
        // 2. Karnataka (Bangalore, Mysore)
        theaters.emplace_back("Gopalan Cinemas", "Bangalore", "Karnataka", 5, 5, 10); // 3
        theaters.emplace_back("PVR Orion Mall", "Mysore", "Karnataka", 4, 6, 9); // 4
        
        // 3. Delhi NCR (New Delhi, Gurugram, Noida)
        theaters.emplace_back("Wave Cinemas", "New Delhi", "Delhi", 5, 5, 10); // 5
        theaters.emplace_back("PVR Ambience", "Gurugram", "Delhi", 6, 4, 11); // 6
        theaters.emplace_back("INOX Mall", "Noida", "Delhi", 7, 3, 9); // 7

        // 4. Tamil Nadu (Chennai, Coimbatore)
        theaters.emplace_back("Jazz Cinemas", "Chennai", "Tamil Nadu", 5, 5, 11); // 8
        theaters.emplace_back("Brookfield Mall", "Coimbatore", "Tamil Nadu", 6, 4, 9); // 9
        
        // 5. West Bengal (Kolkata, Siliguri)
        theaters.emplace_back("Inox South City", "Kolkata", "West Bengal", 7, 3, 10); // 10
        theaters.emplace_back("PVR City Centre", "Siliguri", "West Bengal", 5, 5, 8); // 11

        // 6. Gujarat (Ahmedabad, Surat, Vadodara)
        theaters.emplace_back("PVR Acropolis", "Ahmedabad", "Gujarat", 6, 4, 10); // 12
        theaters.emplace_back("Cinepolis VR", "Surat", "Gujarat", 5, 5, 12); // 13
        theaters.emplace_back("Inox Inorbit", "Vadodara", "Gujarat", 4, 6, 9); // 14
        
        // 7. Uttar Pradesh (Lucknow, Kanpur, Agra)
        theaters.emplace_back("Wave Mall", "Lucknow", "Uttar Pradesh", 7, 3, 10); // 15
        theaters.emplace_back("PVR Rave 3", "Kanpur", "Uttar Pradesh", 5, 5, 11); // 16
        theaters.emplace_back("INOX Pacific", "Agra", "Uttar Pradesh", 6, 4, 8); // 17

        // --- Showtimes (Distributed across all new theaters) ---
        
        // Maharashtra
        showtimes.emplace_back(movies[0], theaters[0], "10:30 AM", "2025-12-15"); // Mumbai
        showtimes.emplace_back(movies[1], theaters[0], "07:00 PM", "2025-12-15"); 
        showtimes.emplace_back(movies[4], theaters[1], "04:00 PM", "2025-12-15"); // Pune
        showtimes.emplace_back(movies[5], theaters[1], "09:30 PM", "2025-12-15"); // Pune (New Time)
        showtimes.emplace_back(movies[5], theaters[2], "01:00 PM", "2025-12-16"); // Nagpur

        // Karnataka
        showtimes.emplace_back(movies[2], theaters[3], "11:00 AM", "2025-12-16"); // Bangalore
        showtimes.emplace_back(movies[0], theaters[3], "05:00 PM", "2025-12-16"); 
        showtimes.emplace_back(movies[1], theaters[4], "09:00 PM", "2025-12-16"); // Mysore
        showtimes.emplace_back(movies[3], theaters[4], "02:00 PM", "2025-12-16"); // Mysore (New Movie)


        // Delhi NCR
        showtimes.emplace_back(movies[3], theaters[5], "02:00 PM", "2025-12-17"); // New Delhi
        showtimes.emplace_back(movies[5], theaters[6], "06:00 PM", "2025-12-17"); // Gurugram
        showtimes.emplace_back(movies[4], theaters[7], "08:30 PM", "2025-12-17"); // Noida
        showtimes.emplace_back(movies[1], theaters[7], "11:00 AM", "2025-12-17"); // Noida (New Time)

        // Tamil Nadu
        showtimes.emplace_back(movies[1], theaters[8], "10:00 AM", "2025-12-18"); // Chennai
        showtimes.emplace_back(movies[0], theaters[8], "06:45 PM", "2025-12-18"); // Chennai (New Time)
        showtimes.emplace_back(movies[2], theaters[9], "03:00 PM", "2025-12-18"); // Coimbatore

        // West Bengal
        showtimes.emplace_back(movies[0], theaters[10], "12:00 PM", "2025-12-19"); // Kolkata
        showtimes.emplace_back(movies[3], theaters[11], "08:00 PM", "2025-12-19"); // Siliguri

        // Gujarat
        showtimes.emplace_back(movies[5], theaters[12], "04:30 PM", "2025-12-20"); // Ahmedabad
        showtimes.emplace_back(movies[2], theaters[13], "07:30 PM", "2025-12-20"); // Surat
        showtimes.emplace_back(movies[4], theaters[14], "01:00 PM", "2025-12-20"); // Vadodara

        // Uttar Pradesh
        showtimes.emplace_back(movies[0], theaters[15], "06:00 PM", "2025-12-21"); // Lucknow
        showtimes.emplace_back(movies[1], theaters[16], "10:00 AM", "2025-12-21"); // Kanpur
        showtimes.emplace_back(movies[3], theaters[17], "02:30 PM", "2025-12-21"); // Agra

        // Load persisted data after initializing all components
        loadBookingData();
    }

    /**
     * @brief Saves all current bookings (including seat IDs) to a file.
     * New Feature: Data Persistence.
     */
    void saveBookingData() const {
        ofstream outFile(BOOKING_DATA_FILE);
        if (outFile.is_open()) {
            for (const auto& booking : allBookings) {
                outFile << booking.toFileString() << "\n";
            }
            outFile.close();
            // cout << "\n[System] Booking data saved successfully." << endl; // Commented out for minimal UI change
        } else {
            cout << "\n[System Error] Unable to save booking data to file: " << BOOKING_DATA_FILE << endl;
        }
    }

    /**
     * @brief Loads booking data from a file and updates the seat map statuses.
     * New Feature: Data Persistence.
     */
    void loadBookingData() {
        ifstream inFile(BOOKING_DATA_FILE);
        string line;
        if (!inFile.is_open()) {
            // cout << "[System] No previous booking data found. Starting fresh." << endl; // Commented out for minimal UI change
            return;
        }

        int loadedCount = 0;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string segment;
            vector<string> parts;
            while (getline(ss, segment, '|')) {
                parts.push_back(segment);
            }

            if (parts.size() == 3) {
                try {
                    int bookingId = stoi(parts[0]);
                    string uniqueShowId = parts[1];
                    string seatsString = parts[2];

                    // 1. Find the corresponding Showtime
                    Showtime* foundShowtime = nullptr;
                    for (auto& show : showtimes) {
                        if (show.getUniqueShowId() == uniqueShowId) {
                            foundShowtime = &show;
                            break;
                        }
                    }

                    if (foundShowtime) {
                        // 2. Parse seat IDs
                        vector<string> bookedSeats;
                        stringstream seatSs(seatsString);
                        string seatId;
                        while (getline(seatSs, seatId, ',')) {
                            bookedSeats.push_back(seatId);
                            
                            // 3. Update seat status in the Theater's seat map
                            Theater& theater = foundShowtime->getTheater();
                            for (auto& row : theater.getSeatMap()) {
                                for (auto& seat : row) {
                                    if (seat.getId() == seatId) {
                                        seat.setStatus(Seat::BOOKED);
                                        break;
                                    }
                                }
                            }
                        }

                        // 4. Recreate the Booking object and add to allBookings list
                        // This uses the second Booking constructor which is now corrected.
                        allBookings.emplace_back(bookingId, *foundShowtime, bookedSeats); 
                        loadedCount++;
                    }
                } catch (const std::exception& e) {
                    cerr << "[System Error] Error processing booking line: " << line << " (" << e.what() << ")" << endl;
                }
            }
        }
        inFile.close();
        // cout << "[System] Successfully loaded " << loadedCount << " bookings." << endl; // Commented out for minimal UI change
    }

    /**
     * @brief Core logic for the Food & Beverage selection process.
     * @param selectedTheater The theater whose menu is displayed.
     * @return A completed FoodOrder object.
     */
    FoodOrder selectFoodItems(Theater& selectedTheater) {
        FoodOrder order;
        int foodChoice;
        int quantity;
        const auto& menu = selectedTheater.getMenu();

        printHeader("STEP 4: Select Food & Beverages (Optional)");
        cout << "You are ordering from the menu of " << selectedTheater.getName() << "." << endl;

        do {
            cout << "\n" << LINE_SEPARATOR << endl;
            cout << "Menu: " << endl;
            for (size_t i = 0; i < menu.size(); ++i) {
                menu[i].displayItem(i + 1);
            }
            cout << LINE_SEPARATOR << endl;
            cout << "[0] Proceed to Payment (Skip Food / Finish Order)" << endl;
            
            foodChoice = getValidatedIntInput("Enter menu number to add, or 0 to continue: ");

            if (foodChoice >= 1 && foodChoice <= (int)menu.size()) {
                quantity = getValidatedIntInput("Enter quantity for " + menu[foodChoice - 1].getName() + ": ");
                order.addItem(menu[foodChoice - 1], quantity);
                cout << "-> Added " << quantity << " x " << menu[foodChoice - 1].getName() << " to your order." << endl;
                order.displayOrder();
            } else if (foodChoice != 0) {
                cout << "Invalid menu number. Please select from 1 to " << menu.size() << "." << endl;
            }
        } while (foodChoice != 0);

        return order;
    }


    /**
     * @brief Core logic for the seat selection process.
     * @param selectedShowtime The showtime for which seats are being booked.
     * @return A vector of selected seat IDs.
     */
    vector<string> selectSeats(Showtime& selectedShowtime) {
        Theater& theater = selectedShowtime.getTheater();
        auto& seatMap = theater.getSeatMap();
        vector<string> selectedSeatIds;
        string seatIdInput;
        bool done = false;

        printHeader("STEP 3: Select Seats");
        cout << "Theater: " << theater.getName() << " | Movie: " << selectedShowtime.getMovie().getTitle() 
             << " | Time: " << selectedShowtime.getTime() << endl;
        cout << "Legend: [S=Standard, P=Premium, X=Booked, V=Selected]" << endl;
        cout << "Standard Price: Rs " << formatCurrency(TICKET_PRICE_STANDARD) 
             << " | Premium Price: Rs " << formatCurrency(TICKET_PRICE_PREMIUM) << endl;

        // Selection loop
        while (!done) {
            cout << LINE_SEPARATOR << endl;
            // Display Seat Map (Improved: Re-display on every major action for clarity)
            for (const auto& row : seatMap) {
                cout << "Row " << row[0].getId()[0] << " | ";
                for (const auto& seat : row) {
                    seat.displaySeat();
                }
                cout << endl;
            }
            cout << LINE_SEPARATOR << endl;

            cout << "Enter Seat ID to select/deselect (e.g., A1, P5, C10), or type 'DONE' to finish: ";
            if (!(cin >> seatIdInput)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error reading input." << endl;
                continue;
            }
            // Convert to uppercase for case-insensitive comparison
            transform(seatIdInput.begin(), seatIdInput.end(), seatIdInput.begin(), ::toupper);

            if (seatIdInput == "DONE") {
                if (selectedSeatIds.empty()) {
                    cout << "Please select at least one seat before proceeding." << endl;
                    continue;
                }
                done = true;
                break;
            }

            bool found = false;
            for (auto& row : seatMap) {
                for (auto& seat : row) {
                    if (seat.getId() == seatIdInput) {
                        found = true;
                        if (seat.getStatus() == Seat::AVAILABLE) {
                            seat.setStatus(Seat::SELECTED);
                            selectedSeatIds.push_back(seat.getId());
                            cout << "-> Seat " << seatIdInput << " selected. Current Selections: ";
                            for(const auto& id : selectedSeatIds) cout << id << " ";
                            cout << endl;
                        } else if (seat.getStatus() == Seat::SELECTED) {
                            // Deselect
                            seat.setStatus(Seat::AVAILABLE);
                            selectedSeatIds.erase(remove(selectedSeatIds.begin(), selectedSeatIds.end(), seatIdInput), selectedSeatIds.end());
                            cout << "-> Seat " << seatIdInput << " deselected. Current Selections: ";
                            for(const auto& id : selectedSeatIds) cout << id << " ";
                            cout << endl;
                        } else {
                            cout << "Seat " << seatIdInput << " is already BOOKED (X). Select another seat." << endl;
                        }
                        goto seat_selection_next_input; // Exit nested loops
                    }
                }
            }

            if (!found) {
                cout << "Invalid Seat ID: " << seatIdInput << ". Please check the map and try again." << endl;
            }

            seat_selection_next_input:; // Label for the goto
        }

        // Finalize seat statuses (Change SELECTED to BOOKED for the actual transaction)
        for (auto& row : seatMap) {
            for (auto& seat : row) {
                if (seat.getStatus() == Seat::SELECTED) {
                    // This change is permanent only if the Booking is finalized.
                    seat.setStatus(Seat::BOOKED); 
                }
            }
        }

        return selectedSeatIds;
    }


    /**
     * @brief Handles the location (State and City) selection flow.
     * @return The selected City string.
     */
    string selectLocation() {
        int stateChoice, cityChoice;
        string selectedState, selectedCity;
        
        printHeader("STEP 1.1: Select Location (State)");
        for (size_t i = 0; i < states.size(); ++i) {
            cout << "[" << i + 1 << "] " << states[i] << endl;
        }
        
        // State selection loop
        while (true) {
            stateChoice = getValidatedIntInput("Enter State number: ");
            if (stateChoice >= 1 && stateChoice <= (int)states.size()) {
                selectedState = states[stateChoice - 1];
                cout << "-> Selected State: " << selectedState << endl;
                break;
            }
            cout << "Invalid state selection." << endl;
        }

        // Filter cities based on selected state
        vector<string> cities;
        for (const auto& theater : theaters) {
            if (theater.getState() == selectedState) {
                // Only add unique cities
                if (find(cities.begin(), cities.end(), theater.getCity()) == cities.end()) {
                    cities.push_back(theater.getCity());
                }
            }
        }
        
        printHeader("STEP 1.2: Select Location (City)");
        for (size_t i = 0; i < cities.size(); ++i) {
            cout << "[" << i + 1 << "] " << cities[i] << endl;
        }

        // City selection loop
        while (true) {
            cityChoice = getValidatedIntInput("Enter City number: ");
            if (cityChoice >= 1 && cityChoice <= (int)cities.size()) {
                selectedCity = cities[cityChoice - 1];
                cout << "-> Selected City: " << selectedCity << endl;
                break;
            }
            cout << "Invalid city selection." << endl;
        }

        return selectedCity;
    }

    /**
     * @brief Handles the Theater selection flow for a given city.
     * @param city The selected city string.
     * @return A pointer to the selected Theater object, or nullptr if none selected.
     */
    Theater* selectTheater(const string& city) {
        vector<Theater*> cityTheaters;
        for (auto& theater : theaters) {
            if (theater.getCity() == city) {
                cityTheaters.push_back(&theater);
            }
        }

        if (cityTheaters.empty()) {
            cout << "No theaters available in " << city << "." << endl;
            return nullptr;
        }

        printHeader("STEP 2.1: Select Theater");
        cout << "Available theaters in " << city << ":" << endl;
        
        for (size_t i = 0; i < cityTheaters.size(); ++i) {
            cout << "  [" << i + 1 << "] " << cityTheaters[i]->getName() << endl;
        }

        // Theater selection loop
        while (true) {
            int theaterChoice = getValidatedIntInput("Enter Theater number: ");
            if (theaterChoice >= 1 && theaterChoice <= (int)cityTheaters.size()) {
                Theater* selectedTheater = cityTheaters[theaterChoice - 1];
                cout << "-> Selected Theater: " << selectedTheater->getName() << endl;
                return selectedTheater;
            }
            cout << "Invalid theater number." << endl;
        }
    }


    /**
     * @brief Handles the Showtime selection flow for a specific theater.
     * New Feature: Filter by Movie Title.
     * @param theater The selected Theater object.
     * @return A pointer to the selected Showtime object, or nullptr if none selected.
     */
    Showtime* selectShowtimeForTheater(Theater& theater) {
        
        string filterMovieTitle;
        cout << "\nDo you want to filter showtimes by a movie title? (Y/N): ";
        char filterChoice;
        cin >> filterChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (toupper(filterChoice) == 'Y') {
            cout << "Enter part of the movie title to filter (e.g., 'Architect'): ";
            getline(cin, filterMovieTitle);
            cout << "Filtering for movies containing: '" << filterMovieTitle << "'" << endl;
        }
        
        vector<Showtime*> theaterShowtimes;
        // Filter showtimes by comparing the address of the Theater object (since it's a reference)
        for (auto& show : showtimes) {
            if (&show.getTheater() == &theater) {
                // Apply movie title filter
                if (filterMovieTitle.empty() || show.getMovie().getTitle().find(filterMovieTitle) != string::npos) {
                    theaterShowtimes.push_back(&show);
                }
            }
        }

        if (theaterShowtimes.empty()) {
            cout << "No showtimes available at " << theater.getName();
            if (!filterMovieTitle.empty()) {
                cout << " matching your filter.";
            }
            cout << endl;
            return nullptr;
        }

        printHeader("STEP 2.2: Select Showtime (Time & Movie)");
        cout << "Showtimes at " << theater.getName() << ":" << endl;
        
        for (size_t i = 0; i < theaterShowtimes.size(); ++i) {
            // Using the specialized display function for clear output
            theaterShowtimes[i]->displayDetails(i + 1);
        }
        
        // Showtime selection loop
        while (true) {
            int showChoice = getValidatedIntInput("Enter Showtime number to book: ");
            if (showChoice >= 1 && showChoice <= (int)theaterShowtimes.size()) {
                Showtime* selectedShow = theaterShowtimes[showChoice - 1];
                cout << "-> Confirmed: " << selectedShow->getMovie().getTitle() 
                     << " at " << selectedShow->getTime() << endl;
                return selectedShow;
            }
            cout << "Invalid showtime number." << endl;
        }
    }

    /**
     * @brief Allows users to find and cancel an existing booking.
     * New Feature: Booking Management (Cancellation).
     */
    void cancelBooking() {
        printHeader("BOOKING CANCELLATION");
        if (allBookings.empty()) {
            cout << "There are no successful bookings to cancel." << endl;
            return;
        }

        // Display a list of all current bookings for reference
        cout << "Existing Bookings:" << endl;
        for (const auto& booking : allBookings) {
            booking.displayBriefDetails();
        }
        cout << LINE_SEPARATOR << endl;

        int bookingIdToCancel = getValidatedIntInput("Enter the Reference ID of the booking to cancel (or 0 to abort): ");
        if (bookingIdToCancel == 0) {
            cout << "Cancellation aborted." << endl;
            return;
        }

        auto it = allBookings.begin();
        while (it != allBookings.end()) {
            if (it->getId() == bookingIdToCancel) {
                // Found the booking. Perform cancellation.
                cout << "\n--- Confirmation ---" << endl;
                cout << "Are you sure you want to cancel booking ID " << bookingIdToCancel << "? (Y/N): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (toupper(confirm) == 'Y') {
                    // 1. Update seat status in the theater
                    it->cancel(); 

                    // 2. Remove the booking from the list
                    it = allBookings.erase(it);

                    cout << "\n>> BOOKING ID " << bookingIdToCancel << " HAS BEEN SUCCESSFULLY CANCELED." << endl;
                    cout << ">> Corresponding seats are now AVAILABLE." << endl;
                    
                    // 3. Save the updated state to file
                    saveBookingData(); 
                    return;
                } else {
                    cout << "Cancellation operation aborted by user." << endl;
                    return;
                }
            }
            ++it;
        }

        cout << "Error: Booking ID " << bookingIdToCancel << " not found." << endl;
    }


public:
    // Constructor (Initializes data upon creation)
    SystemManager() {
        initializeData();
    }
    
    // Destructor (Ensures data is saved when the application closes)
    ~SystemManager() {
        saveBookingData();
    }

    /**
     * @brief The main execution flow of the booking application.
     */
    void runBookingProcess() {
        
        // --- Main Menu for new features ---
        while (true) {
            printHeader("MAIN MENU");
            cout << "[1] Start New Booking" << endl;
            cout << "[2] Cancel Existing Booking" << endl;
            cout << "[3] Exit Application" << endl;
            cout << LINE_SEPARATOR << endl;
            
            int mainChoice = getValidatedIntInput("Enter your choice: ");
            
            if (mainChoice == 3) {
                break; // Exit loop, destructor will save data
            } else if (mainChoice == 2) {
                cancelBooking();
                continue;
            } else if (mainChoice != 1) {
                cout << "Invalid choice. Please select 1, 2, or 3." << endl;
                continue;
            }

            // --- STEP 1: Location Selection (State, City) ---
            string selectedCity = selectLocation();
            
            // --- STEP 2.1: Theater Selection ---
            Theater* selectedTheaterPtr = selectTheater(selectedCity);

            if (!selectedTheaterPtr) {
                cout << "\nBooking process aborted. No theater selected." << endl;
                continue;
            }
            
            // --- STEP 2.2: Showtime Selection for that Theater ---
            Showtime* selectedShowtimePtr = selectShowtimeForTheater(*selectedTheaterPtr);

            if (!selectedShowtimePtr) {
                cout << "\nBooking process aborted. No showtime selected." << endl;
                continue;
            }

            Showtime& selectedShowtime = *selectedShowtimePtr;
            Theater& selectedTheater = selectedShowtime.getTheater();

            // --- STEP 3: Seat Selection ---
            vector<string> bookedSeatIds = selectSeats(selectedShowtime);

            if (bookedSeatIds.empty()) {
                cout << "\nBooking process aborted. No seats selected." << endl;
                continue; 
            }

            // --- STEP 4: Food & Beverage Selection ---
            FoodOrder finalFoodOrder = selectFoodItems(selectedTheater);

            // --- STEP 5: Finalize Booking and Billing ---
            Booking finalBooking(selectedShowtime, bookedSeatIds, finalFoodOrder);
            finalBooking.generateBill();

            // --- Record the booking and persist data ---
            allBookings.push_back(finalBooking);
            // saveBookingData() is called in the destructor, but also here to update the file immediately
            saveBookingData(); 

            cout << "\nPress Enter to return to the main menu...";
            cin.get();
        }
    }
};

// --- Main Program Entry Point ---
int main() {
    // Set output formatting for currency
    cout << fixed << setprecision(2);
    
    cout << LINE_SEPARATOR << endl;
    cout << APP_NAME << " - C++ OOP Console Project" << endl;
    cout << "Welcome to the world-class movie booking experience." << endl;
    cout << LINE_SEPARATOR << endl;
    
    // Create the system manager object
    SystemManager system;

    // Start the application flow
    system.runBookingProcess();

    cout << "\n" << LINE_SEPARATOR << endl;
    cout << "Application Session Ended. All current bookings have been saved to " << BOOKING_DATA_FILE << endl;
    cout << LINE_SEPARATOR << endl;
    
    return 0;
}