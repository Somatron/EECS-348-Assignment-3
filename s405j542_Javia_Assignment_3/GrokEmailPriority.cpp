/*
    Program name: GrokEmailPriority.cpp
    Description: A max heap sorting program written in C++ that automatically sorts emails for a CEO
    working at a busy company to read, the program prioritizes emails in order that
    makes it easier for the CEO to know which tasks to focus on first. 

    Inputs: User sends test file that includes Email types, counts email, goes to next email, and read email.
    Outputs: Depending on what text the user inputs, the output will either display how many emails to be read or shows user what email the person would be reading. 
    Collaborators: Som Javia
    
    Code Source: Grok AI
    Author's name: Som Javia
    Creation Date: 9/18/2026 
    Revisions: Create program in C++

    I AUTHORED ALL OF THE CODE HERE IN DISPLAY 
*/

#include <iostream> //input output manipulation
#include <string> //pretty obvious
#include <vector> //for <sender, message, date> format
#include <sstream> //string stream format
#include <cctype> //test and convert individual characters

// Email class - holds one email's data and provides comparison for MaxHeap
class Email {
private:
    std::string sender; //format sender into string
    std::string subject; //do the samething for subject
    std::string date;          // original "MM-DD-YYYY"
    int priority;              // higher number = higher priority
    int dateValue;             // YYYYMMDD for easy comparison (newer = larger)

    // Map sender category to priority level
    static int getPriority(const std::string& s) { // read based on what sender is
        if (s == "Boss")            return 5; //boss is super important
        if (s == "Subordinate")     return 4; //decrement it by one
        if (s == "Peer")            return 3; //each time
        if (s == "ImportantPerson") return 2; 
        if (s == "OtherPerson")     return 1; //this is how we rank it
        return 0; // should never happen given problem constraints
    }

    // Convert "MM-DD-YYYY" to integer YYYYMMDD
    static int parseDate(const std::string& d) {
        // d is guaranteed to be MM-DD-YYYY
        int month = (d[0] - '0') * 10 + (d[1] - '0'); //month will be between 1-12
        int day   = (d[3] - '0') * 10 + (d[4] - '0'); //dates will be between 0-31
        int year  = (d[6] - '0') * 1000 + (d[7] - '0') * 100 //year is usually between 0000-9999
                  + (d[8] - '0') * 10   + (d[9] - '0'); //if we survive of course
        return year * 10000 + month * 100 + day; //return overall formate of YYYMMDD
    }

public:
    Email() : priority(0), dateValue(0) {} //rank on priority and recent email 

    Email(const std::string& snd, const std::string& sub, const std::string& dt) //grab string on sender, message, year
        : sender(snd), subject(sub), date(dt) {
        priority  = getPriority(snd); //id for how important this person is
        dateValue = parseDate(dt); //and also format date
    }

    // Max-heap comparison: higher priority first;
    // if equal priority, newer date first
    bool operator>(const Email& other) const {
        if (priority != other.priority)
            return priority > other.priority;
        return dateValue > other.dateValue;
    }

    // Display in the exact required format
    void display() const {
        std::cout << "\tSender: " << sender << "\n"; //this is usually for the NEXT command
        std::cout << "\tSubject: " << subject << "\n"; //where it prints out the email
        std::cout << "\tDate: " << date << "\n"; //on top of the heap
    }

    // Accessors (needed only for debugging if desired)
    const std::string& getSender()  const { return sender; }
    const std::string& getSubject() const { return subject; }
    const std::string& getDate()    const { return date; }
};

// MaxHeap class - list-based (std::vector) implementation from scratch
class MaxHeap {
private:
    std::vector<Email> heap; //put each component into a list [sender, message, date]

    // Restore heap property after insertion (bubble up)
    void heapifyUp(int index) { 
        while (index > 0) {
            int parent = (index - 1) / 2; //begin swapping of each node and parent if the
            if (heap[index] > heap[parent]) { //node we added ends up being the biggest, (it all depends)
                std::swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break; //exit while loop, we dont do recurrsion
            }
        }
    }

