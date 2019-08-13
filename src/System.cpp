#include "System.hpp"

#include <iostream>
#include <sstream>
#include <vector>

System *core = new System();

System::System() {

}

void System::command(string line, ConsoleOutput *output) {
    this->m_output = output;

    transform(line.begin(), line.end(), line.begin(),
        [](unsigned char c){ return tolower(c); });

    if (line == "list") {
        for (map<int, string>::iterator it = program.begin(); it != program.end(); it++ )
        {
            m_output->addText(it->second);
        }
    } else if (line == "run") {
        lines();
    } else {
        istringstream iss(line);
        vector<string> results((istream_iterator<string>(iss)),
                                istream_iterator<string>());
        
        if (is_number(results[0])) {
            if (results.size() == 1) {
                program.erase(stoi(results[0]));
            } else {
                program[stoi(results[0])] = line;
            }
        } else if (line == "cls") {
            m_output->clearText();
        } else if (line == "bye") {
            m_output->terminate();
        } else {
            map<int, string> program_backup = program;
            program.clear();
            program[0] = line;
            lines();
            program = program_backup;
        }
    }
}

bool System::is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void System::lines() {
    for (map<int, string>::iterator it = program.begin(); it != program.end(); it++ )
    {
        cout << "Processing line: " << it->second << endl;
    }
}