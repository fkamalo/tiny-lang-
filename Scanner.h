//
// Created by farah on 20-Nov-24.
//

#ifndef SCANNER_H
#define SCANNER_H



#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>

class Scanner {
private:
    std::unordered_map<std::string, bool> STATES = {
        {"START", false}, {"IN_COMMENT", false}, {"IN_IDENTIFIER", false},
        {"IN_NUMBER", false}, {"IN_ASSIGNMENT", false}, {"DONE", false}, {"OTHER", false}
    };

    std::vector<std::pair<std::string, std::string>> tokens;
    bool state_other = false;

    std::vector<std::string> KEYWORDS = {"else", "end", "if", "repeat", "then", "until", "read", "write"};
    std::unordered_map<std::string, std::string> OPERATORS = {
        {"+", "PLUS"}, {"-", "MINUS"}, {"*", "MULT"}, {"/", "DIV_FLOAT"},
        {":", "COLON"}, {"=", "EQUALS"}, {":=", "ASSIGNMENT"},
        {">", "GREATER"}, {"<", "LESS"}, {";", "SEMICOLON"},
        {"(", "OPEN_PARENTHESIS"}, {")", "CLOSE_PARENTHESIS"}
    };

    void set_state(const std::string& state) {
        for (auto& pair : STATES) pair.second = false;
        STATES[state] = true;
    }

    bool get_state(const std::string& state) const {
        auto it = STATES.find(state);
        return it != STATES.end() ? it->second : false;
    }

    bool is_str(const std::string& token) {
        return std::all_of(token.begin(), token.end(), [](char c) { return std::isalpha(c); });
    }

    bool is_num(const std::string& token) {
        return std::all_of(token.begin(), token.end(), [](char c) { return std::isdigit(c); });
    }

    bool is_col(char c) {
        return c == ':';
    }

    bool is_symbol(const std::string& token) {
        std::string symbols = "+-*/=<>();";
        return token.size() == 1 && symbols.find(token) != std::string::npos;
    }

    bool is_comment(const std::string& token) {
        return token.size() > 1 && token.front() == '{' && token.back() == '}';
    }

    std::string read_file(const std::string& fileName) {
        std::ifstream file(fileName);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content + " ";
    }

    void classify(const std::string& token) {
        std::string trimmed_token = token;
        if (!trimmed_token.empty() && trimmed_token.back() == ' ')
            trimmed_token.pop_back();

        if (is_str(trimmed_token)) {
            if (std::find(KEYWORDS.begin(), KEYWORDS.end(), trimmed_token) != KEYWORDS.end()) {
                tokens.emplace_back(trimmed_token, trimmed_token);
            } else {
                tokens.emplace_back(trimmed_token, "IDENTIFIER");
            }
        } else if (is_num(trimmed_token)) {
            tokens.emplace_back(trimmed_token, "NUMBER");
        } else if (OPERATORS.find(trimmed_token) != OPERATORS.end()) {
            tokens.emplace_back(trimmed_token, OPERATORS[trimmed_token]);
        } else if (is_comment(trimmed_token)) {
            tokens.emplace_back(trimmed_token, "COMMENT");
        }
    }

public:
    void scan(const std::string& input_file = "input.txt") {
        std::string input_text = read_file(input_file);
        std::string token;
        for (char c : input_text) {
            if (get_state("START")) {
                if (is_symbol(std::string(1, c))) {
                    set_state("DONE");
                } else if (c == ' ') {
                    set_state("START");
                    continue;
                } else if (c == '{') {
                    set_state("IN_COMMENT");
                } else if (std::isdigit(c)) {
                    set_state("IN_NUMBER");
                } else if (std::isalpha(c)) {
                    set_state("IN_IDENTIFIER");
                } else if (is_col(c)) {
                    set_state("IN_ASSIGNMENT");
                }
            } else if (get_state("IN_COMMENT")) {
                if (c == '}') {
                    set_state("DONE");
                } else {
                    set_state("IN_COMMENT");
                }
            } else if (get_state("IN_NUMBER")) {
                if (std::isdigit(c)) {
                    set_state("IN_NUMBER");
                } else if (c == ' ') {
                    set_state("DONE");
                } else {
                    set_state("OTHER");
                }
            } else if (get_state("IN_IDENTIFIER")) {
                if (std::isalpha(c)) {
                    set_state("IN_IDENTIFIER");
                } else if (c == ' ') {
                    set_state("DONE");
                } else {
                    set_state("OTHER");
                }
            } else if (get_state("IN_ASSIGNMENT")) {
                if (c == '=') {
                    set_state("DONE");
                } else {
                    set_state("OTHER");
                }
            }

            if (!get_state("OTHER")) {
                token += c;
            }

            if (get_state("OTHER")) {
                set_state("DONE");
                state_other = true;
            }

            if (get_state("DONE")) {
                classify(token);
                if (state_other) {
                    token = std::string(1, c);
                    if (is_col(c)) set_state("IN_ASSIGNMENT");
                    else if (c == '{') set_state("IN_COMMENT");
                    else if (std::isdigit(c)) set_state("IN_NUMBER");
                    else if (std::isalpha(c)) set_state("IN_IDENTIFIER");
                    else if (is_symbol(std::string(1, c))) {
                        classify(std::string(1, c));
                        token = "";
                        set_state("START");
                    }
                    state_other = false;
                } else {
                    token = "";
                }
                set_state("START");
            }
        }
    }

    void output() {
        std::ofstream file("output.txt");
        file << std::left << "Type          Token\n";
        file << "====          =====\n";
        for (const auto& token : tokens) {
            file << token.second << "   " << token.first << "\n";
        }
        file.close();
    }
};



#endif //SCANNER_H
