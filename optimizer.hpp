#pragma once

#include<vector>
#include<queue>
#include<set>
#include<sstream>
#include<iostream>
#include<algorithm>
#include<random>

using namespace std;

// RF - Size of a single raw filter string
// DNF - Maximum number of handled conjunctive clauses (number of OR - 1)
// NC - Number of generated cascades to be optimized
template<size_t RF, size_t DNF = 4, size_t NC = 32>
class Cascade {
    char tree_nodes[(1 << DNF) - 1][RF + 1];

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

    vector<vector<int>> optimize_raw_filters(
        const vector<vector<vector<string>>>& raw_filters, 
        const vector<string>& records, 
        vector<string>& filters, 
        vector<vector<bool>>& hits)
    {
        // Generate passthrough rates for each filter, select the best filter for each token
        vector<vector<int>> selected;

        for (auto filters_for_clause : raw_filters) {

            vector<int> best_for_clause;

            int clause_index = best_for_clause.size();

            for (auto filters_for_token : filters_for_clause) {

                if (filters_for_token.size() == 0)
                    continue;

                int best_filter = filters.size();
                int best_hit = 0;

                for (int i = 0; i < filters_for_token.size(); i++) {

                    auto filter = filters_for_token[i];
                    int hit = 0;

                    auto filter_index = filters.size();

                    filters.push_back(filter);
                    hits.emplace_back(vector<bool>(records.size()));

                    for (int j = 0; j < records.size(); j++) {
                        hits[filter_index][j] = is_substring(records[j], filter);
                        hit++;
                    }

                    if (hit > best_hit) {
                        best_filter = filter_index;
                        best_hit = hit;
                    }
                }

                best_for_clause.push_back(best_filter);
            }

            selected.emplace_back(best_for_clause);
        }

        return selected;
    };

    void generate_cascade(
        const vector<vector<int>>& selected, 
        const vector<string>& filters,
        const vector<vector<bool>>& hits)
    {
        int parent = 0;

        int levels = min(DNF, selected.size());

        vector<int> clauses;
        for (int i = 0; i < selected.size(); i++) {
            clauses.emplace_back(i);
        }

        vector<vector<int>> trees;
        for (int i = 0; i < NC; i++) {
            // Shuffle clause level order around
            random_shuffle(clauses.begin(), clauses.end());

            queue<int> clause_queue;

            for (auto clause : clauses) {
                clause_queue.emplace(clause);
            }

            vector<int> tree((1 << DNF) - 1);
            for (int j = 0; j < (1 << DNF) - 1; j++) {
                tree[j] = -1;
            }

            random_tree(0, 1, tree, clause_queue, selected);
            trees.emplace_back(tree);
        }

        // TODO: Calculate costs of every generated cascade and pick the best one

        int best = 0;

        auto best_tree = trees[best];

        // Copy the optimized cascade to the final tree

        for (int i = 0; i < (1 << DNF) - 1; i++) {
            if (best_tree[i] != -1)
            strncpy_s(&tree_nodes[i][0], sizeof tree_nodes[i], filters[best_tree[i]].c_str(), RF);
        }
    };

    // Generate a random, but valid cascade tree
    // When a filter fails, create another node checking one of the remaining clauses
    // When a filter passes, either end execution or make another node checking the same clause 
    // (but only if there are more levels remaining than the remaining number of clauses)
    void random_tree(int node, int level, vector<int> &tree, queue<int> clauses, vector<vector<int>> selected)
    {
        // If there is nothing more to add from the query, or we are outside of the tree return
        if (clauses.empty() || node >= (1 << DNF) - 1)
            return;

        int cl_idx = clauses.front();
        int opt_idx = rand() % selected[cl_idx].size();

        tree[node] = selected[cl_idx][opt_idx];

        selected[cl_idx].erase(selected[cl_idx].begin() + opt_idx);

        // If we pass...
        
        // If enough levels and tokens from the clause are remaining, we can attempt another filtering step from the same clause (or not)
        if (clauses.size() <= DNF - level && selected[cl_idx].size() > 0 && rand() % 2 == 0) {
            random_tree(node * 2 + 2, level + 1, tree, clauses, selected);
        }

        // If we fail...

        // Try another clause
        clauses.pop();
        random_tree(node * 2 + 1, level + 1, tree, clauses, selected);
    }

    bool is_substring(const string& input, const string& filter) {
        return input.find(filter) != string::npos;
    }

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

        vector<string> filters;
        vector<vector<bool>> hits;

        auto selected = optimize_raw_filters(raw_filters, records, filters, hits);

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

        generate_cascade(selected, filters, hits);
    };

    bool eval(const string& input) 
    {
        int parent = 0;

        while (parent < (1 << DNF) - 1)
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
