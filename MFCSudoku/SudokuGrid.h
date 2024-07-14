#pragma once

class SudokuGrid
{
	struct BlockValues
	{
		// Assuming a 32 bit unsigned int
		unsigned int _pencilMark : 9;		// 9 bits for pencil marks 1-9
		unsigned int _snyderNotation : 9;	// 9 bits for snyder notation 1-9
		unsigned int _value : 4;			// 4 bits for the value 1-9
		unsigned int _reserved : 10;		// 10 bits reserved for future use

		CRect _rect;
	};

	BlockValues pencilMarks[9][9] = { 0 };

public:
	// Get the value of a cell
	int getValue(int row, int column)
	{
		return pencilMarks[row][column]._value;
	}

	// Get the pencil mark value of a cell
	int getPencilMarkValue(int row, int column, int pencilMark)
	{
		return pencilMarks[row][column]._pencilMark;
	}

	// Get the rectangle for a cell
	CRect& getRect(int row, int column)
	{
		return pencilMarks[row][column]._rect;
	}

	// Set the value of a cell
	void setValue(int row, int column, int value)
	{
		pencilMarks[row][column]._value = (unsigned int)value;
	}

	// Set the pencil mark value of a cell
	void setPencilMarkValue(int row, int column, int value)
	{
		pencilMarks[row][column]._pencilMark = 1u << value;
	}

};

