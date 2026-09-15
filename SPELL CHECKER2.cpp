#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include <list>

using namespace std;

const string BASE_DIR = "[project folder path]";


class HashTable {
    static const size_t INITIAL_SIZE = 4096;
    struct Entry {
        string key;
        int value;
        Entry(const string& k, int v) : key(k), value(v) {}
    };
    vector<list<Entry>> buckets;
    size_t num_elements;

    size_t hash(const string& key) const {
        
        unsigned long hash = 5381;
        for (char c : key)
            hash = ((hash << 5) + hash) + c;
        return hash % buckets.size();
    }

    void rehash() {
        if (num_elements < buckets.size() * 2) return;
        vector<list<Entry>> new_buckets(buckets.size() * 2);
        for (auto& bucket : buckets) {
            for (auto& entry : bucket) {
                size_t idx = djb2(entry.key, new_buckets.size());
                new_buckets[idx].push_back(entry);
            }
        }
        buckets.swap(new_buckets);
    }

    size_t djb2(const string& key, size_t mod) const {
        unsigned long hash = 5381;
        for (char c : key)
            hash = ((hash << 5) + hash) + c;
        return hash % mod;
    }

public:
    HashTable() : buckets(INITIAL_SIZE), num_elements(0) {}

    int& operator[](const string& key) {
        size_t idx = hash(key);
        for (auto& entry : buckets[idx]) {
            if (entry.key == key)
                return entry.value;
        }
        buckets[idx].emplace_back(key, 0);
        ++num_elements;
        rehash();
        return buckets[idx].back().value;
    }

    bool count(const string& key) const {
        size_t idx = hash(key);
        for (const auto& entry : buckets[idx]) {
            if (entry.key == key)
                return true;
        }
        return false;
    }

    vector<string> keys() const {
        vector<string> result;
        for (const auto& bucket : buckets) {
            for (const auto& entry : bucket) {
                result.push_back(entry.key);
            }
        }
        return result;
    }

    int get(const string& key) const {
        size_t idx = hash(key);
        for (const auto& entry : buckets[idx]) {
            if (entry.key == key)
                return entry.value;
        }
        return 0;
    }

    size_t size() const { return num_elements; }
};

class SpellCorrector {
private:
    HashTable dictionary;
    string dictionaryType;

    string preprocess(const string& word) {
        string w = word;
        transform(w.begin(), w.end(), w.begin(), ::tolower);

        vector<string> suffixes = {
            "ing", "ies", "es", "ed", "s", "ly", "ment", "tion", "ness", "able", "ful", "less", "ous", "ive", "al", "er", "est"
        };

        for (const auto& suffix : suffixes) {
            if (w.length() > suffix.length() && w.substr(w.length() - suffix.length()) == suffix) {
                string candidate = w.substr(0, w.length() - suffix.length());
                if (suffix == "ies" && !candidate.empty()) candidate += "y";
                if (suffix == "ing" && !candidate.empty() && candidate.back() == 'e') candidate += "e";
                if (dictionary.count(candidate)) return candidate;
            }
        }
        return w;
    }

    bool loadDictionary(const string& filename) {
        ifstream file(BASE_DIR + filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open dictionary file: " << BASE_DIR + filename << endl;
            return false;
        }
        string word;
        while (file >> word) {
            string processed = word;
            transform(processed.begin(), processed.end(), processed.begin(), ::tolower);
            dictionary[processed]++;
        }

        if (dictionaryType == "UK") {
            dictionary["colour"]++;
            dictionary["favour"]++;
            dictionary["realise"]++;
            dictionary["organise"]++;
        }
        else {
            dictionary["color"]++;
            dictionary["favor"]++;
            dictionary["realize"]++;
            dictionary["organize"]++;
        }

        cout << "Loaded " << dictionary.size() << " words into the dictionary." << endl;
        return true;
    }

    vector<string> suggestCorrections(const string& word) {
        vector<string> suggestions;
        const size_t MAX_SUGGESTIONS = 5;

        
        for (size_t i = 0; i <= word.length(); ++i) {
            for (char c = 'a'; c <= 'z'; ++c) {
                string newWord = word.substr(0, i) + c + word.substr(i);
                if (dictionary.count(newWord)) {
                    suggestions.push_back(newWord);
                }
            }
        }

        
        for (size_t i = 0; i < word.length(); ++i) {
            string newWord = word.substr(0, i) + word.substr(i + 1);
            if (dictionary.count(newWord)) {
                suggestions.push_back(newWord);
            }
        }

        
        for (size_t i = 0; i + 1 < word.length(); ++i) {
            string newWord = word;
            swap(newWord[i], newWord[i + 1]);
            if (dictionary.count(newWord)) {
                suggestions.push_back(newWord);
            }
        }

        
        for (size_t i = 0; i < word.length(); ++i) {
            for (char c = 'a'; c <= 'z'; ++c) {
                if (word[i] == c) continue;
                string newWord = word;
                newWord[i] = c;
                if (dictionary.count(newWord)) {
                    suggestions.push_back(newWord);
                }
            }
        }

        sort(suggestions.begin(), suggestions.end());
        suggestions.erase(unique(suggestions.begin(), suggestions.end()), suggestions.end());
        sort(suggestions.begin(), suggestions.end(), [&](const string& a, const string& b) {
            return dictionary.get(a) > dictionary.get(b);
            });

        if (suggestions.size() > MAX_SUGGESTIONS)
            suggestions.resize(MAX_SUGGESTIONS);

        return suggestions;
    }

