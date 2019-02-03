#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template<typename Type>
class LinkedList
{
public:
	using difference_type = std::ptrdiff_t;
	using size_type = std::size_t;
	using value_type = Type;
	using pointer = Type *;
	using reference = Type &;
	using const_pointer = const Type *;
	using const_reference = const Type &;

	class ConstIterator;

	class Iterator;

	using iterator = Iterator;
	using const_iterator = ConstIterator;

	class Node;

private:
	size_t size = 0;
	Node *sentinel; //this is our good guy, he`s both head and tail

	void init()
	{
		sentinel = new Node();
		sentinel->next = sentinel;
		sentinel->prev = sentinel;
	}

	void clearData()
	{
		Node *temp ;
		Node *currentNode = sentinel->next;

		while (currentNode != sentinel) //delete everything except sentinel
		{
			temp = currentNode;
			currentNode = currentNode->next;
			delete temp;
		}

		size=0;
		sentinel->next = sentinel; //sentinel points to himself
		sentinel->prev = sentinel;
	}

	void move(LinkedList &other)
	{
		clearData();
		delete sentinel;

		size = other.size;
		sentinel = other.sentinel; //taking other sentinel and giving him our

		other.size = 0;
		other.sentinel = nullptr;
	}

public:

	LinkedList()
	{
		init();
	}

	LinkedList(std::initializer_list<Type> l)
	{
		init();
		for (auto elem : l)
			append(elem);
	}

	LinkedList(const LinkedList &other)
	{
		init();
		for (auto elem : other)
			append(elem);
	}

	LinkedList(LinkedList &&other)
	{
		init(); //init sentinel for swap with other`s sentinel
		move(other); //swap sentinels and size
	}

	~LinkedList()
	{
		if(size != 0)
			clearData();
		delete sentinel;
	}

	LinkedList &operator=(const LinkedList &other)
	{
		if(this != &other)
		{
			clearData();
			delete sentinel;
			init();

			for (auto elem : other)
				append(elem);
		}

		return *this;
	}

	LinkedList &operator=(LinkedList &&other)
	{
		if(this != &other)
		{
			clearData();
			move(other);
		}
		return *this;
	}

	bool isEmpty() const
	{
		return size == 0;
	}

	size_type getSize() const
	{
		return size;
	}

	void append(const Type &item)
	{
		if (size == 0)
		{
			prepend(item);
			return;
		}

		//sentinel->prev is the last element

		Node *temp = new Node(item, sentinel->prev, sentinel);
		sentinel->prev->next = temp;
		sentinel->prev = temp;
		++size;
	}


	void prepend(const Type &item)
	{
		Node *temp = new Node(item, sentinel, sentinel->next);

		//sentinel->next is first element

		sentinel->next->prev = temp;
		sentinel->next = temp;
		++size;
	}

	void insert(const const_iterator &insertPosition, const Type &item)
	{
		if(insertPosition == cbegin())
		{
			prepend(item);
			return;
		}
		if(insertPosition == cend())
		{
			append(item);
			return;
		}

		Node* insertNode = insertPosition.getCurrent();
		Node* temp = sentinel;

		while(temp->next != insertNode) //find proper Node to insert a new one as his next
			temp = temp->next;

		Node *toInsert = new Node(item, temp, temp->next);
		temp->next = toInsert;
		toInsert->next->prev = toInsert;
		++size;

	}

	Type popFirst()
	{
		if(size == 0)
			throw std::logic_error("");

		Node *toDelete = sentinel->next;
		value_type retValue = toDelete->value;

		sentinel->next = toDelete->next; //change first element
		toDelete->next->prev = sentinel; //new one->prev should points to sentinel

		delete toDelete;
		--size;

		return retValue;

	}

	Type popLast()
	{
		if(size == 0)
			throw std::logic_error("");
		if(size == 1)
			return popFirst();

		Node *toDelete = sentinel->prev;
		value_type retValue = toDelete->value;

		toDelete->prev->next = sentinel; //last->next points to sentinel
		toDelete->next->prev = toDelete->prev; //sentinel->prev points to last element

		delete toDelete;
		--size;

		return retValue;


	}

	void erase(const const_iterator &position)
	{
		if(size == 0 || position==cend())
			throw std::out_of_range("");

		if(position == cbegin())
		{
			popFirst();
			return;
		}
		if(position == --cend())
		{
			popLast();
			return;
		}

		Node* erasePosition = position.getCurrent();
		Node* temp = sentinel->next;

		while(temp != erasePosition)
			temp = temp->next;

		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		--size;

		delete erasePosition;
	}

