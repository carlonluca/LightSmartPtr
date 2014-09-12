/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    05.12.2011
 *
 * Copyright (c) 2013 Luca Carlon. All rights reserved.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 */

#ifndef LC_SMARTPTR_H
#define LC_SMARTPTR_H

/*------------------------------------------------------------------------------
 |    includes
 +-----------------------------------------------------------------------------*/
#include <map>
#include <typeinfo>

#ifdef ENABLE_DEBUG_DYNAMIC
/*------------------------------------------------------------------------------
 |    LC_HeapDebugger class
 +-----------------------------------------------------------------------------*/
class LC_HeapDebugger
{
public:
   static void addAllocation(void* p, const char* name) {
      m_allocations.insert(std::pair<void*, const char*>(p, name));
   }

   static void delAllocation(void* p) {
      m_allocations.erase(p);
   }

   static void listAllocations() {
      for (std::map<void*, const char*>::iterator i  = m_allocations.begin(); i != m_allocations.end(); i++)
         printf("In heap: %s.\n", i->second);
   }

   static std::map<void*, const char*>::size_type allocationsCount() {
      return m_allocations.size();
   }

   static std::map<void*, const char*>& allocations() {
      return m_allocations;
   }

private:
   static std::map<void*, const char*> m_allocations;
};
std::map<void*, const char*> LC_HeapDebugger::m_allocations;
#endif // ENABLE_DEBUG_DYNAMIC

/*------------------------------------------------------------------------------
 |    LC_RefCounter class
 +-----------------------------------------------------------------------------*/
class LC_RefCounter
{
public:
   void retain() {
      // Increment the reference count
      count++;
   }

   int release() {
      // Decrement the reference count and return the reference count.
      return --count;
   }

   int retainCount() {
      return count;
   }

private:
   int count;
};

/*------------------------------------------------------------------------------
 |    LC_SmartPtr class
 +-----------------------------------------------------------------------------*/
template <typename T>
class LC_SmartPtr
{
public:
   LC_SmartPtr() : pData(0), reference(0), m_isDetached(false) {
      reference = new LC_RefCounter();
      reference->retain();
   }

   LC_SmartPtr(T* pValue) : pData(pValue), reference(0), m_isDetached(false) {
      reference = new LC_RefCounter();
      reference->retain();
#ifdef ENABLE_DEBUG_DYNAMIC
      LC_HeapDebugger::addAllocation(pValue, typeName());
#endif
   }

   LC_SmartPtr(const LC_SmartPtr<T>& sp) : pData(sp.pData), reference(sp.reference), m_isDetached(false) {
      // Copy constructor.
      reference->retain();
   }

   ~LC_SmartPtr() {
      if (m_isDetached)
         return;
      if (reference->release())
         return;

#ifdef ENABLE_DEBUG_DYNAMIC
      LC_HeapDebugger::delAllocation(pData);
#endif
      delete pData;
      delete reference;
   }

   T& operator *() const {
      return *pData;
   }

   T* operator ->() const {
      return pData;
   }

   LC_SmartPtr<T>& operator =(const LC_SmartPtr<T>& sp) {
      // Avoid self assignment
      if (this == &sp)
         return *this;

      // If reference become zero delete the old data.
      if (!reference->release()) {
#ifdef ENABLE_DEBUG_DYNAMIC
         LC_HeapDebugger::delAllocation(pData, typeName());
#endif
         delete pData;
         delete reference;
      }

      // Copy the data and reference pointer and increment
      // the reference count
      pData     = sp.pData;
      reference = sp.reference;
      reference->retain();
#ifdef ENABLE_DEBUG_DYNAMIC
      LC_HeapDebugger::addAllocation(pData, typeName());
#endif

      return *this;
   }

   bool operator ==(const LC_SmartPtr<T>& s) const {
      if (data() == s.data())
         return true;
      if (data() == NULL || s.data() == NULL)
         return false;
      return *(s.data()) == *(data());
   }

   bool operator !=(const LC_SmartPtr<T>& s) const {
      return !(*this == s);
   }

   int retainCount() {
      return reference->retainCount();
   }

   const char* typeName() {
      return typeid(T).name();
   }

   T* data() {
      return pData;
   }

   T* data() const {
      return pData;
   }

   void detach() {
      m_isDetached = true;
   }

   void attach() {
      m_isDetached = false;
   }

private:
   T* pData;
   LC_RefCounter* reference;
   bool m_isDetached;
};

#endif // LC_SMARTPTR_H
