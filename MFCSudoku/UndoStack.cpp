#include "pch.h"
#include "UndoStack.h"

void UndoStack::addEntry(int _row, int _col, EntryType _type, int _value)
{
  stack.emplace_back(_row, _col, _type, _value);
  redoStack.clear(); // Clear redoStack whenever a new entry is added
}

void UndoStack::addEntry(Entry& _entry)
{
  stack.emplace_back(_entry);
  redoStack.clear(); // Clear redoStack whenever a new entry is added
}

std::optional<Entry> UndoStack::undo()
{
  if (!stack.empty()) {
    Entry entry = stack.back();
    redoStack.emplace_back(entry);
    stack.pop_back();
    return entry;
  }
  return {}; // Return a default entry if stack is empty
}

std::optional<Entry> UndoStack::undoToMark()
{
  while (!stack.empty() && stack.back().type != EntryType::MarkStack) {
    redoStack.emplace_back(stack.back());
    stack.pop_back();
  }
  if (!stack.empty()) {
    //We are at the mark stack entry, remove it and return the next entry
    redoStack.emplace_back(stack.back());
    stack.pop_back();

    Entry entry = stack.back();
    stack.pop_back(); // Remove the mark stack entry
    return entry;
  }
  return {}; // Return a default entry if stack is empty
}

std::optional<Entry> UndoStack::redo()
{
  if (!redoStack.empty()) {
    Entry redoneEntry = redoStack.back();
    stack.push_back(redoneEntry); // Move the entry back to the main stack
    redoStack.pop_back();
    return redoneEntry;
  }
  return {}; // Return an empty optional if there's nothing to redo
}
