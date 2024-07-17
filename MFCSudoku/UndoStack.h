#include <iostream>
#include <vector>
#include <memory>
#include <optional>

// Enumeration for different types of entries
enum class EntryType {
  ValueEntry,
  PencilMarkEntry,
  SnyderNotation,
  MarkStack
};

// Structure to hold an entry
struct Entry {
  EntryType type;
  int value; // Value of the entry
  int row; // Added row
  int col; // Added column

  Entry(int _row, int _col, EntryType _type, int _value) : row(_row), col(_col), type(_type), value(_value) {}

  // Overload the == operator to compare entries
  bool operator==(const Entry& other) const {
    return row == other.row && col == other.col && type == other.type && value == other.value;
  }
};

// UndoStack class
class UndoStack {
private:
  std::vector<Entry> undoStack;
  std::vector<Entry> redoStack; // Stack for redo operations

public:
  // Add an entry to the stack
  void addEntry(int _row, int _col, EntryType _type, int _value);
  void addEntry(Entry& _entry);

  // Undo the last entry and return the entry
  std::optional<Entry> undo();

  // Undo to the last marked stack
  std::optional<Entry> undoToMark();

  std::optional<Entry> redo();

  // Mark the stack for an undo point
  void markStack() {
    undoStack.emplace_back(-1, -1, EntryType::MarkStack, -1);
  }

  // For demonstration: Print the stack contents
  void printStack() {
    for (const auto& entry : undoStack) {
      std::cout << "Type: " << static_cast<int>(entry.type) << ", Value: " << entry.value << std::endl;
    }
  }
};

