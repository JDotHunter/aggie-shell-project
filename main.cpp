/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <unistd.h>   // getcwd (Linux/Mac)
#include <stdlib.h>   // free, perror
#include <sstream>
#include <dirent.h>   // Required for directory listing
#include <sys/stat.h> // Required for stat()
#include <sys/types.h>
#include <errno.h>
#include <algorithm>
#include <cctype>
#include <vector>
#include <string>
#include <fstream>
#include <string.h>
 
using namespace std;

string trimString(const string& str) {
    // finds the first non-space character from the start of the string (if there is one).
    auto start = find_if_not(str.begin(), str.end(), ::isspace);
    // finds the first non-space character from the end of the string (if there is one),
    // and converts it to a normal iterator using .base()
    auto end = find_if_not(str.rbegin(), str.rend(), ::isspace).base();
    // if there is something other than white space, return the trimmed version of the word.
    // if not, that means either the string is all white space or is empty, so return an empty string.
    return (start < end ? string(start, end) : string());
}

vector<string> stringSplitter(const string& str, char delimiter) {
    vector<string> tokens; // a vector where we'll store the parts split by the delimiter.
    size_t startPos = 0;
    size_t end = str.find(delimiter);

    // string::npos means that no match was found when searching through the string.
    // what the loop is doing is searching for the delimiter character within the input string.
    // this function is built specifically to handle multiple redirections but that isn't allowed in this project.
    // An error handler will address this.
    while (end != string::npos) {
        // push the input before the first delimiter.
        tokens.push_back(str.substr(startPos, end - startPos));
        // resets the start position to be after the first delimiter so that we can find any other potential delimiters.
        startPos = end + 1;
        // finds the position where the second delimiter is located.
        end = str.find(delimiter, startPos);
    }
    // pushes everything that comes after the last delimiter into the vector.
    tokens.push_back(str.substr(startPos));
    return tokens;
}

vector<string> pathList;

