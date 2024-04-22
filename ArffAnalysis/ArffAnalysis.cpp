// Name:        Jared Schneider
// Created:     4/17/2024
// Problem Description: Reads training data from a weka arff file and stores them in data structures and then generates OneR outputs. OneR rule is in fact a set of 
// rules that all based on value of a SINGLE attribute.  In addition the program also prints the error 
// rate of the oneR rule (set) on the training set.  We assume that all attributes are nominal type and thus 
// no discretization is required.
//
// Note: If you want to see the full Data Structure outputed uncomment line 121.
// 
// Performance Summary:
// for weather.arff the error rate was 4/14, Restaurant.arff the error rate was 2/12, and
// for contactLense.arff the error rate was 7/24. 

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

struct AttributeType {
    string name;
    vector<string> values;
};

string trim(const string& str) {
    const auto strBegin = str.find_first_not_of(" \t");
    if (strBegin == string::npos) return ""; 

    const auto strEnd = str.find_last_not_of(" \t");
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

void readARFFFile(const string& filename);
void printDataSection(const vector<AttributeType>& attributes, const vector<vector<int>>& dataset);
void performOneR(const vector<AttributeType>& attributes, const vector<vector<int>>& dataset);

int main() {
    string filename;

    cout << "Please enter name of the data file: ";
    cin >> filename;

    readARFFFile(filename);

    return 0;
}

void readARFFFile(const string& filename) {
    ifstream file(filename);
    string line;
    vector<AttributeType> attributes;
    vector<vector<int>> dataset;
    bool dataSection = false;

    if (!file.is_open()) {
        cerr << "Could not open file " << filename << endl;
        return;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string firstWord;
        ss >> firstWord;

        if (firstWord.empty() || firstWord[0] == '%') continue;

        if (firstWord == "@attribute") {
            AttributeType attribute;
            ss >> attribute.name;
            string values;
            getline(ss, values, '{');
            getline(ss, values, '}');
            stringstream valueStream(values);
            string value;
            while (getline(valueStream, value, ',')) {
                attribute.values.push_back(trim(value));
            }
            attributes.push_back(attribute);
        } else if (firstWord == "@data") {
            dataSection = true;
            continue;
        }

        if (dataSection) {
            stringstream valueStream(line);
            string value;
            vector<int> instance;
            int attrIndex = 0;
            while (getline(valueStream, value, ',')) {
                value = trim(value); 
                auto it = find(attributes[attrIndex].values.begin(), attributes[attrIndex].values.end(), value);
                if (it != attributes[attrIndex].values.end()) {
                    instance.push_back(distance(attributes[attrIndex].values.begin(), it));
                } else {
                    instance.push_back(-1); //For missing or unrecognized values
                }
                attrIndex++;
            }
            dataset.push_back(instance);
        }
    }

    file.close();

    cout << attributes.size() << " attributes" << endl;
    for (const auto& attribute : attributes) {
        cout << attribute.name << "(" << attribute.values.size() << "): ";
        for (const auto& value : attribute.values) {
            cout << value << " ";
        }
        cout << endl;
    }

    //printDataSection(attributes, dataset);
    performOneR(attributes, dataset);
}

void printDataSection(const vector<AttributeType>& attributes, const vector<vector<int>>& dataset) {
    cout << "\nData Section:\n";
    for (const auto& instance : dataset) {
        for (size_t i = 0; i < instance.size(); ++i) {
            int valueIndex = instance[i];
            if (valueIndex != -1 && valueIndex < attributes[i].values.size()) {
                cout.width(10); cout << attributes[i].values[valueIndex];
            } else {
                cout.width(10); cout << "?"; //Edge case for if Attr value is missing or not recognized
            }
        }
        cout << endl;
    }
}

void performOneR(const vector<AttributeType>& attributes, const vector<vector<int>>& dataset) {
    int decisionAttrIndex = attributes.size() - 1; // Assuming the last attribute is the decision attribute
    int minError = dataset.size(); // Start with the maximum possible error (all wrong)
    string bestAttribute;
    map<string, string> bestRules;

    for (int attrIndex = 0; attrIndex < decisionAttrIndex; ++attrIndex) {
        // Count occurrences of each pair
        map<int, map<int, int>> valueDecisionCounts;
        for (const auto& instance : dataset) {
            int attrValue = instance[attrIndex];
            int decisionValue = instance[decisionAttrIndex];
            valueDecisionCounts[attrValue][decisionValue]++;
        }

        // Determine the best decision for each attribute value
        map<int, int> bestDecisionForValue;
        int errors = 0;
        for (const auto& pair : valueDecisionCounts) {
            int bestDecision = -1;
            int bestCount = -1;
            int total = 0;
            for (const auto& decisionCount : pair.second) {
                if (decisionCount.second > bestCount) {
                    bestCount = decisionCount.second;
                    bestDecision = decisionCount.first;
                }
                total += decisionCount.second;
            }
            bestDecisionForValue[pair.first] = bestDecision;
            errors += total - bestCount;
        }

        // Check if this attribute is better
        if (errors < minError) {
            minError = errors;
            bestAttribute = attributes[attrIndex].name;
            bestRules.clear();
            for (const auto& pair : bestDecisionForValue) {
                bestRules[attributes[attrIndex].values[pair.first]] = attributes[decisionAttrIndex].values[pair.second];
            }
        }
    }

    // Output the best rule
    cout << "\n***Best 1-rule***" << endl;
    cout << "\t" << bestAttribute << ":" << endl;
    for (const auto& rule : bestRules) {
        cout << "\t\t\t" << rule.first << " --> " << rule.second << endl;
    }
    cout << "\nError rate: " << minError << "/" << dataset.size() << endl;
}
