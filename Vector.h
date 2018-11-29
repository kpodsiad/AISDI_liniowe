#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{
template<typename Type>
class Vector
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

private:
	pointer elemArray = nullptr;
	size_t maxCapacity = 0;
	const size_t BEGINNINING_CAPACITY = 16;
	size_t size = 0;


	void clear(Vector &other) const
	{
		other.elemArray = nullptr;
		other.size = 0;
		other.maxCapacity = 0;
	}

	void realloc()
	{
		if (maxCapacity == 0)
			maxCapacity = BEGINNINING_CAPACITY;
		else
			maxCapacity *= 2;

		pointer temp = new value_type[maxCapacity];

		std::copy(cbegin(), cend(), temp);

		delete[] elemArray;
		elemArray = temp;
	}

public:

	Vector()
	{}

	Vector(std::initializer_list<Type> l)
	{
		elemArray = new value_type[l.size()];
		std::copy(l.begin(), l.end(), elemArray);
		size = l.size();
		maxCapacity = l.size();
	}

	Vector(const Vector &other) : maxCapacity(other.maxCapacity), size(other.size)
	{
		elemArray = new value_type[maxCapacity];
		std::copy(other.begin(), other.end(), elemArray);
	}

	Vector(Vector &&other) : elemArray(other.elemArray), maxCapacity(other.maxCapacity), size(other.size)
	{

		clear(other);
	}

	~Vector()
	{
		delete[] elemArray;
	}

	Vector &operator=(const Vector &other)
	{
		if (this != &other)
		{
			delete[] elemArray;
			maxCapacity = other.maxCapacity;
			elemArray = new value_type[maxCapacity];
			std::copy(other.begin(), other.end(), begin());
			size = other.size;
		}

		return *this;
	}

	Vector &operator=(Vector &&other)
	{
		if (this != &other)
		{
			delete[] elemArray;
			elemArray = other.elemArray;
			size = other.size;
			maxCapacity = other.maxCapacity;
			clear(other);
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
		insert(cend(), item);
	}

	void prepend(const Type &item)
	{
		insert(cbegin(), item);
	}

	void insert(const const_iterator &insertPosition, const Type &item)
	{
		if (size >= maxCapacity)
			realloc();

		for (size_type i = size; i > insertPosition.getIndex(); --i)
		{
			elemArray[i] = elemArray[i - 1]; //doing free space at index = insertPosition.getIndex()
		}
		elemArray[insertPosition.getIndex()] = item;

		++size;
	}

	Type popFirst()
	{
		if (size == 0)
			throw std::logic_error("vector is empty, cannot popFirst");

		auto retValue = elemArray[0];

		for (size_t i = 0; i < size - 1; ++i)
		{
			elemArray[i] = elemArray[i + 1];
		}
		--size;

		return retValue;
	}

	Type popLast()
	{
		if (size == 0)
			throw std::logic_error("vector is empty, cannot popLast");

		--size;
		return elemArray[size];
	}

	void erase(const const_iterator &position)
	{
		if(position.getIndex() > size)
			throw std::invalid_argument("Trying to erase element which is not in vector");

		std::copy(position + 1, cend(), Iterator(position));
		//copy data from [position+1,cend) to [position,cend-1)

		--size;
	}

	void erase(const const_iterator &firstIncluded, const const_iterator &lastExcluded)
	{
		if(firstIncluded.getIndex() > lastExcluded.getIndex())
			throw std::invalid_argument("first is greater than last!");

		std::copy(lastExcluded, cend(), Iterator(firstIncluded));
		//copy data from [lastExcluded,cend) to [firstIncluded,cend-1)

		size = size - lastExcluded.getIndex() + firstIncluded.getIndex();
	}

	iterator begin()
	{
		return Iterator(cbegin());
	}

	iterator end()
	{
		return Iterator(cend());
	}

	const_iterator cbegin() const
	{
		return ConstIterator(this, 0);
	}

	const_iterator cend() const
	{
		return ConstIterator(this, size);
	}

	const_iterator begin() const
	{
		return ConstIterator(this, 0);
	}

	const_iterator end() const
	{
		return ConstIterator(this, size);
	}
};

template<typename Type>
class Vector<Type>::ConstIterator
{
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename Vector::value_type;
	using difference_type = typename Vector::difference_type;
	using pointer = typename Vector::const_pointer;
	using reference = typename Vector::const_reference;

private:
	const Vector<Type> *vector;
	size_t index;

public:
	size_t getIndex() const
	{
		return index;
	}

	explicit ConstIterator(const Vector<Type> *vector = nullptr, size_t index = 0) :
			vector(vector), index(index)
	{}

	reference operator*() const
	{
		if (index >= vector->size || vector == nullptr)
			throw std::out_of_range("Incorrect index");
		return vector->elemArray[index];
	}

	ConstIterator &operator++()
	{
		if (index + 1 > vector->size)
			throw std::out_of_range("Cannot increment, it`s last element");
		++index;

		return *this;
	}

	ConstIterator operator++(int)
	{
		if (index + 1 > vector->size)
			throw std::out_of_range("Cannot increment, it`s last element");
		ConstIterator beforeIncrement(vector, index);
		++index;

		return beforeIncrement;
	}

	ConstIterator &operator--()
	{
		if (index == 0)
			throw std::out_of_range("Cannot decrement, it`s first element");
		--index;

		return *this;
	}

	ConstIterator operator--(int)
	{
		if (index == 0)
			throw std::out_of_range("Cannot decrement, it`s first element");
		ConstIterator beforeDecrement(vector, index);
		--index;

		return beforeDecrement;
	}

	ConstIterator operator+(difference_type d) const
	{
		if (index + d > vector->size)
			throw std::out_of_range("Cannot increment, it`s last element");

		return ConstIterator(vector, index + d);
	}

	ConstIterator operator-(difference_type d) const
	{
		if (index < (size_t) d)
			throw std::out_of_range("Cannot decrement, it`s first element");

		return ConstIterator(vector, index - d);
	}

	bool operator==(const ConstIterator &other) const
	{
		return vector == other.vector && index == other.index;
	}

	bool operator!=(const ConstIterator &other) const
	{
		return !operator==(other);
	}
};

template<typename Type>
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
public:
	using pointer = typename Vector::pointer;
	using reference = typename Vector::reference;

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
#endif // AISDI_LINEAR_VECTOR_H
