    // CEO Inbox prioritizer: MaxHeap (list-based, dynamic array) priority queue.
    // Usage: ./ceo_inbox testfile.txt   (or pipe the file via stdin)
    #include <iostream>
    #include <fstream>
    #include <string>
    using namespace std;

    struct Email {
        string sender;
        string subject;
        string date;      // MM-DD-YYYY
        int rank;         // higher = read sooner (Boss=5 ... OtherPerson=1)
        int dateKey;      // YYYYMMDD, larger = newer
        long seq;         // arrival order, used only to break exact ties
    };

    // Returns true if a should be read before b.
    bool higher(const Email& a, const Email& b) {
        if (a.rank != b.rank) return a.rank > b.rank;
        if (a.dateKey != b.dateKey) return a.dateKey > b.dateKey;   // newest first
        return a.seq < b.seq;                                         // earlier arrival first
    }

    // MaxHeap stored in a dynamically resized list (array), written from scratch.
    class MaxHeap {
    private:
        Email* data;
        int count;
        int capacity;

        static int parent(int i) { return (i - 1) / 2; }
        static int left(int i)   { return 2 * i + 1; }
        static int right(int i)  { return 2 * i + 2; }

        void grow() {
            int newCap = capacity * 2;
            Email* bigger = new Email[newCap];
            for (int i = 0; i < count; i++) bigger[i] = data[i];
            delete[] data;
            data = bigger;
            capacity = newCap;
        }

        void swapAt(int i, int j) {
            Email t = data[i];
            data[i] = data[j];
            data[j] = t;
        }

        void siftUp(int i) {
            while (i > 0 && higher(data[i], data[parent(i)])) {
                swapAt(i, parent(i));
                i = parent(i);
            }
        }

        void siftDown(int i) {
            while (true) {
                int l = left(i), r = right(i), best = i;
                if (l < count && higher(data[l], data[best])) best = l;
                if (r < count && higher(data[r], data[best])) best = r;
                if (best == i) break;
                swapAt(i, best);
                i = best;
            }
        }

    public:
        MaxHeap() : data(new Email[16]), count(0), capacity(16) {}
        ~MaxHeap() { delete[] data; }
        MaxHeap(const MaxHeap&) = delete;
        MaxHeap& operator=(const MaxHeap&) = delete;

        bool empty() const { return count == 0; }
        int size() const { return count; }

        void insert(const Email& e) {
            if (count == capacity) grow();
            data[count] = e;
            siftUp(count);
            count++;
        }

        const Email& peekMax() const { return data[0]; }   // call only if !empty()

        void removeMax() {                                  // call only if !empty()
            count--;
            if (count > 0) {
                data[0] = data[count];
                siftDown(0);
            }
        }
    };

    string trim(const string& s) {
        size_t b = 0, e = s.size();
        while (b < e && (s[b] == ' ' || s[b] == '\t' || s[b] == '\r' || s[b] == '\n')) b++;
        while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t' || s[e - 1] == '\r' || s[e - 1] == '\n')) e--;
        return s.substr(b, e - b);
    }

    int senderRank(const string& s) {
        if (s == "Boss") return 5;
        if (s == "Subordinate") return 4;
        if (s == "Peer") return 3;
        if (s == "ImportantPerson") return 2;
        return 1;   // OtherPerson
    }

    // MM-DD-YYYY -> YYYYMMDD
    int dateToKey(const string& d) {
        if (d.size() < 10) return 0;
        int mm = stoi(d.substr(0, 2));
        int dd = stoi(d.substr(3, 2));
        int yyyy = stoi(d.substr(6, 4));
        return yyyy * 10000 + mm * 100 + dd;
    }

    int main(int argc, char* argv[]) {
        ifstream file;
        istream* in = &cin;
        if (argc > 1) {
            file.open(argv[1]);
            if (!file) {
                cerr << "Could not open file: " << argv[1] << endl;
                return 1;
            }
            in = &file;
        }

        MaxHeap inbox;
        long seq = 0;
        string line;

        while (getline(*in, line)) {
            line = trim(line);
            if (line.empty()) continue;

            if (line.compare(0, 6, "EMAIL ") == 0) {
                string rest = line.substr(6);
                size_t c1 = rest.find(',');
                size_t c2 = (c1 == string::npos) ? string::npos : rest.find(',', c1 + 1);
                if (c1 == string::npos || c2 == string::npos) continue;   // malformed

                Email e;
                e.sender  = trim(rest.substr(0, c1));
                e.subject = trim(rest.substr(c1 + 1, c2 - c1 - 1));
                e.date    = trim(rest.substr(c2 + 1));
                e.rank    = senderRank(e.sender);
                e.dateKey = dateToKey(e.date);
                e.seq     = seq++;
                inbox.insert(e);
            }
            else if (line == "NEXT") {
                if (inbox.empty()) {
                    cout << "No emails to read." << endl;
                } else {
                    const Email& e = inbox.peekMax();
                    cout << "Next email:" << endl;
                    cout << "Sender: "  << e.sender  << endl;
                    cout << "Subject: " << e.subject << endl;
                    cout << "Date: "    << e.date    << endl;
                }
            }
            else if (line == "READ") {
                if (!inbox.empty()) inbox.removeMax();
            }
            else if (line == "COUNT") {
                cout << "There are " << inbox.size() << " emails to read." << endl;
            }
        }
        return 0;
    }