    // Restore heap property after removal (bubble down)
    void heapifyDown(int index) {
        int size = static_cast<int>(heap.size());
        while (true) {
            int left  = 2 * index + 1;
            int right = 2 * index + 2; //compare children nodes
            int largest = index; //select the current index as largest

            if (left < size && heap[left] > heap[largest])
                largest = left; //swap the largest between
            if (right < size && heap[right] > heap[largest])
                largest = right; //left or right node

            if (largest != index) { //if our largest value isnt our current index
                std::swap(heap[index], heap[largest]); //swap between largest and current index
                index = largest; //so that way the largest element can be on top
            } else {
                break; //escape
            }
        }
    }

public:
    // Insert a new email into the heap
    void insert(const Email& email) {
        heap.push_back(email); //add all email components into heap
        heapifyUp(static_cast<int>(heap.size()) - 1); //reorder heap after insertion 
    }

    // Return (but do not remove) the highest-priority email
    // Precondition: !empty()
    const Email& peek() const {
        return heap[0];
    }

    // Remove and return the highest-priority email
    // Precondition: !empty()
    Email extractMax() {
        Email maxEmail = heap[0]; //find root
        heap[0] = heap.back(); //access last element
        heap.pop_back(); //remove root
        if (!heap.empty()) //as long as the heap isnt empty
            heapifyDown(0); //swap elements
        return maxEmail;
    }

    bool empty() const {
        return heap.empty(); //return if heap empty
    }

    int size() const {
        return static_cast<int>(heap.size()); //return how big the heap is
    }
};

// Helper: trim leading/trailing whitespace
std::string trim(const std::string& s) {
    size_t start = 0; //begin text extraction
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) //check if space
        ++start; //trims left side
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) //check if space
        --end; //trims right side
    return s.substr(start, end - start); //at the end of the day we remove some white space
}

int main() {
    MaxHeap inbox; //overall heap will be represented as an inbox
    std::string line; //extract line by string

    while (std::getline(std::cin, line)) { //loop through line
        line = trim(line); //trim each line
        if (line.empty()) //if the line is empty
            continue; //continue

        // Determine command
        if (line.compare(0, 5, "EMAIL") == 0) {
            // Format: EMAIL <sender>,<subject>,<date>
            // Subject may contain spaces but no commas
            std::string rest = trim(line.substr(5));

            // Find the two commas that separate the three fields
            size_t firstComma  = rest.find(',');
            size_t secondComma = rest.find(',', firstComma + 1);

            if (firstComma == std::string::npos || secondComma == std::string::npos) //first comma person, second comma message
                continue; // malformed line – ignore

            std::string sender  = trim(rest.substr(0, firstComma)); //grab person
            std::string subject = trim(rest.substr(firstComma + 1, secondComma - firstComma - 1)); //after first comma comes the message
            std::string date    = trim(rest.substr(secondComma + 1)); //then the second comma leads to the date

            Email e(sender, subject, date); //format into email
            inbox.insert(e); //add it into the heap
        }
        else if (line == "COUNT") { //count the heap size (how many emails there are)
            std::cout << "There are " << inbox.size() << " emails to read.\n\n";
        }
        else if (line == "NEXT") { //look at email 
            if (!inbox.empty()) { //as long as inbox isnt empty
                std::cout << "Next email:\n";
                inbox.peek().display(); //shows who sent it, message, and date
                std::cout << "\n";   // blank line after each NEXT block (matches sample)
            }
            // if empty: do nothing (as required)
        }
        else if (line == "READ") { //remove email
            if (!inbox.empty()) { //which the most important email is read and next one in the list becomes ready to read
                inbox.extractMax();   // remove highest priority, no display
            }
            // if empty: do nothing
        }
        // any other line is ignored
    }

    return 0; //program ran successfully
}
