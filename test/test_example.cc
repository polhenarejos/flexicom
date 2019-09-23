/* 
 * This file is part of the FlexiCom distribution (https://github.com/polhenarejos/flexicom).
 * Copyright (c) 2012-2020 Pol Henarejos, at Centre Tecnologic de Telecomunicacions de Catalunya (CTTC).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* $Id$ */
/* $Format:%ci$ ($Format:%h$) by $Format:%an%$ */

#include <cppunit/extensions/HelperMacros.h>

class TestExample : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TestExample);
	CPPUNIT_TEST(test_t1);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void test_t1()
		{
			//Macro to assert equal variables
			float v1 = 1.14, v2 = -2.38, error = 1e-2, res = v1+v2;
			CPPUNIT_ASSERT_EQUAL(v1+v2, res);
			//Macro to assert equal variables with tolerance error
			CPPUNIT_ASSERT_DOUBLES_EQUAL(v1+v2+error, res, 1e-1);
		}
};
CPPUNIT_TEST_SUITE_REGISTRATION(TestExample);
