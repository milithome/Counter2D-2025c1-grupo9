#ifndef EDITOR_H
#define EDITOR_H

class Editor {
public:
    Editor();  // Constructor

    void start(); // Method to start the editor
    void stop();  // Method to stop the editor

private:
    bool isRunning; // State of the editor
};

#endif // EDITOR_H