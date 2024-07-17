#include "pch.h"
#include "UndoStack.h"

void UndoStack::addEntry(int _row, int _col, EntryType _type, int _value)
{
  undoStack.emplace_back(_row, _col, _type, _value);
  redoStack.clear(); // Clear redoStack whenever a new entry is added
}

void UndoStack::addEntry(Entry& _entry)
{
  undoStack.emplace_back(_entry);
  redoStack.clear(); // Clear redoStack whenever a new entry is added
}

std::optional<Entry> UndoStack::undo()
{
  if (!undoStack.empty()) {
    Entry entry = undoStack.back();
    redoStack.emplace_back(entry);
    undoStack.pop_back();
    return entry;
  }
  return {}; // Return a default entry if stack is empty
}

std::optional<Entry> UndoStack::undoToMark()
{
  while (!undoStack.empty() && undoStack.back().type != EntryType::MarkStack) {
    redoStack.emplace_back(undoStack.back());
    undoStack.pop_back();
  }
  if (undoStack.empty()) {
    return {}; // Return a default entry if stack is empty
  }

  //We are at the mark stack entry, remove it and return the next entry
  redoStack.emplace_back(undoStack.back());
  undoStack.pop_back();

  Entry entry = undoStack.back();
  redoStack.emplace_back(entry);
  undoStack.pop_back(); // Remove the mark stack entry
  return entry;
}

std::optional<Entry> UndoStack::redo()
{
  if (!redoStack.empty()) {
    Entry redoneEntry = redoStack.back();
    undoStack.push_back(redoneEntry); // Move the entry back to the main stack
    redoStack.pop_back();
    // if the redone entry is a mark stack entry, we need to get the next entry
    if (redoneEntry.type == EntryType::MarkStack) {
      auto retVal = redo();
      if (retVal.has_value()) {
        redoneEntry = retVal.value();
      }
    }
    return redoneEntry;
  }
  return {}; // Return an empty optional if there's nothing to redo
}
