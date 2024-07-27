#include "pch.h"
#include "../MFCSudoku/UndoStack.h"

TEST(UndoStackTests, Undo) {
  UndoStack stack;
  UndoEntry entry1(1, 1, EntryType::ValueEntry, 5);
  stack.addEntry(entry1);
  UndoEntry entry2(2, 2, EntryType::ValueEntry, 6);
  stack.addEntry(entry2);

  auto result = stack.undo();
  EXPECT_EQ(entry2, result.value());

  result = stack.undo();
  EXPECT_EQ(entry1, result.value());
}

TEST(UndoStackTests, Redo) {
  UndoStack stack;
  UndoEntry entry1(1, 1, EntryType::ValueEntry, 5);
  stack.addEntry(entry1);
  UndoEntry entry2(2, 2, EntryType::ValueEntry, 6);
  stack.addEntry(entry2);

  auto result = stack.undo();
  EXPECT_EQ(entry2, result.value());

  stack.redo();

  result = stack.undo();
  EXPECT_EQ(entry2, result.value());

  // Added two and removed one, redo should put back the last one
  result = stack.undo();
  EXPECT_EQ(entry1, result.value());

  result = stack.undo();
  EXPECT_EQ(std::nullopt, result);
}

TEST(UndoStackTests, UndoToMark) {
  UndoStack stack;
  UndoEntry entry1(1, 1, EntryType::ValueEntry, 5);
  stack.addEntry(entry1);
  UndoEntry entry2(2, 2, EntryType::ValueEntry, 6);
  stack.addEntry(entry2);

  stack.markStack();

  UndoEntry entry3(3, 3, EntryType::ValueEntry, 7);
  stack.addEntry(entry3);

  auto result = stack.undoToMark();
  EXPECT_EQ(entry2, result.value());

  result = stack.undo();
  EXPECT_EQ(entry1, result.value());

  stack.redo();
  stack.redo();
  stack.redo();
  result = stack.undo();
  EXPECT_EQ(entry3, result.value());
}