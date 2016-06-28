/*
    Goblin compiler v1.0 - The standard compiler for the Goblin language.
    Copyright (C) 2016  Daniel McCarthy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   Stack.h
 * Author: Daniel McCarthy
 *
 * Created on 27 June 2016, 21:37
 * 
 * Description: A simple stack template that has a stack pointer
 */

#ifndef STACK_H
#define STACK_H

#include <vector>
#include <stdexcept>

template <class T>
class Stack
{
public:
    void push(T elem);
    T pop();
    T top();
    T first();
    T pop_first();
    void setSP(int pos);
    bool isEmpty();
    void empty();
    int size();
    int getSP();
private:
    std::vector<T> elements;
    int sp = 0;

};

template <class T>
void Stack<T>::push(T elem)
{
    elements.insert(elements.begin() + sp, elem);
    sp++;
}

template <class T>
T Stack<T>::pop()
{
    sp--;
    T element = top();
    elements.erase(elements.begin() + sp);
    return element;
}

template <class T>
T Stack<T>::top()
{
    if (isEmpty())
    {
        throw std::out_of_range("Stack<>::top(): stack is empty");
    }
    
    return this->elements.at(sp);
}

template <class T>
T Stack<T>::first()
{
    if (isEmpty())
    {
        throw std::out_of_range("Stack<>::first(): stack is empty");
    }
    
    return this->elements.at(0);
}

template <class T>
T Stack<T>::pop_first()
{
    T element = first();
    elements.erase(elements.begin());
    
    return element;
}

template <class T>
void Stack<T>::setSP(int pos)
{
    sp = pos;
}

template <class T>
bool Stack<T>::isEmpty()
{
    return elements.empty();
}

template <class T>
void Stack<T>::empty()
{
    elements.clear();
}

template <class T>
int Stack<T>::size()
{
    return elements.size();
}

template <class T>
int Stack<T>::getSP()
{
    return this->sp;
}


#endif /* STACK_H */

