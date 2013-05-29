#pragma once

#include <vector>
#include "BatchPointer.h"

// Interface class used to provide a common
// access pattern for all Batch-templates
class IBatch
{
public:
	virtual ~IBatch(){}
};

// Class storing items in batched manner
template <class T>
class Batch : public IBatch
{
private:
	int m_itemCount;
	std::vector<T> m_item_list;
	std::vector<bool> m_gap_list;
	int m_index_firstGap;
	int m_index_lastGap;
	int m_index_next;

public:
	Batch()
	{
		m_itemCount = 0;
		m_index_firstGap = 0;
		m_index_lastGap = 0;

		resetIndex();
	}
	// Index of the last gap in the vector
	int* lastGapPtr()
	{
		return &m_index_lastGap;
	}
	int indexCount()
	{
		return m_index_lastGap;
	}
	std::vector<T>* itemList()
	{
		return &m_item_list;
	}
	int nextAvailableIndex()
	{
		return m_index_firstGap;
	}
	int itemCount()
	{
		return m_itemCount;
	}
	int* itemCountPtr()
	{
		return &m_itemCount;
	}
	T* itemAt(int p_index)
	{
		return &m_item_list.at(p_index);
	}
	void removeItemAt(int p_index)
	{
		if(m_gap_list[p_index] == false)
		{
			// Remove item
			m_gap_list[p_index] = true;
			m_itemCount--;

			// Find new first gap
			if(p_index < m_index_firstGap)
				m_index_firstGap = p_index;

			// Find new last gap
			if(p_index+1 == m_index_lastGap)
			{
				m_index_lastGap--;
				findNextLastGap();
			}
		}
	}

	bool isGap(int p_index)
	{
		return m_gap_list[p_index];
	}

	bool hasGaps()
	{
		return m_index_firstGap < m_index_lastGap;
	}

	void findNextGap()
	{
		while(m_index_firstGap < (int)m_gap_list.size() && !isGap(m_index_firstGap))
		{
			m_index_firstGap++;
		}
	}

	void findNextLastGap()
	{
		while(m_index_lastGap > 0 && isGap(m_index_lastGap-1))
		{
			m_index_lastGap--;
		}
	}
	
	// Stores the item.
	// RETURN: The index where item was stored.
	int addItem(T& p_item)
	{
		// Reuse index, if we have empty gaps.
		// ELSE: Extend array to make room for
		// new item
		int itemIndex = m_index_firstGap;
		if(itemIndex == m_item_list.size())
		{
			m_item_list.push_back(p_item);
			m_gap_list.push_back(false);
		}
		else
		{
			m_item_list[itemIndex] = p_item;
			m_gap_list[itemIndex] = false;
		}
		m_itemCount++;
		m_index_firstGap++;
		findNextGap();
		
		// Check if we have a new "last item"
		if(itemIndex == m_index_lastGap)
			m_index_lastGap = itemIndex+1;

		return itemIndex;
	}

	void overwrite(T& p_item, int p_index)
	{
		// HACK: Fix index problem
		if(p_index >= (int)m_item_list.size())
		{
			while(p_index >= (int)m_item_list.size())
			{
				m_item_list.push_back(p_item);
				m_gap_list.push_back(true);
			}
		}
		
		int tmp = m_index_firstGap;
		removeItemAt(p_index);
		m_index_firstGap = p_index;
		addItem(p_item);
		m_index_firstGap = tmp;
	}

	bool hasNext()
	{
		// Step to next Item or until end is reached
		while(m_index_next < m_index_lastGap && m_gap_list[m_index_next] == true)
		{
			m_index_next++;
		}

		// Returns TRUE if next Item is valid, otherwise end has been reached
		if(m_index_next < m_index_lastGap)
		{
			return true;
		}
		else
		{
			// Reset index
			resetIndex();
			return false;
		}
	}

	/**
	Fetch current item, and step to next item 
	*/
	T* next()
	{
		m_index_next++;
		return &m_item_list[currentIndex()];
	}

	int currentIndex()
	{
		int indexCurrent = m_index_next-1;
		return indexCurrent;
	}

	void resetIndex()
	{
		m_index_next = 0;
	}
};
