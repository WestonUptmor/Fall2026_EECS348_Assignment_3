//EECS 348 Assignment 3
//Program that sorts emails
//Claude was used to help debug code
//Weston G. Uptmor
//10/1/2026

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

struct Email { //structure of email
    string sender;
    string subject;
    string date; //date but as a string
    int priority; 
    int dateKey; //added this so that dates can be converted to integers
};

class MaxHeap {
private:
    vector<Email> heap;

    // Returns true if a should have higher priority than b
    bool higherPriority(const Email& a, const Email& b) const {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.dateKey > b.dateKey;
    }

    void swapEmails(int i, int j) { //swaps positions of emails in the heap
        Email temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    // Move an element upward, does this by moving element up until its parent has equal or higher priority
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (higherPriority(heap[index], heap[parent])) {//checks the parents
                swapEmails(index, parent);
                index = parent;
            }
            else {
                break;
            }
        }
    }

    // Move an element downward, used after removal by swapping with its highest priority child until it fits
    void heapifyDown(int index) {
        int size = heap.size();

        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index; //assumes current node is largest
            //checks left child
            if (left < size &&
                higherPriority(heap[left], heap[largest])) {
                largest = left;
            }
            //checks right child
            if (right < size &&
                higherPriority(heap[right], heap[largest])) {
                largest = right;
            }

            if (largest != index) {
                swapEmails(index, largest);
                index = largest;
            }
            else {
                break;
            }
        }
    }

public:
    // Add an email to the MaxHeap
    void insert(Email email) {
        heap.push_back(email);
        heapifyUp(heap.size() - 1);
    }

    // Return the highest-priority email without removing it
    Email getMax() {
        return heap[0];
    }

    // Remove the highest-priority email
    void removeMax() {
        if (heap.empty()) {
            return;
        }

        heap[0] = heap.back(); //move last element to root
        heap.pop_back();

        if (!heap.empty()) {
            heapifyDown(0);
        }
    }

    // Return number of emails
    int size() {
        return heap.size();
    }
    //trye if no emails
    bool empty() {
        return heap.empty();
    }
};


// Convert sender category to priority
int getPriority(string sender) {
    if (sender == "Boss")
        return 5;
    else if (sender == "Subordinate")
        return 4;
    else if (sender == "Peer")
        return 3;
    else if (sender == "ImportantPerson")
        return 2;
    else
        return 1; // OtherPerson
}


// Print highest priority email without removing it
void printNext(MaxHeap& emails) {
    if (emails.empty()) {
        return; //nothing to print
    }

    Email email = emails.getMax();

    cout << "Next email:" << endl;
    cout << "Sender: " << email.sender << endl;
    cout << "Subject: " << email.subject << endl;
    cout << "Date: " << email.date << endl;
}


// Process an EMAIL command
void processEmail(string line, MaxHeap& emails) {
    // Remove "EMAIL " from beginning
    line = line.substr(6);

    size_t firstComma = line.find(',');
    size_t secondComma = line.find(',', firstComma + 1);

    string sender = line.substr(0, firstComma);
    string subject = line.substr(
        firstComma + 1,
        secondComma - firstComma - 1
    );
    string date = line.substr(secondComma + 1);

    //creates object
    Email email;
    email.sender = sender;
    email.subject = subject;
    email.date = date;
    email.priority = getPriority(sender);

    // turns date into integers
    int month;
    int day;
    int year;
    char dash;
    stringstream ss(date);
    ss >> month >> dash >> day >> dash >> year;
    email.dateKey = year * 10000 + month * 100 + day;

    emails.insert(email);
}


int main() {
    MaxHeap emails;

    string command;
    //reads commands from inputs
    while (getline(cin, command)) {

        // EMAIL command
        if (command.substr(0, 5) == "EMAIL") {
            processEmail(command, emails);
        }

        // NEXT command
        else if (command == "NEXT") {
            printNext(emails);
        }

        // READ command
        else if (command == "READ") {
            emails.removeMax();
        }

        // COUNT command
        else if (command == "COUNT") {
            cout << "There are "
                 << emails.size()
                 << " emails to read."
                 << endl;
        }
    }

    return 0;
}