	void erase(const const_iterator &firstIncluded, const const_iterator &lastExcluded)
	{
		if(firstIncluded.getCurrent() == lastExcluded.getCurrent())
		{
			return;
		}
		//if(firstIncluded.getCurrent() == sentinel && firstIncluded.getCurrent() != lastExcluded.getCurrent())

		Node *startErase = firstIncluded.getCurrent();
		Node *first = sentinel->next;
		Node *last;

		while(first != startErase)
			first = first->next;

		last = first->next;

		while(last->next != lastExcluded.getCurrent())
			last = last->next;

		first->prev->next = last->next;
		last->next->prev = first->prev;
		last->next = nullptr;

		while(first != nullptr)
		{
			auto temp = first;
			first = first->next;
			--size;
			delete temp;
		}

	}

	iterator begin()
	{
		return Iterator(cbegin());
	}

	iterator end()
	{
		return  Iterator(cend());
	}

	const_iterator cbegin() const
	{
		return ConstIterator(this, sentinel->next);
	}

	const_iterator cend() const
	{
		return ConstIterator(this, sentinel);
	}

	const_iterator begin() const
	{
		return cbegin();
	}

	const_iterator end() const
	{
		return cend();
	}
};

template<typename Type>
class LinkedList<Type>::Node
{

public:
	value_type value = 0;
	Node *prev = nullptr;
	Node *next = nullptr;

	Node(const value_type &value, Node *prev = nullptr, Node *next = nullptr) : value(value), prev(prev), next(next)
	{}

	Node(Node *prev = nullptr, Node *next = nullptr) : prev(prev), next(next)
	{}

	bool operator==(const Node &rhs) const
	{
		return value == rhs.value &&
		       prev == rhs.prev &&
		       next == rhs.next;
	}

	bool operator!=(const Node &rhs) const
	{
		return !(rhs == *this);
	}

};

template<typename Type>
class LinkedList<Type>::ConstIterator
{
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename LinkedList::value_type;
	using difference_type = typename LinkedList::difference_type;
	using pointer = typename LinkedList::const_pointer;
	using reference = typename LinkedList::const_reference;

private:
	const LinkedList<Type> *list;
	Node *current;

public:

	const aisdi::LinkedList<Type> *getList() const
	{
		return list;
	}

	Node* getCurrent() const
	{
		return current;
	}
	explicit ConstIterator(const LinkedList<Type> *list = nullptr, Node *current = nullptr):
	list(list), current(current)
	{}

	reference operator*() const
	{
		if(*this == list->cend())
			throw std::out_of_range("Cannot dereference from sentinel");

		return current->value;
	}

	ConstIterator &operator++()
	{
		if(*this == list->cend())
			throw std::out_of_range("Cannot increment");

		current = current->next;
		return *this;
	}

	ConstIterator operator++(int)
	{
		if(*this == list->cend())
			throw std::out_of_range("Cannot increment");

		auto beforeIncrement = ConstIterator(list, current);
		current = current->next;

		return beforeIncrement;
	}

	ConstIterator &operator--()
	{
		if(*this == list->cbegin())
			throw std::out_of_range("");

		current = current->prev;
		return *this;
	}

	ConstIterator operator--(int)
	{
		if(*this == list->cbegin())
			throw std::out_of_range("");

		auto beforeDecrement = ConstIterator(list, current);
		current = current->prev;

		return beforeDecrement;
	}

	ConstIterator operator+(difference_type d) const
	{
		ConstIterator iterator = *this;

		for(int i=0; i<std::abs(d); ++i)
			++iterator;

		return iterator;
	}

	ConstIterator operator-(difference_type d) const
	{
		ConstIterator iterator = *this;

		for(int i=std::abs(d); i>0; --i)
			--iterator;

		return iterator;

	}

	bool operator==(const ConstIterator &other) const
	{
		return list == other.list && current == other.current;
	}

	bool operator!=(const ConstIterator &other) const
	{
		return !operator==(other);
	}
};

template<typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
{
public:
	using pointer = typename LinkedList::pointer;
	using reference = typename LinkedList::reference;

	explicit Iterator()
	{}

	Iterator(const ConstIterator &other)
			: ConstIterator(other)
	{}

	Iterator &operator++()
	{
		ConstIterator::operator++();
		return *this;
	}

	Iterator operator++(int)
	{
		auto result = *this;
		ConstIterator::operator++();
		return result;
	}

	Iterator &operator--()
	{
		ConstIterator::operator--();
		return *this;
	}

	Iterator operator--(int)
	{
		auto result = *this;
		ConstIterator::operator--();
		return result;
	}

	Iterator operator+(difference_type d) const
	{
		return ConstIterator::operator+(d);
	}

	Iterator operator-(difference_type d) const
	{
		return ConstIterator::operator-(d);
	}

	reference operator*() const
	{
		// ugly cast, yet reduces code duplication.
		return const_cast<reference>(ConstIterator::operator*());
	}
};

}

#endif // AISDI_LINEAR_LINKEDLIST_H
