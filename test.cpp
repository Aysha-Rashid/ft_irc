#include <iostream>
#include <vector>
#include <string>
#include <algorithm>  // Needed for std::find_if

// ✅ Define a function pointer type for command handlers
typedef void (*CommandHandler)(const std::string& name);

// ✅ Structure to hold command data
struct Command {
    std::string label;       // Command name
    CommandHandler handler;  // Function pointer

    // ✅ C++98 Constructor (because `{}` is not supported)
    Command(const std::string& lbl, CommandHandler hnd) : label(lbl), handler(hnd) {}
};

class CommandProcessor {
private:
    std::vector<Command> commands;  // ✅ Store commands in a vector

public:
    CommandProcessor();  // Constructor

    // Function to find a command by name
    Command* getCommand(const std::string& commandLabel);
    
    // Function to execute a command
    void executeCommand(const std::string& commandLabel, const std::string& param);
};


// ✅ Function that prints a greeting
void sayHello(const std::string& name) {
    std::cout << "Hello, " << name << "!" << std::endl;
}

// ✅ Function that says goodbye
void sayGoodbye(const std::string& name) {
    std::cout << "Goodbye, " << name << "!" << std::endl;
}
// ✅ Constructor to initialize commands
// ✅ Constructor to initialize commands
CommandProcessor::CommandProcessor() {
    commands.push_back(Command("HELLO", sayHello));    // ✅ Use constructor (C++98 compatible)
    commands.push_back(Command("GOODBYE", sayGoodbye));
}

// ✅ Function to find a command in the vector (C++98 way)
Command* CommandProcessor::getCommand(const std::string& commandLabel) {
    for (std::vector<Command>::iterator it = commands.begin(); it != commands.end(); ++it) {
        if (it->label == commandLabel) {
            return &(*it);  // ✅ Return pointer to the found command
        }
    }
    return NULL;  // ❌ Command not found
}

// ✅ Function to execute a command if it exists
void CommandProcessor::executeCommand(const std::string& commandLabel, const std::string& param) {
    Command* cmd = getCommand(commandLabel);
    if (cmd) {
        cmd->handler(param);  // ✅ Call function pointer
    } else {
        std::cout << "Unknown command: " << commandLabel << std::endl;
    }
}

int main() {
    CommandProcessor processor;  // ✅ Create CommandProcessor object

    processor.executeCommand("HELLO", "Alice");    // Output: Hello, Alice!
    processor.executeCommand("GOODBYE", "Bob");   // Output: Goodbye, Bob!
    processor.executeCommand("INVALID", "Eve");   // Output: Unknown command: INVALID

    return 0;
}