// for CheckPoint 4. istream handles both cin and file streams.
void processInput(istream& input) {
    string userInput;
    // passes each line from the input to our userInput variable.
    while (getline(input, userInput)) {
        userInput = trimString(userInput);

        char buffer[1024]; // getcwd pre-allocates the buffer versus _getcwd which forces you to allocate it dynamically.
        if (getcwd(buffer, sizeof(buffer)) == NULL) {
            perror("getcwd error");
        } else {
            // check to see if any directories exist in /home
            DIR* dir = opendir("/home"); // opens up /home directory for reading

            // if /home was opened successfully, proceed.
            if (dir) {
                struct dirent* entry; // a pointer that will store directory entries as they're read.
                cout << "Contents of /home directory: " << endl;
                // will loop and print entries until there are no more entries left to read.
                while ((entry = readdir(dir)) != NULL) {
                    cout << entry->d_name << endl;
                }
                closedir(dir); // after opendir(dir) to open up the directory stream, close it using closedir(dir).
            } else {
                cout << "Could not open /home directory:(" << endl; // handles the case when the directory isn't opened.
            }

            // creates a var source of type struct stat. source will hold information about the file/directory.
            struct stat source;
            // checks if source code exists.
            if (stat("source code", &source) == 0) {
                // checks what type of thing source code is. Specifically source.st_mode contains bit flags about source.
                // S_ISDIR is a macro that'll return true if the bit flags say source code is a directory.
                if (S_ISDIR(source.st_mode)) {
                    cout << "\"source code\" is a directory :)" << endl;
                } else {
                    cout << "\"source code\" exists but is not a directory." << endl;
                }
            } else {
                cout << "The path: \"source code\" does not exist." << endl;
            }

            // Project Checkpoints 1, 2, & 3 code.
            cout << "ash " << buffer << " > ";
            vector<string> tokens = stringSplitter(userInput, '>');

            // checks if there are multiple redirection symbols.
            if (tokens.size() > 2) {
                cout << "Multiple redirections are not allowed :)" << endl;
                continue;
            }

            if (tokens.size() == 1) {
                cout << "There is no redirection occurring. Proceed as normal." << endl;
            }

            if (tokens.size() == 2) {
                string commandPart = trimString(tokens[0]);
                string filePart = trimString(tokens[1]);
                stringstream ss(commandPart);
                string command, argument;
                ss >> command;          // Extracts "cd" or "path"
                getline(ss, argument);  // gets everything after the command.
                argument = trimString(argument);

                // redirection logic
                streambuf* originalBuffer = cout.rdbuf(); // save the original cout destination so you can go back later
                // creates a file with the string filePart as the filename.
                ofstream fileOut(filePart);
                // redirects cout to write to the filePart.
                cout.rdbuf(fileOut.rdbuf());

                if (command == "cd") {
                    if (argument == "") {
                        cout << "Error! Please add a directory as well." << endl;
                    } else {
                        if (chdir(argument.c_str()) == 0) {
                            cout << "Directory changed to: " << argument << endl;
                        } else {
                            perror("chdir error :(");
                        }
                    }
                    // resets so that cout isn't redirecting to the new file anymore.
                    fileOut.close();
                    cout.rdbuf(originalBuffer);
                }

                if (command == "exit") {
                    cout << "Jaylan Hunter" << endl;
                    cout << "Aggie Shell Project" << endl;
                    cout.rdbuf(originalBuffer); // restore output before exiting.
                    exit(0);
                }

                if (command == "path") {
                    pathList.clear(); // reset the path so the user can start fresh.
                    stringstream ss(argument);
                    string path;
                    // passes each word into the path variable
                    while (ss >> path) {
                        // push each word into the vector one by one.
                        pathList.push_back(path);
                    }
                    // iterates through each word of the vector and prints them out.
                    for (const string& strPath : pathList) {
                        cout << "[DEBUG] path added: " << strPath << endl;
                    }
                }
                continue;
            }

            // non-redirection logic
            stringstream ss(userInput);
            string command, directory;
            ss >> command;           // Extract "command such as cd, path, mkdir, etc."
            getline(ss, directory);  // Gets everything after the command.
            directory = trimString(directory);

            if (command == "cd") {
                if (directory == "") {
                    cout << "Error! Please add a directory as well." << endl;
                } else {
                    if (chdir(directory.c_str()) == 0) {
                        cout << "Directory changed to: " << directory << endl;
                    } else {
                        perror("chdir error!");
                    }
                }
            }

            if (command == "exit") {
                cout << "Jaylan Hunter" << endl;
                cout << "Aggie Shell Project" << endl;
                exit(0);
            } else {
                cout << "You've typed: " << userInput << endl;
            }

            if (command == "path") {
                pathList.clear(); // reset the path so the user can start fresh.
                stringstream ss(userInput);
                string command, path;
                ss >> command; // extract "path" to skip it which prevents the word "path" from being pushed into the vector.

                // read the rest of the input that follows the path command.
                while (ss >> path) {
                    // push the input into the vector.
                    pathList.push_back(path);
                }

                // iterates through each word within the vector (the rest of the input) & prints it out.
                for (const string& strPath : pathList) {
                    cout << "[DEBUG] path added: " << strPath << endl;
                }
            }
            
            //mkdir is a linux command that creates a new directory.
            if(command == "mkdir"){
                //mkdir() is a function that accepts C-style strings, not C++ std::string.
                //.c_str() converts our C++ string (directory) into a const char* for compatibility.
                const char* directoryName = directory.c_str();
                //mkdir will return 0 on success, -1 on failure (e.g., empty name, already exists, improper permissions).
                //strerror(errno) translates the system error message into a human-readable format.
                if(mkdir(directoryName, 0777) == -1){
                    cerr << "Error : " << strerror(errno) << endl; 
                }else{
                    cout << "Directory created successfully!" << endl;
                }
            }
        }
    }
}

int main() {
    cout << "Decide the input mode(1 for cin, 2 for files): " << endl;
    int choice;
    cin >> choice;
    cin.ignore(); // after reading the choice, clear the remaining newline character for stylistic purposes.

    // cin case
    if (choice == 1) {
        // passes input provided by the user to the processInput function.
        processInput(cin);
    }
    // file stream case
    else if (choice == 2) {
        // creates a new file that we can read from.
        ifstream batchFile("test_file.txt");
        // error handler if we can't open the file.
        if (!batchFile) {
            cerr << "Unable to open test_file.txt :(";
            return 1;
        }
        // passes the file that we've created, which will then allow processInput to read our file line by line.
        processInput(batchFile);
    }
    // case if user enters in an invalid input.
    else {
        cerr << "Inavlid choice. Please enter 1 or 2." << endl;
        return 1;
    }
    return 0;
}