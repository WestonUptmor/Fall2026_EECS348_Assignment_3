#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

struct Email {
    string sender;
    string subject;
    string date;

    int priority;
};

class MaxHeap {
private:
    vector<Email> heap;

    // Returns true if a should have higher priority than b
    bool higherPriority(const Email& a, const Email& b) {
        // Higher sender priority wins
        if (a.priority != b.priority) {
            return a.priority > b.priority;
        }

        // If same sender category, newer date wins
        // MM-DD-YYYY can be converted into YYYYMMDD
        int aMonth, aDay, aYear;
        int bMonth, bDay, bYear;

        char dash;

        stringstream aStream(a.date);
        aStream >> aMonth >> dash >> aDay >> dash >> aYear;

        stringstream bStream(b.date);
        bStream >> bMonth >> dash >> bDay >> dash >> bYear;

        if (aYear != bYear)
            return aYear > bYear;

        if (aMonth != bMonth)
            return aMonth > bMonth;

        return aDay > bDay;
    }

    void swapEmails(int i, int j) {
        Email temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    // Move an element upward
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (higherPriority(heap[index], heap[parent])) {
                swapEmails(index, parent);
                index = parent;
            }
            else {
                break;
            }
        }
    }

    // Move an element downward
    void heapifyDown(int index) {
        int size = heap.size();

        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size &&
                higherPriority(heap[left], heap[largest])) {
                largest = left;
            }

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

        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty()) {
            heapifyDown(0);
        }
    }

    // Return number of emails
    int size() {
        return heap.size();
    }

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


// Print the next email
void printNext(MaxHeap& emails) {
    if (emails.empty()) {
        return;
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

    // Find the commas
    size_t firstComma = line.find(',');
    size_t secondComma = line.find(',', firstComma + 1);

    string sender = line.substr(0, firstComma);
    string subject = line.substr(
        firstComma + 1,
        secondComma - firstComma - 1
    );
    string date = line.substr(secondComma + 1);

    Email email;

    email.sender = sender;
    email.subject = subject;
    email.date = date;
    email.priority = getPriority(sender);

    emails.insert(email);
}


int main() {
    MaxHeap emails;

    string command;

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