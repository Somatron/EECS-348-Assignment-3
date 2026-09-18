/*
    Program name: GeminiEmailPriority.cpp
    Description: A max heap sorting program that automatically sorts emails for a CEO
    working at a busy company to read, the program prioritizes emails in order that
    makes it easier for the CEO to know which tasks to focus on first. 

    Inputs: User sends test file that includes Email types, counts email, goes to next email, and read email.
    Outputs: Depending on what text the user inputs, the output will either display how many emails to be read or shows user what email the person would be reading. 
    Collaborators: Som Javia
    
    Code Source: Google Gemini
    Author's name: Som Javia
    Creation Date: 9/18/2026 
    Revisions: Create program in C++
    Revision Date: 9/18/2026
    Other sources: None

    I AUTHORED ALL OF THE CODE HERE IN DISPLAY 
*/

#include <iostream> //input output manipulation
#include <string> //pretty obvious
#include <vector> //for <sender, message, date> format
#include <sstream> //string stream format

class Email { //categorize the email format
private: //private to restrict visibility
    std::string sender; //person who sent the email
    std::string subject; //what the email is about
    std::string dateStr; //date
    int priorityLevel; //depending on the person, rank on how important the email is
    int month, day, year; //date where it comes from

    void parseDate(const std::string& date) { // grab the date string we revieve
        // Formatted as MM-DD-YYYY
        std::stringstream ss(date); //format it
        std::string m, d, y; 
        std::getline(ss, m, '-'); //seperate
        std::getline(ss, d, '-'); //month day year
        std::getline(ss, y, '-'); //by detecting the "-"
        month = std::stoi(m); //string into int
        day = std::stoi(d); 
        year = std::stoi(y);
    }

    void assignPriority(const std::string& senderCategory) { //fun part: we rank people on how important they are
        if (senderCategory == "Boss") priorityLevel = 5; //boss is SUPPER IMPORTANT
        else if (senderCategory == "Subordinate") priorityLevel = 4; //also important
        else if (senderCategory == "Peer") priorityLevel = 3; //co worker
        else if (senderCategory == "ImportantPerson") priorityLevel = 2; //yeah i guess theyre important to
        else if (senderCategory == "OtherPerson") priorityLevel = 1; //ah forget about it
        else priorityLevel = 0;
    }

public:
    Email() : priorityLevel(0), month(0), day(0), year(0) {} //categorize email ranking by how important it is and date it was sent

    Email(std::string s, std::string sub, std::string d) { //parameters of extrating the email
        sender = s; //s for sender
        subject = sub; //what the message is
        dateStr = d; //date it was sent
        assignPriority(s); //categorize the email
        parseDate(d); //extract date into day month year
    }

    std::string getSender() const { return sender; } //extract the whole line and seperate it 
    std::string getSubject() const { return subject; } //based on sender subject and datestring
    std::string getDate() const { return dateStr; } //which gets parsed differently

    // Overload comparison operators for MaxHeap placement
    bool operator<(const Email& other) const {
        if (this->priorityLevel != other.priorityLevel) { //pointer, compare priorities of 2 people
            return this->priorityLevel < other.priorityLevel; //tell us whos more important
        }
        // If same sender priority, newer date wins (has higher value)
        if (this->year != other.year) return this->year < other.year;
        if (this->month != other.month) return this->month < other.month;
        return this->day < other.day; //repeat until we get the latest email we can possibly can
    }
};

class MaxHeap { //heap to add our emails
private: //make it private because no one should be reading a CEOs emails
    std::vector<Email> heap; //grab the string and add the sender, subject, and date upon the heap

    void heapifyUp(int index) { //when we add to the heap
        while (index > 0) {
            int parent = (index - 1) / 2; //we have to reorder it
            if (heap[parent] < heap[index]) {
                std::swap(heap[parent], heap[index]); //so that the category is organized
                index = parent;
            } else {
                break; //instead of recurrsion (which required a function) we use a while loop
            }
        }
    }


    //when removing from a heap we organize every node in the heap to make sure it follows the max heap format
    void heapifyDown(int index) { //when we remove a heap
        int size = heap.size();
        while (2 * index + 1 < size) { //we have to reorganize between children
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2; //of each tree
            int largest = leftChild;

            if (rightChild < size && heap[leftChild] < heap[rightChild]) {
                largest = rightChild; //repeat sorting between 2 children to see which one is the largest
            }

            if (heap[index] < heap[largest]) {
                std::swap(heap[index], heap[largest]);
                index = largest; //if the biggest heap is more than the current index we swap
            } else {
                break; //escape recurrsion
            }
        }
    }

public:
    void insert(const Email& email) { //add email
        heap.push_back(email);
        heapifyUp(heap.size() - 1); //organize heap after add
    }

    void pop() { //remove email (read)
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) { //as long as the heap isnt empty we pop from the heap
            heapifyDown(0); //and reorganize
        }
    }

    Email peek() const { //show us the most important email right now
        if (!heap.empty()) { //as long as the heap isnt empty
            return heap[0]; //show us what the email is
        }
        return Email();
    }

    int size() const {
        return heap.size(); //count size
    }

    bool empty() const { //give us true and false if empty or not
        return heap.empty();
    }
};

class EmailProcessor { //proccess the emails
private:
    MaxHeap priorityQueue; //rank on importance

public: 
    void processCommand(const std::string& line) { //grab the WHOLE line
        if (line.empty()) return; //if line empty skip

        std::stringstream ss(line); //streamline the string, c++ has some weird ways of manipulating string
        std::string command; //create string command
        ss >> command; //grab the string from file and store it into a variable

        if (command == "EMAIL") { //if EMAIL is the first thing
            std::string remaining, sender, subject, date; //create whats in the email
            std::getline(ss, remaining); //grab remaining lines
            
            // Clean leading spaces
            if (!remaining.empty() && remaining[0] == ' ') {
                remaining = remaining.substr(1); //grab smaller part of text string
            }

            std::stringstream fields(remaining); //break down the line into 3 parts
            std::getline(fields, sender, ','); //sender 
            std::getline(fields, subject, ','); //message
            std::getline(fields, date, ','); //and date, we do this by detecting the comma

            Email email(sender, subject, date); //send all of this into our email class
            priorityQueue.insert(email); //add them after success
        } 
        else if (command == "COUNT") { //if count we just count how big the heap is
            std::cout << "There are " << priorityQueue.size() << " emails to read." << std::endl; //tell us size of heap
        } 
        else if (command == "NEXT") { //grab the top of the heap
            if (!priorityQueue.empty()) {
                Email nextEmail = priorityQueue.peek(); //peek means see whats at top
                std::cout << "Next email:" << std::endl; // basically a print statement
                std::cout << "      Sender: " << nextEmail.getSender() << std::endl; //basically access our methods of the peek
                std::cout << "      Subject: " << nextEmail.getSubject() << std::endl; //return us what we want 
                std::cout << "      Date: " << nextEmail.getDate() << std::endl; //same thing with returning date from the peek of the email
            }
        } 
        else if (command == "READ") { //if we read
            if (!priorityQueue.empty()) {
                priorityQueue.pop(); //simply delete the top of the queue (as long as it isnt empty)
            }
        }
    }
};

int main() { //program return a 0 or 1
    EmailProcessor processor; //process the whole text file
    std::string line; //grab line by line

    // Reads commands sequentially from standard input until EOF
    while (std::getline(std::cin, line)) {
        processor.processCommand(line); //process each line
    }

    return 0; //program ran successfully
}
