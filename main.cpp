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

/*------------------------------------------------------------------------------
 |    includes
 +-----------------------------------------------------------------------------*/
#include <QCoreApplication>
#include <map>
#include <assert.h>

#include "lc_smartptr.h"

/*------------------------------------------------------------------------------
 |    LC_TestClass class
 +-----------------------------------------------------------------------------*/
class LC_TestClass
{
public:
   LC_TestClass(int i) : m_i(i) {
      qDebug("LC_Debug %d instantiated!", m_i);
   }

   ~LC_TestClass() {
      qDebug("LC_TestClass %d dtor!", m_i);
   }

   bool operator ==(const LC_TestClass& c) {
      return m_i == c.m_i;
   }

private:
   int m_i;
};

/*------------------------------------------------------------------------------
 |    main
 +-----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
   QCoreApplication a(argc, argv);
   Q_UNUSED(a);

   {
      qDebug("Allocating 2 LC_TestClass's...\n");
      LC_SmartPtr<LC_TestClass> p1(new LC_TestClass(1));
      LC_SmartPtr<LC_TestClass> p2(new LC_TestClass(2));
      Q_UNUSED(p1);
      Q_UNUSED(p2);

#ifdef ENABLE_DEBUG_DYNAMIC
      LC_HeapDebugger::listAllocations();
#endif

      qDebug("Assigning a new smart pointer...\n");
      LC_SmartPtr<LC_TestClass> p3 = p1;
      Q_UNUSED(p3);

#ifdef ENABLE_DEBUG_DYNAMIC
      LC_HeapDebugger::listAllocations();
#endif

      // Check comparison operators.
      assert(p1 == p3);
      assert(p1 != p2);
   }

   qDebug("All should be dealloc'ed now...\n");

#ifdef ENABLE_DEBUG_DYNAMIC
   LC_HeapDebugger::listAllocations();
#endif
   
   return 0;
}
