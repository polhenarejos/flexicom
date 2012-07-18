// $Id$
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
