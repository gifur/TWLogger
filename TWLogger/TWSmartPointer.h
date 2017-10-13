/* ---------------------------------------------------------------------------------- *
*
* Copyright (c) 2017 Josephus <guifaliao@gmail.com>
*
* TWSmartPointer.h is the smart pointer class mechanism implement.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files(the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify,
* merge, publish, distribute, sublicense, and / or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to the following
* conditions :
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* --------------------------------------------------------------------------------- */


#ifndef TWINKLE_TSMARTPOINT_H_
#define TWINKLE_TSMARTPOINT_H_


#pragma once

#include <assert.h>
#include <algorithm>

typedef unsigned CounterType;
typedef unsigned count_type;

template<class T>
class TSmartPointer
{
public:
	TSmartPointer(void):counter(0)
	{ }
	TSmartPointer(const TSmartPointer&):counter(0)
	{ }
	explicit TSmartPointer(T* rhs = 0)
	
	{ }
	virtual ~TSmartPointer(void);

	void AddRef() const;
	void RemoveRef() const;
	TSmartPointer& operator=(const TSmartPointer& ) { return *this; }

private:
	CounterType counter;
	T* pointee;
};



class TWSharedObject
{
public:
	void AddRef() const
	{
		++use_count;
		printf("AddRef: %d\n", use_count);
	}
	void RemoveRef() const
	{
		assert(use_count > 0);
		bool destroy = --use_count == 0;
		printf("RemoveRef: %d\n", use_count);
		if (destroy) {
			printf("Before destroy\n");
			delete this;
		}
			
	}
	CounterType GetCount() const { return use_count; }

protected:
	TWSharedObject()
		: access_mutex()
		, use_count(0)
	{
		printf("TWSharedObject::Constructor\n");
	}

	TWSharedObject(const TWSharedObject&)
		: access_mutex()
		, use_count(0)
	{
		printf("TWSharedObject::Constructor1\n");
	}

	virtual ~TWSharedObject() 
	{ 
		assert(use_count == 0);
		printf("TWSharedObject::Distructor\n");
	}

	TWSharedObject& operator=(const TWSharedObject&)  { return *this; }

public:
	typedef unsigned Mutex;
	mutable Mutex access_mutex;

private:
	
	mutable CounterType use_count;
};


//Use it match with Type<TWSharedObject>
template<class T>
class TWSharedPointer
{
public:
	
	explicit TWSharedPointer(T* realPtr = 0)
		: pointee(realPtr)
	{
		AddRef();
	}

	TWSharedPointer(const TWSharedPointer& rhs)
		:pointee(rhs.pointee)
	{
		AddRef();
	}

	~TWSharedPointer()
	{
		if (pointee)
			pointee->RemoveRef();
	}

	// Operators
	bool operator==(const TWSharedPointer& rhs) const
	{ return (pointee == rhs.pointee); }
	bool operator!=(const TWSharedPointer& rhs) const
	{ return (pointee != rhs.pointee); }
	bool operator==(const T* rhs) const { return (pointee == rhs); }
	bool operator!=(const T* rhs) const { return (pointee != rhs); }
	T* operator->() const {assert (pointee); return pointee; }
	T& operator*() const {assert (pointee); return *pointee; }

	TWSharedPointer& operator=(const TWSharedPointer& rhs)
	{
		return this->operator = (rhs.pointee);
	}
	TWSharedPointer& operator=(T* rhs)
	{
		TWSharedPointer<T>(rhs).Swap(*this);
		return *this;
	}

	void Swap(TWSharedPointer& other)
	{//Core.speciailize.argumemnt-dependent lookup
		using std::swap;
		swap (pointee, other.pointee);
	}

	T* Get() const { return pointee; }
	bool operator ! () const
	{
		return ! pointee;
	}
	bool Unique() const
	{
		if(pointee)
			return (pointee->GetCount() == 1);
		return false;
	}

private:

	void AddRef() const
	{
		if (pointee)
			pointee->AddRef();
	}
	T* pointee;
};

#endif //TWINKLE_TSMARTPOINT_H_

