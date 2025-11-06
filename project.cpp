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

const double TICKET_PRICE_STANDARD = 250.00;
const double TICKET_PRICE_PREMIUM = 450.00;
const string APP_NAME = "CineSphere Booking Console";
const string LINE_SEPARATOR = string(70, '-');
const string BOOKING_DATA_FILE = "bookings.txt";

void printHeader(const string &title)
{
    cout << "\n"
         << string(10, '=') << " " << title << " " << string(10, '=') << endl;
}

int getValidatedIntInput(const string &prompt)
{
    int input;
    while (true)
    {
        cout << prompt;
        if (cin >> input)
        {
            if (cin.peek() == '\n')
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return input;
            }
        }
        cout << "Invalid input. Please enter a valid number." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string formatCurrency(double amount)
{
    stringstream ss;
    ss << fixed << setprecision(2) << amount;
    return ss.str();
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


class Product
{
protected:
    string name;
    double price;

public:
    Product(string n, double p) : name(n), price(p) {}

    
    virtual void displayInfo() const = 0;

  
    virtual ~Product() {}

  
    string getName() const { return name; }
    double getPrice() const { return price; }
};


class MenuItem : public Product
{
private:
    string category;

public:
    MenuItem(string n, double p, string c)
        : Product(n, p), category(c) {}

    string getCategory() const { return category; }


    void displayInfo() const override
    {
        cout << "  " << left << setw(30) << name
             << " - " << left << setw(10) << category
             << " @ Rs " << formatCurrency(price) << endl;
    }

    void displayItem(int index) const
    {
        cout << "  [" << index << "] ";
        displayInfo();
    }
};


class Seat
{
public:
    enum Status
    {
        AVAILABLE,
        BOOKED,
        SELECTED
    };
    enum Type
    {
        STANDARD,
        PREMIUM
    };

private:
    string seatId;
    Status status;
    Type type;

public:
    Seat(string id, Type t) : seatId(id), status(AVAILABLE), type(t) {}

    
    string getId() const { return seatId; }
    Status getStatus() const { return status; }
    Type getType() const { return type; }
    double getPrice() const { return (type == PREMIUM ? TICKET_PRICE_PREMIUM : TICKET_PRICE_STANDARD); }

    string getStatusString() const
    {
        if (status == BOOKED)
            return "BOOKED";
        if (status == SELECTED)
            return "SELECTED";
        return "AVAILABLE";
    }

    void setStatus(Status newStatus)
    {
        status = newStatus;
    }

    void displaySeat() const
    {
        string displayChar = (type == PREMIUM ? "P" : "S");
        if (status == BOOKED)
        {
            displayChar = "X";
        }
        else if (status == SELECTED)
        {
            displayChar = "V";
        }
        cout << "[" << displayChar << setw(4) << right << seatId << "]";
    }
};



class Entertainment
{
protected:
    string title;
    string genre;
    int durationMinutes;

public:
    Entertainment(string t, string g, int d)
        : title(t), genre(g), durationMinutes(d) {}

    virtual ~Entertainment() {}

   
    virtual void displayDetails(int index) const
    {
        cout << "  [" << index << "] " << left << setw(40) << title
             << " (" << genre << ", " << durationMinutes << " mins)" << endl;
    }

    string getTitle() const { return title; }
    int getDuration() const { return durationMinutes; }
    string getGenre() const { return genre; }
};


class Movie : public Entertainment
{
private:
    string director;
    string language;

public:
    Movie(string t, string g, int d, string dir = "Unknown", string lang = "English")
        : Entertainment(t, g, d), director(dir), language(lang) {}

   
    void displayDetails(int index) const override
    {
        cout << "  [" << index << "] " << left << setw(40) << title
             << " (" << genre << ", " << durationMinutes << " mins, " << language << ")" << endl;
    }

    string getDirector() const { return director; }
    string getLanguage() const { return language; }
};


class PriceCalculator;

class FoodOrder
{
private:
    map<string, pair<int, double>> orderItems;
    double totalFoodPrice;

public:
    FoodOrder() : totalFoodPrice(0.0) {}

    void addItem(const MenuItem &item, int quantity)
    {
        if (quantity <= 0)
            return;

        if (orderItems.count(item.getName()))
        {
            int currentQty = orderItems[item.getName()].first;
            orderItems[item.getName()].first = currentQty + quantity;
        }
        else
        {
            orderItems[item.getName()] = {quantity, item.getPrice()};
        }
        totalFoodPrice += item.getPrice() * quantity;
    }

    double getTotalPrice() const
    {
        return totalFoodPrice;
    }

    bool isEmpty() const
    {
        return orderItems.empty();
    }

    void displayOrder() const
    {
        cout << "\n    --- Food Order Details ---" << endl;
        if (isEmpty())
        {
            cout << "    (No food items ordered)" << endl;
            return;
        }
        for (const auto &entry : orderItems)
        {
            const string &name = entry.first;
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

    friend class PriceCalculator;
    friend double calculateDiscount(const FoodOrder &order, double discountPercent);
};


double calculateDiscount(const FoodOrder &order, double discountPercent)
{

    return order.totalFoodPrice*(discountPercent / 100.0);
}


class Location
{
protected:
    string name;
    string city;
    string state;

public:
    Location(string n, string c, string s) : name(n), city(c), state(s) {}

    virtual ~Location() {}

   
    virtual void displayLocationInfo() const = 0;

    string getName() const { return name; }
    string getCity() const { return city; }
    string getState() const { return state; }
};


class Theater : public Location
{
private:
    vector<MenuItem> menu;
    vector<vector<Seat>> seatMap;
    int theaterCapacity;

    void initializeSeatMap(int standardRows, int premiumRows, int seatsPerRow)
    {
        char currentRow = 'A';
        theaterCapacity = 0;

        for (int r = 0; r < premiumRows; ++r)
        {
            vector<Seat> row;
            for (int c = 1; c <= seatsPerRow; ++c)
            {
                string seatId = string(1, currentRow) + to_string(c);
                row.emplace_back(seatId, Seat::PREMIUM);
                theaterCapacity++;
            }
            seatMap.push_back(row);
            currentRow++;
        }

        for (int r = 0; r < standardRows; ++r)
        {
            vector<Seat> row;
            for (int c = 1; c <= seatsPerRow; ++c)
            {
                string seatId = string(1, currentRow) + to_string(c);
                row.emplace_back(seatId, Seat::STANDARD);
                theaterCapacity++;
            }
            seatMap.push_back(row);
            currentRow++;
        }
    }

    void initializeMenu()
    {
        menu.emplace_back("Caramel Popcorn (Large)", 350.00, "Popcorn");
        menu.emplace_back("Salty Popcorn (Medium)", 250.00, "Popcorn");
        menu.emplace_back("Coca-Cola (500ml)", 150.00, "Beverage");
        menu.emplace_back("Fresh Lime Soda", 180.00, "Beverage");
        menu.emplace_back("Nachos with Cheese Dip", 290.00, "Snack");
        menu.emplace_back("Veg Burger", 220.00, "Snack");
    }

public:
    Theater(string n, string c, string s, int stdRows, int premRows, int seatsPer)
        : Location(n, c, s), theaterCapacity(0)
    {
        initializeSeatMap(stdRows, premRows, seatsPer);
        initializeMenu();
    }

  
    void displayLocationInfo() const override
    {
        cout << "Theater: " << name << " | City: " << city
             << " | State: " << state << " | Capacity: " << theaterCapacity << " seats" << endl;
    }

    const vector<MenuItem> &getMenu() const { return menu; }
    vector<vector<Seat>> &getSeatMap() { return seatMap; }
    const vector<vector<Seat>> &getConstSeatMap() const { return seatMap; }
    int getCapacity() const { return theaterCapacity; }

    void displayDetails(int index) const
    {
        cout << "  [" << index << "] " << name << ", " << city << endl;
    }

   
    friend class PriceCalculator;
};

class Showtime
{
private:
    const Movie &movie;
    Theater &theater;
    string time;
    string date;
    string uniqueShowId;

    string createUniqueId() const
    {
        return theater.getName() + "|" + date + "|" + time + "|" + movie.getTitle();
    }

public:
    Showtime(const Movie &m, Theater &t, string tm, string d)
        : movie(m), theater(t), time(tm), date(d)
    {
        uniqueShowId = createUniqueId();
    }

    const Movie &getMovie() const { return movie; }
    Theater &getTheater() const { return theater; }
    string getTime() const { return time; }
    string getDate() const { return date; }
    string getUniqueShowId() const { return uniqueShowId; }

    void displayDetails(int index) const
    {
        cout << "  [" << index << "] ";
        cout << left << setw(10) << time;
        cout << " - " << left << setw(30) << movie.getTitle();
        cout << " (" << movie.getDuration() << " mins) on " << date << endl;
    }
};


class PriceCalculator
{
public:
    
    static double calculateTotalRevenue(const Theater &theater, const FoodOrder &order)
    {
        int bookedSeats = 0;
        double ticketRevenue = 0.0;

        for (const auto &row : theater.seatMap)
        {
            for (const auto &seat : row)
            {
                if (seat.getStatus() == Seat::BOOKED)
                {
                    bookedSeats++;
                    ticketRevenue += seat.getPrice();
                }
            }
        }

        // Access private member of FoodOrder
        double foodRevenue = order.totalFoodPrice;

        return ticketRevenue + foodRevenue;
    }

    static double calculateOccupancyRate(const Theater &theater)
    {
        int bookedSeats = 0;
        int totalSeats = theater.theaterCapacity;

        for (const auto &row : theater.seatMap)
        {
            for (const auto &seat : row)
            {
                if (seat.getStatus() == Seat::BOOKED)
                {
                    bookedSeats++;
                }
            }
        }

        return totalSeats > 0 ? (bookedSeats * 100.0) / totalSeats : 0.0;
    }
};

class Booking
{
private:
    static int nextBookingId;
    int bookingId;
    const Showtime *showtimePtr;
    vector<string> bookedSeatIds;
    FoodOrder foodOrder;
    double ticketTotal;
    double grandTotal;
    double appliedDiscount;

    void calculateTicketTotal(Theater &theater)
    {
        ticketTotal = 0.0;
        const auto &seatMap = theater.getConstSeatMap();
        for (const string &seatId : bookedSeatIds)
        {
            for (const auto &row : seatMap)
            {
                for (const auto &seat : row)
                {
                    if (seat.getId() == seatId)
                    {
                        ticketTotal += seat.getPrice();
                        goto seat_found;
                    }
                }
            }
        seat_found:;
        }

      
        appliedDiscount = 0.0;
        if (foodOrder.getTotalPrice() > 500.0)
        {
            appliedDiscount = calculateDiscount(foodOrder, 10.0);
        }

        grandTotal = ticketTotal + foodOrder.getTotalPrice() - appliedDiscount;
    }

public:
    Booking(const Showtime &s, const vector<string> &seats, const FoodOrder &order)
        : showtimePtr(&s), bookedSeatIds(seats), foodOrder(order), appliedDiscount(0.0)
    {
        bookingId = nextBookingId++;
        calculateTicketTotal(s.getTheater());
    }

    Booking(int id, const Showtime &s, const vector<string> &seats)
        : showtimePtr(&s), bookedSeatIds(seats), foodOrder({}), appliedDiscount(0.0)
    {
        bookingId = id;
        calculateTicketTotal(s.getTheater());

        if (id >= nextBookingId)
        {
            nextBookingId = id + 1;
        }
    }

    int getId() const { return bookingId; }
    const Showtime &getShowtime() const { return *showtimePtr; }
    const vector<string> &getBookedSeatIds() const { return bookedSeatIds; }

    void generateBill() const
    {
        printHeader("BOOKING CONFIRMATION & BILL");
        cout << "Reference ID: " << bookingId << endl;
        cout << LINE_SEPARATOR << endl;

        cout << left << setw(20) << "Movie:" << getShowtime().getMovie().getTitle() << endl;
        cout << left << setw(20) << "Theater:" << getShowtime().getTheater().getName()
             << " (" << getShowtime().getTheater().getCity() << ")" << endl;
        cout << left << setw(20) << "Show Time:" << getShowtime().getDate() << " at " << getShowtime().getTime() << endl;
        cout << LINE_SEPARATOR << endl;

        cout << "Ticket Details:" << endl;
        cout << "  Seats Reserved (" << bookedSeatIds.size() << "): ";
        for (size_t i = 0; i < bookedSeatIds.size(); ++i)
        {
            cout << bookedSeatIds[i] << (i < bookedSeatIds.size() - 1 ? ", " : "");
        }
        cout << endl;
        cout << left << setw(20) << "  Ticket Subtotal:" << "Rs " << formatCurrency(ticketTotal) << endl;

        foodOrder.displayOrder();

        if (appliedDiscount > 0.0)
        {
            cout << "\n    ** SPECIAL DISCOUNT APPLIED (10% on Food) **" << endl;
            cout << "    Discount Amount: Rs " << formatCurrency(appliedDiscount) << endl;
        }

        cout << LINE_SEPARATOR << endl;
        cout << ">> " << left << setw(20) << "GRAND TOTAL:" << "Rs " << formatCurrency(grandTotal) << endl;
        cout << LINE_SEPARATOR << endl;

        double occupancy = PriceCalculator::calculateOccupancyRate(getShowtime().getTheater());
        cout << "Theater Occupancy: " << fixed << setprecision(1) << occupancy << "%" << endl;
        cout << "Enjoy your movie! Seats are confirmed." << endl;
    }

    void cancel() const
    {
        Theater &theater = getShowtime().getTheater();
        auto &seatMap = theater.getSeatMap();

        for (const string &seatId : bookedSeatIds)
        {
            for (auto &row : seatMap)
            {
                for (auto &seat : row)
                {
                    if (seat.getId() == seatId && seat.getStatus() == Seat::BOOKED)
                    {
                        seat.setStatus(Seat::AVAILABLE);
                        break;
                    }
                }
            }
        }
    }

    string toFileString() const
    {
        string seatList;
        for (size_t i = 0; i < bookedSeatIds.size(); ++i)
        {
            seatList += bookedSeatIds[i];
            if (i < bookedSeatIds.size() - 1)
            {
                seatList += ",";
            }
        }
        return to_string(bookingId) + "|" + getShowtime().getUniqueShowId() + "|" + seatList;
    }

    void displayBriefDetails() const
    {
        cout << "  [ID: " << bookingId << "] "
             << getShowtime().getMovie().getTitle()
             << " at " << getShowtime().getTime()
             << " on " << getShowtime().getDate()
             << " (" << getShowtime().getTheater().getName() << ")" << endl;
        cout << "    Seats: ";
        for (size_t i = 0; i < bookedSeatIds.size(); ++i)
        {
            cout << bookedSeatIds[i] << (i < bookedSeatIds.size() - 1 ? ", " : "");
        }
        cout << endl;
    }
};

int Booking::nextBookingId = 5001;

class SystemManager
{
private:
    vector<Movie> movies;
    vector<Theater> theaters;
    vector<Showtime> showtimes;
    vector<Booking> allBookings;
    vector<string> states;

    void initializeData()
    {
        states = {"Maharashtra", "Karnataka", "Delhi", "Tamil Nadu", "West Bengal", "Gujarat", "Uttar Pradesh"};

        movies.emplace_back("The AI Architect", "Sci-Fi/Action", 145, "James Cameron", "English");
        movies.emplace_back("Eternal Sun", "Romantic Drama", 120, "Sofia Coppola", "Hindi");
        movies.emplace_back("Rogue Agent 7", "Spy Thriller", 130, "Christopher Nolan", "English");
        movies.emplace_back("Jungle Quest", "Family Animation", 95, "Pete Docter", "Hindi");
        movies.emplace_back("Desert Storm", "War Epic", 160, "Ridley Scott", "English");
        movies.emplace_back("The Last Voyage", "Mystery", 110, "Denis Villeneuve", "English");

        theaters.emplace_back("PVR Phoenix", "Mumbai", "Maharashtra", 6, 4, 10);
        theaters.emplace_back("Cinepolis Amanora", "Pune", "Maharashtra", 5, 5, 8);
        theaters.emplace_back("INOX Empress", "Nagpur", "Maharashtra", 7, 3, 12);

        theaters.emplace_back("Gopalan Cinemas", "Bangalore", "Karnataka", 5, 5, 10);
        theaters.emplace_back("PVR Orion Mall", "Mysore", "Karnataka", 4, 6, 9);

        theaters.emplace_back("Wave Cinemas", "New Delhi", "Delhi", 5, 5, 10);
        theaters.emplace_back("PVR Ambience", "Gurugram", "Delhi", 6, 4, 11);
        theaters.emplace_back("INOX Mall", "Noida", "Delhi", 7, 3, 9);

        theaters.emplace_back("Jazz Cinemas", "Chennai", "Tamil Nadu", 5, 5, 11);
        theaters.emplace_back("Brookfield Mall", "Coimbatore", "Tamil Nadu", 6, 4, 9);

        theaters.emplace_back("Inox South City", "Kolkata", "West Bengal", 7, 3, 10);
        theaters.emplace_back("PVR City Centre", "Siliguri", "West Bengal", 5, 5, 8);

        theaters.emplace_back("PVR Acropolis", "Ahmedabad", "Gujarat", 6, 4, 10);
        theaters.emplace_back("Cinepolis VR", "Surat", "Gujarat", 5, 5, 12);
        theaters.emplace_back("Inox Inorbit", "Vadodara", "Gujarat", 4, 6, 9);

        theaters.emplace_back("Wave Mall", "Lucknow", "Uttar Pradesh", 7, 3, 10);
        theaters.emplace_back("PVR Rave 3", "Kanpur", "Uttar Pradesh", 5, 5, 11);
        theaters.emplace_back("INOX Pacific", "Agra", "Uttar Pradesh", 6, 4, 8);

        showtimes.emplace_back(movies[0], theaters[0], "10:30 AM", "2025-12-15");
        showtimes.emplace_back(movies[1], theaters[0], "07:00 PM", "2025-12-15");
        showtimes.emplace_back(movies[4], theaters[1], "04:00 PM", "2025-12-15");
        showtimes.emplace_back(movies[5], theaters[1], "09:30 PM", "2025-12-15");
        showtimes.emplace_back(movies[5], theaters[2], "01:00 PM", "2025-12-16");

        showtimes.emplace_back(movies[2], theaters[3], "11:00 AM", "2025-12-16");
        showtimes.emplace_back(movies[0], theaters[3], "05:00 PM", "2025-12-16");
        showtimes.emplace_back(movies[1], theaters[4], "09:00 PM", "2025-12-16");
        showtimes.emplace_back(movies[3], theaters[4], "02:00 PM", "2025-12-16");

        showtimes.emplace_back(movies[3], theaters[5], "02:00 PM", "2025-12-17");
        showtimes.emplace_back(movies[5], theaters[6], "06:00 PM", "2025-12-17");
        showtimes.emplace_back(movies[4], theaters[7], "08:30 PM", "2025-12-17");
        showtimes.emplace_back(movies[1], theaters[7], "11:00 AM", "2025-12-17");

        showtimes.emplace_back(movies[1], theaters[8], "10:00 AM", "2025-12-18");
        showtimes.emplace_back(movies[0], theaters[8], "06:45 PM", "2025-12-18");
        showtimes.emplace_back(movies[2], theaters[9], "03:00 PM", "2025-12-18");

        showtimes.emplace_back(movies[0], theaters[10], "12:00 PM", "2025-12-19");
        showtimes.emplace_back(movies[3], theaters[11], "08:00 PM", "2025-12-19");

        showtimes.emplace_back(movies[5], theaters[12], "04:30 PM", "2025-12-20");
        showtimes.emplace_back(movies[2], theaters[13], "07:30 PM", "2025-12-20");
        showtimes.emplace_back(movies[4], theaters[14], "01:00 PM", "2025-12-20");

        showtimes.emplace_back(movies[0], theaters[15], "06:00 PM", "2025-12-21");
        showtimes.emplace_back(movies[1], theaters[16], "10:00 AM", "2025-12-21");
        showtimes.emplace_back(movies[3], theaters[17], "02:30 PM", "2025-12-21");

        loadBookingData();
    }

    void saveBookingData() const
    {
        ofstream outFile(BOOKING_DATA_FILE);
        if (outFile.is_open())
        {
            for (const auto &booking : allBookings)
            {
                outFile << booking.toFileString() << "\n";
            }
            outFile.close();
        }
        else
        {
            cout << "\n[System Error] Unable to save booking data to file: " << BOOKING_DATA_FILE << endl;
        }
    }

    void loadBookingData()
    {
        ifstream inFile(BOOKING_DATA_FILE);
        string line;
        if (!inFile.is_open())
        {
            return;
        }

        int loadedCount = 0;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string segment;
            vector<string> parts;
            while (getline(ss, segment, '|'))
            {
                parts.push_back(segment);
            }

            if (parts.size() == 3)
            {
                try
                {
                    int bookingId = stoi(parts[0]);
                    string uniqueShowId = parts[1];
                    string seatsString = parts[2];

                    Showtime *foundShowtime = nullptr;
                    for (auto &show : showtimes)
                    {
                        if (show.getUniqueShowId() == uniqueShowId)
                        {
                            foundShowtime = &show;
                            break;
                        }
                    }

                    if (foundShowtime)
                    {
                        vector<string> bookedSeats;
                        stringstream seatSs(seatsString);
                        string seatId;
                        while (getline(seatSs, seatId, ','))
                        {
                            bookedSeats.push_back(seatId);

                            Theater &theater = foundShowtime->getTheater();
                            for (auto &row : theater.getSeatMap())
                            {
                                for (auto &seat : row)
                                {
                                    if (seat.getId() == seatId)
                                    {
                                        seat.setStatus(Seat::BOOKED);
                                        break;
                                    }
                                }
                            }
                        }

                        allBookings.emplace_back(bookingId, *foundShowtime, bookedSeats);
                        loadedCount++;
                    }
                }
                catch (const std::exception &e)
                {
                    cerr << "[System Error] Error processing booking line: " << line << " (" << e.what() << ")" << endl;
                }
            }
        }
        inFile.close();
    }

    FoodOrder selectFoodItems(Theater &selectedTheater)
    {
        FoodOrder order;
        int foodChoice;
        int quantity;
        const auto &menu = selectedTheater.getMenu();

        printHeader("STEP 4: Select Food & Beverages (Optional)");
        cout << "You are ordering from the menu of " << selectedTheater.getName() << "." << endl;
        cout << "** Spend over Rs 500 on food to get 10% discount! **" << endl;

        do
        {
            cout << "\n"
                 << LINE_SEPARATOR << endl;
            cout << "Menu: " << endl;
            for (size_t i = 0; i < menu.size(); ++i)
            {
                menu[i].displayItem(i + 1);
            }
            cout << LINE_SEPARATOR << endl;
            cout << "[0] Proceed to Payment (Skip Food / Finish Order)" << endl;

            foodChoice = getValidatedIntInput("Enter menu number to add, or 0 to continue: ");

            if (foodChoice >= 1 && foodChoice <= (int)menu.size())
            {
                quantity = getValidatedIntInput("Enter quantity for " + menu[foodChoice - 1].getName() + ": ");
                order.addItem(menu[foodChoice - 1], quantity);
                cout << "-> Added " << quantity << " x " << menu[foodChoice - 1].getName() << " to your order." << endl;
                order.displayOrder();

                // Show potential discount
                if (order.getTotalPrice() > 500.0)
                {
                    cout << "\n    ** You qualify for 10% food discount! **" << endl;
                }
            }
            else if (foodChoice != 0)
            {
                cout << "Invalid menu number. Please select from 1 to " << menu.size() << "." << endl;
            }
        } while (foodChoice != 0);

        return order;
    }

    vector<string> selectSeats(Showtime &selectedShowtime)
    {
        Theater &theater = selectedShowtime.getTheater();
        auto &seatMap = theater.getSeatMap();
        vector<string> selectedSeatIds;
        string seatIdInput;
        bool done = false;

        printHeader("STEP 3: Select Seats");
        cout << "Theater: " << theater.getName() << " | Movie: " << selectedShowtime.getMovie().getTitle()
             << " | Time: " << selectedShowtime.getTime() << endl;
        cout << "Legend: [S=Standard, P=Premium, X=Booked, V=Selected]" << endl;
        cout << "Standard Price: Rs " << formatCurrency(TICKET_PRICE_STANDARD)
             << " | Premium Price: Rs " << formatCurrency(TICKET_PRICE_PREMIUM) << endl;

        while (!done)
        {
            cout << LINE_SEPARATOR << endl;

            for (const auto &row : seatMap)
            {
                cout << "Row " << row[0].getId()[0] << " | ";
                for (const auto &seat : row)
                {
                    seat.displaySeat();
                }
                cout << endl;
            }
            cout << LINE_SEPARATOR << endl;

            cout << "Enter Seat ID to select/deselect (e.g., A1, P5, C10), or type 'DONE' to finish: ";
            if (!(cin >> seatIdInput))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error reading input." << endl;
                continue;
            }

            transform(seatIdInput.begin(), seatIdInput.end(), seatIdInput.begin(), ::toupper);

            if (seatIdInput == "DONE")
            {
                if (selectedSeatIds.empty())
                {
                    cout << "Please select at least one seat before proceeding." << endl;
                    continue;
                }
                done = true;
                break;
            }

            bool found = false;
            for (auto &row : seatMap)
            {
                for (auto &seat : row)
                {
                    if (seat.getId() == seatIdInput)
                    {
                        found = true;
                        if (seat.getStatus() == Seat::AVAILABLE)
                        {
                            seat.setStatus(Seat::SELECTED);
                            selectedSeatIds.push_back(seat.getId());
                            cout << "-> Seat " << seatIdInput << " selected. Current Selections: ";
                            for (const auto &id : selectedSeatIds)
                                cout << id << " ";
                            cout << endl;
                        }
                        else if (seat.getStatus() == Seat::SELECTED)
                        {
                            seat.setStatus(Seat::AVAILABLE);
                            selectedSeatIds.erase(remove(selectedSeatIds.begin(), selectedSeatIds.end(), seatIdInput), selectedSeatIds.end());
                            cout << "-> Seat " << seatIdInput << " deselected. Current Selections: ";
                            for (const auto &id : selectedSeatIds)
                                cout << id << " ";
                            cout << endl;
                        }
                        else
                        {
                            cout << "Seat " << seatIdInput << " is already BOOKED (X). Select another seat." << endl;
                        }
                        goto seat_selection_next_input;
                    }
                }
            }

            if (!found)
            {
                cout << "Invalid Seat ID: " << seatIdInput << ". Please check the map and try again." << endl;
            }

        seat_selection_next_input:;
        }

        for (auto &row : seatMap)
        {
            for (auto &seat : row)
            {
                if (seat.getStatus() == Seat::SELECTED)
                {
                    seat.setStatus(Seat::BOOKED);
                }
            }
        }

        return selectedSeatIds;
    }

    string selectLocation()
    {
        int stateChoice, cityChoice;
        string selectedState, selectedCity;

        printHeader("STEP 1.1: Select Location (State)");
        for (size_t i = 0; i < states.size(); ++i)
        {
            cout << "[" << i + 1 << "] " << states[i] << endl;
        }

        while (true)
        {
            stateChoice = getValidatedIntInput("Enter State number: ");
            if (stateChoice >= 1 && stateChoice <= (int)states.size())
            {
                selectedState = states[stateChoice - 1];
                cout << "-> Selected State: " << selectedState << endl;
                break;
            }
            cout << "Invalid state selection." << endl;
        }

        vector<string> cities;
        for (const auto &theater : theaters)
        {
            if (theater.getState() == selectedState)
            {
                if (find(cities.begin(), cities.end(), theater.getCity()) == cities.end())
                {
                    cities.push_back(theater.getCity());
                }
            }
        }

        printHeader("STEP 1.2: Select Location (City)");
        for (size_t i = 0; i < cities.size(); ++i)
        {
            cout << "[" << i + 1 << "] " << cities[i] << endl;
        }

        while (true)
        {
            cityChoice = getValidatedIntInput("Enter City number: ");
            if (cityChoice >= 1 && cityChoice <= (int)cities.size())
            {
                selectedCity = cities[cityChoice - 1];
                cout << "-> Selected City: " << selectedCity << endl;
                break;
            }
            cout << "Invalid city selection." << endl;
        }

        return selectedCity;
    }

    Theater *selectTheater(const string &city)
    {
        vector<Theater *> cityTheaters;
        for (auto &theater : theaters)
        {
            if (theater.getCity() == city)
            {
                cityTheaters.push_back(&theater);
            }
        }

        if (cityTheaters.empty())
        {
            cout << "No theaters available in " << city << "." << endl;
            return nullptr;
        }

        printHeader("STEP 2.1: Select Theater");
        cout << "Available theaters in " << city << ":" << endl;

        for (size_t i = 0; i < cityTheaters.size(); ++i)
        {
            cout << "  [" << i + 1 << "] " << cityTheaters[i]->getName() << endl;
        }

        while (true)
        {
            int theaterChoice = getValidatedIntInput("Enter Theater number: ");
            if (theaterChoice >= 1 && theaterChoice <= (int)cityTheaters.size())
            {
                Theater *selectedTheater = cityTheaters[theaterChoice - 1];
                cout << "-> Selected Theater: " << selectedTheater->getName() << endl;
                selectedTheater->displayLocationInfo(); // Using polymorphism
                return selectedTheater;
            }
            cout << "Invalid theater number." << endl;
        }
    }

    Showtime *selectShowtimeForTheater(Theater &theater)
    {
        string filterMovieTitle;
        cout << "\nDo you want to filter showtimes by a movie title? (Y/N): ";
        char filterChoice;
        cin >> filterChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (toupper(filterChoice) == 'Y')
        {
            cout << "Enter part of the movie title to filter (e.g., 'Architect'): ";
            getline(cin, filterMovieTitle);
            cout << "Filtering for movies containing: '" << filterMovieTitle << "'" << endl;
        }

        vector<Showtime *> theaterShowtimes;

        for (auto &show : showtimes)
        {
            if (&show.getTheater() == &theater)
            {
                if (filterMovieTitle.empty() || show.getMovie().getTitle().find(filterMovieTitle) != string::npos)
                {
                    theaterShowtimes.push_back(&show);
                }
            }
        }

        if (theaterShowtimes.empty())
        {
            cout << "No showtimes available at " << theater.getName();
            if (!filterMovieTitle.empty())
            {
                cout << " matching your filter.";
            }
            cout << endl;
            return nullptr;
        }

        printHeader("STEP 2.2: Select Showtime (Time & Movie)");
        cout << "Showtimes at " << theater.getName() << ":" << endl;

        for (size_t i = 0; i < theaterShowtimes.size(); ++i)
        {
            theaterShowtimes[i]->displayDetails(i + 1);
        }

        while (true)
        {
            int showChoice = getValidatedIntInput("Enter Showtime number to book: ");
            if (showChoice >= 1 && showChoice <= (int)theaterShowtimes.size())
            {
                Showtime *selectedShow = theaterShowtimes[showChoice - 1];
                cout << "-> Confirmed: " << selectedShow->getMovie().getTitle()
                     << " at " << selectedShow->getTime() << endl;
                return selectedShow;
            }
            cout << "Invalid showtime number." << endl;
        }
    }

    void cancelBooking()
    {
        printHeader("BOOKING CANCELLATION");
        if (allBookings.empty())
        {
            cout << "There are no successful bookings to cancel." << endl;
            return;
        }

        cout << "Existing Bookings:" << endl;
        for (const auto &booking : allBookings)
        {
            booking.displayBriefDetails();
        }
        cout << LINE_SEPARATOR << endl;

        int bookingIdToCancel = getValidatedIntInput("Enter the Reference ID of the booking to cancel (or 0 to abort): ");
        if (bookingIdToCancel == 0)
        {
            cout << "Cancellation aborted." << endl;
            return;
        }

        auto it = allBookings.begin();
        while (it != allBookings.end())
        {
            if (it->getId() == bookingIdToCancel)
            {
                cout << "\n--- Confirmation ---" << endl;
                cout << "Are you sure you want to cancel booking ID " << bookingIdToCancel << "? (Y/N): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (toupper(confirm) == 'Y')
                {
                    it->cancel();
                    it = allBookings.erase(it);

                    cout << "\n>> BOOKING ID " << bookingIdToCancel << " HAS BEEN SUCCESSFULLY CANCELED." << endl;
                    cout << ">> Corresponding seats are now AVAILABLE." << endl;

                    saveBookingData();
                    return;
                }
                else
                {
                    cout << "Cancellation operation aborted by user." << endl;
                    return;
                }
            }
            ++it;
        }

        cout << "Error: Booking ID " << bookingIdToCancel << " not found." << endl;
    }

public:
    SystemManager()
    {
        initializeData();
    }

    ~SystemManager()
    {
        saveBookingData();
    }

    void runBookingProcess()
    {
        while (true)
        {
            printHeader("MAIN MENU");
            cout << "[1] Start New Booking" << endl;
            cout << "[2] Cancel Existing Booking" << endl;
            cout << "[3] Exit Application" << endl;
            cout << LINE_SEPARATOR << endl;

            int mainChoice = getValidatedIntInput("Enter your choice: ");

            if (mainChoice == 3)
            {
                break;
            }
            else if (mainChoice == 2)
            {
                cancelBooking();
                continue;
            }
            else if (mainChoice != 1)
            {
                cout << "Invalid choice. Please select 1, 2, or 3." << endl;
                continue;
            }

            string selectedCity = selectLocation();

            Theater *selectedTheaterPtr = selectTheater(selectedCity);

            if (!selectedTheaterPtr)
            {
                cout << "\nBooking process aborted. No theater selected." << endl;
                continue;
            }

            Showtime *selectedShowtimePtr = selectShowtimeForTheater(*selectedTheaterPtr);

            if (!selectedShowtimePtr)
            {
                cout << "\nBooking process aborted. No showtime selected." << endl;
                continue;
            }

            Showtime &selectedShowtime = *selectedShowtimePtr;
            Theater &selectedTheater = selectedShowtime.getTheater();

            vector<string> bookedSeatIds = selectSeats(selectedShowtime);

            if (bookedSeatIds.empty())
            {
                cout << "\nBooking process aborted. No seats selected." << endl;
                continue;
            }

            FoodOrder finalFoodOrder = selectFoodItems(selectedTheater);

            Booking finalBooking(selectedShowtime, bookedSeatIds, finalFoodOrder);
            finalBooking.generateBill();

            allBookings.push_back(finalBooking);

            saveBookingData();

            cout << "\nPress Enter to return to the main menu...";
            cin.get();
        }
    }
};

int main()
{
    cout << fixed << setprecision(2);

    cout << LINE_SEPARATOR << endl;
    cout << APP_NAME << " - C++ OOP Console Project" << endl;
    cout << "Welcome to the world-class movie booking experience." << endl;
    cout << "Demonstrating: Encapsulation, Inheritance, Polymorphism, Abstraction," << endl;
    cout << "               Friend Functions, Friend Classes, Virtual Functions" << endl;
    cout << LINE_SEPARATOR << endl;

    SystemManager system;

    system.runBookingProcess();

    cout << "\n"
         << LINE_SEPARATOR << endl;
    cout << "Application Session Ended. All current bookings have been saved to " << BOOKING_DATA_FILE << endl;
    cout << LINE_SEPARATOR << endl;

    return 0;
}