    void processWord(string word, ofstream& outfile) {
        string cleanWord;
        bool hasPunctuation = false;
        char punctuation = '\0';
        for (char c : word) {
            if (isalpha(c) || c == '\'') {
                cleanWord += c;
            }
            else if (ispunct(c)) {
                hasPunctuation = true;
                punctuation = c;
            }
        }
        if (cleanWord.empty()) {
            outfile << word;
            return;
        }
        bool isCapitalized = isupper(cleanWord[0]);
        string processed = preprocess(cleanWord);
        if (dictionary.count(processed)) {
            outfile << word;
        }
        else {
            auto suggestions = suggestCorrections(processed);
            if (!suggestions.empty()) {
                cout << "\nMisspelled word: " << cleanWord << endl;
                cout << "Dictionary: " << dictionaryType << " English\n";
                cout << "Suggestions:\n";
                for (size_t i = 0; i < suggestions.size(); ++i) {
                    cout << i + 1 << ". " << suggestions[i] << endl;
                }
                cout << "Enter the number of your choice (or 0 to keep original): ";
                int choice;
                cin >> choice;
                if (choice > 0 && choice <= static_cast<int>(suggestions.size())) {
                    string corrected = suggestions[choice - 1];
                    if (isCapitalized && !corrected.empty()) {
                        corrected[0] = toupper(corrected[0]);
                    }
                    if (hasPunctuation) {
                        corrected += punctuation;
                    }
                    outfile << corrected;
                }
                else {
                    outfile << word;
                }
            }
            else {
                outfile << word;
            }
        }
    }

public:
    bool setDictionary(const string& type) {
        dictionaryType = type;
        string filename = (type == "UK") ? "cleaned_dict_uk.txt" : "cleaned_dict_us.txt";
        return loadDictionary(filename);
    }

    void processFile(const string& inputFile, const string& outputFile) {
        ifstream infile(BASE_DIR + inputFile);
        ofstream outfile(BASE_DIR + outputFile);

        if (!infile.is_open()) {
            cerr << "Error: Could not open input file: " << BASE_DIR + inputFile << endl;
            return;
        }
        if (!outfile.is_open()) {
            cerr << "Error: Could not open output file: " << BASE_DIR + outputFile << endl;
            return;
        }

        string line;
        while (getline(infile, line)) {
            size_t start = 0;
            size_t end = line.find(' ');
            while (end != string::npos) {
                string word = line.substr(start, end - start);
                processWord(word, outfile);
                outfile << " ";
                start = end + 1;
                end = line.find(' ', start);
            }
            string lastWord = line.substr(start);
            if (!lastWord.empty()) {
                processWord(lastWord, outfile);
            }
            outfile << endl;
        }
        infile.close();
        outfile.close();
    }
};

int main() {
    SpellCorrector corrector;
    cout << "Select dictionary type:\n";
    cout << "1. UK English\n";
    cout << "2. US English\n";
    cout << "Enter your choice (1 or 2): ";
    int dictChoice;
    cin >> dictChoice;
    cin.ignore();
    bool dictLoaded = false;
    if (dictChoice == 1) {
        dictLoaded = corrector.setDictionary("UK");
    }
    else if (dictChoice == 2) {
        dictLoaded = corrector.setDictionary("US");
    }
    else {
        cerr << "Invalid choice. Defaulting to US English.\n";
        dictLoaded = corrector.setDictionary("US");
    }
    if (!dictLoaded) {
        cerr << "Failed to load dictionary. Exiting.\n";
        return 1;
    }
    string inputFile, outputFile;
    cout << "Enter input file name (e.g., input.txt): ";
    getline(cin, inputFile);
    cout << "Enter output file name (e.g., output.txt): ";
    getline(cin, outputFile);
    corrector.processFile(inputFile, outputFile);
    cout << "Spell checking complete. Results written to " << BASE_DIR + outputFile << endl;
    return 0;
}