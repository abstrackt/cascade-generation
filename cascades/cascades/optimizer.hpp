#pragma once

#include<vector>
#include<bitset>
#include<sstream>
#include<iostream>
#include<algorithm>

using namespace std;

template<size_t RF>
class Cascade {

    char tree_nodes[15][RF + 1];

    vector<vector<string>> get_clauses(const string& filter)
    {
        // TODO, plug in a proper parser here, right now we assume everything is provided in DNF form without parentheses.

        vector<vector<string>> result;
        stringstream ss(filter);

        vector<string> clause;

        stringstream sst("");

        for (string word; getline(ss, word, ' ');)
        {
            if (word == "&&" || word == "||")
            {
                // Create new token, logical operator detected
                clause.push_back(sst.str());
                sst.str("");
            }
            
            if (word == "||")
            {
                // Start a new conjunctive clause
                result.push_back(clause);
                clause.clear();
            }
            else if (word != "&&")
            {
                // Still the same token delimited by a whitespace character
                if (sst.str() != "") {
                    sst << " ";
                }
                sst << word;
            }
        }

        if (sst.str() != "")
        {
            clause.push_back(sst.str());
        }

        if (!clause.empty()) 
        {
            result.push_back(clause);
        }

        return result;
    };

    vector<vector<vector<string>>> get_raw_filters(const vector<vector<string>>& clauses)
    {
        // Generate all RFs for each conjunctive clause represented as a vector of tokens
        vector<vector<vector<string>>> filters_for_query;

        for (auto clause : clauses) {

            vector<vector<string>> filters_for_clause;

            for (auto token : clause) {

                vector<string> filters_for_token;

                if (token.empty()) 
                {
                    continue;
                }

                if (token.size() < RF)
                {
                    filters_for_token.push_back(token);
                }
                else 
                {
                    for (int i = 0; i <= token.length() - RF; i++) {
                        filters_for_token.push_back(token.substr(i, RF));
                    }
                }

                filters_for_clause.push_back(filters_for_token);
            }

            filters_for_query.push_back(filters_for_clause);
        }

        return filters_for_query;
    };

    vector<string> sample_records(const string& json, size_t n_samples)
    {
        // Split by newlines
        vector<string> lines;
        stringstream ss(json);

        for (string line; getline(ss, line, '\n');)
        {
            lines.push_back(line);
        }

        if (lines.size() <= n_samples) 
        {
            return lines;
        }
        else 
        {
            random_shuffle(lines.begin(), lines.end());
            return vector<string>(&lines[0], &lines[n_samples]);
        }
    };

    vector<vector<string>> optimize_raw_filters(const vector<vector<vector<string>>>& raw_filters, const vector<string>& records)
    {
        // TODO: Apply raw filters to the sampled records, select ones with highest passthrough for a given conjunctive clause
        vector<vector<string>> selected_for_clause;

        for (auto filters_for_clause : raw_filters) {
            if (filters_for_clause.size() > 0) {
                selected_for_clause.push_back(filters_for_clause[0]);
            }
        }

        return selected_for_clause;
    };

    void generate_cascade(const vector<vector<string>>& raw_filters)
    {
        int parent = 0;

        // TODO: Select the best cascade via method described in the paper, for now just creates a path with single element of each conjunctive clause
        
        int levels = min<uint64_t>({ raw_filters.size(), 4 });

        for (int i = 0; i < levels; i++)
        {
            strncpy_s(&tree_nodes[parent][0], sizeof tree_nodes[parent], raw_filters[i][0].c_str(), RF);
            parent = 2 * parent + 1;
        }
    };

    bool run_filter(const string& input, const string& filter)
    {
        // TODO: Connect with filtering code
        return true;
    };

public:
    Cascade(string json, string filter, size_t n_samples)
    {
        memset(tree_nodes, 0, sizeof(tree_nodes));

        auto records = sample_records(json, n_samples);
        auto clauses = get_clauses(filter);
        auto raw_filters = get_raw_filters(clauses);
        auto selected_filters = optimize_raw_filters(raw_filters, records);

        for (int i = 0; i < raw_filters.size(); i++) 
        {
            auto clause = raw_filters[i];
            cout << "RFs for clause " << i << ": " << endl;
            for (int j = 0; j < clause.size(); j++)
            {
                auto token = raw_filters[i][j];
                cout << "  RFs for token " << clauses[i][j] << ": " << endl;
                for (auto rf : token) 
                {
                    cout << "    " << rf << endl;
                }
            }
        }
        
        cout << endl << "Records:" << endl;

        for (auto record : records) 
        {
            cout << record << endl;
        }

        generate_cascade(selected_filters);
    };

    bool eval(const string& input) 
    {
        int parent = 0;

        while (parent < 15) 
        {
            auto filter = string(tree_nodes[parent]);
            if (run_filter(input, filter)) 
            {
                // If right child is empty or out of range we have passed the cascade.
                int right = parent * 2 + 2;
                    if (right >= 15 || tree_nodes[right][0] == 0) {
                        return true;
                    }
                parent = right;
            }
            else
            {
                // If left child is empty or out of range we have failed the cascade.
                int left = parent * 2 + 1;
                if (left >= 15 || tree_nodes[left][0] == 0) {
                    return false;
                }
                parent = left;
            }
        }
        
        return false;
    }
};
