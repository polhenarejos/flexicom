// $Id$
#include <cppunit/extensions/HelperMacros.h>
#include "InterPunct.h"
#include "bbVLCDeInterleaver.h"
#include <gr_file_source.h>
#include <gr_top_block.h>
#include <gr_vector_sink_f.h>
#include <gr_float_to_complex.h>
#include <vector>
#include <gr_float_to_int.h>
#include <gr_char_to_float.h>
#include <gr_int_to_float.h>
#include "MSE.h"
#include "bbMatlab.h"

class TestVLCInterleaver : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TestVLCInterleaver);
	CPPUNIT_TEST(test_Interleave_PHR);
	CPPUNIT_TEST(test_Interleave_PSDU);
	CPPUNIT_TEST(test_DeInterleave_PHR);
	CPPUNIT_TEST(test_DeInterleave_PSDU);
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void test_Interleave_PHR()
		{
			unsigned int GF = 4, N = 15, K = 7, phr_len = 54, len = 30;
			gr_top_block_sptr gt = gr_make_top_block("Interleave_PHR");
			//bbVLCInterleaver::sptr intlv = bbVLCInterleaver::Create(GF, N, K , phr_len, len);
			InterPunct::sptr intlv = InterPunct::Create(GF, N, K , phr_len, len);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PHR_out_rs_15_7.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PHR_in_cc_1_4.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, intlv, 0);
			gt->connect(intlv, 0, i2f, 0);
			gt->connect(i2f, 0, f2c0, 0);
			gt->connect(fo, 0, f2c1, 0);
			gt->connect(f2c0, 0, mse, 0);
			gt->connect(f2c1, 0, mse, 1);
			gt->connect(mse, 0, vec, 0);
			gt->run();
			std::vector<float> data = vec->data();
			for (unsigned int i = 0; i < data.size(); i++)
   				CPPUNIT_ASSERT_DOUBLES_EQUAL(0, data[i], 10e-9);
		}
		
		void test_Interleave_PSDU()
		{
			unsigned int GF = 4, N = 15, K = 7, phr_len = 10*8, len = 45;
			gr_top_block_sptr gt = gr_make_top_block("Interleave_PSDU");
			//bbVLCInterleaver::sptr intlv = bbVLCInterleaver::Create(GF, N, K , phr_len, len);
			InterPunct::sptr intlv = InterPunct::Create(GF, N, K , phr_len, len);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_out_rs_15_7.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_in_cc_1_4.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, intlv, 0);
			gt->connect(intlv, 0, i2f, 0);
			gt->connect(i2f, 0, f2c0, 0);
			gt->connect(fo, 0, f2c1, 0);
			gt->connect(f2c0, 0, mse, 0);
			gt->connect(f2c1, 0, mse, 1);
			gt->connect(mse, 0, vec, 0);
			gt->run();
			std::vector<float> data = vec->data();
			for (unsigned int i = 0; i < data.size(); i++)
   				CPPUNIT_ASSERT_DOUBLES_EQUAL(0, data[i], 10e-9);
		}
		
		void test_DeInterleave_PHR()
		{
			unsigned int GF = 4, N = 15, K = 7, phr_len = 54, len = 120;
			gr_top_block_sptr gt = gr_make_top_block("DeInterleave_PHR");
			bbVLCDeInterleaver::sptr deintlv = bbVLCDeInterleaver::Create(GF, N, K , phr_len, len);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PHR_in_deinterleaver.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PHR_out_deinterleaver.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, deintlv, 0);
			gt->connect(deintlv, 0, i2f, 0);
			gt->connect(i2f, 0, f2c0, 0);
			gt->connect(fo, 0, f2c1, 0);
			gt->connect(f2c0, 0, mse, 0);
			gt->connect(f2c1, 0, mse, 1);
			gt->connect(mse, 0, vec, 0);
			gt->run();
			std::vector<float> data = vec->data();
			for (unsigned int i = 0; i < data.size(); i++)
   				CPPUNIT_ASSERT_DOUBLES_EQUAL(0, data[i], 10e-9);
		}
		
		void test_DeInterleave_PSDU()
		{
			unsigned int GF = 4, N = 15, K = 7, psdu_len = 10*8, len = 176;
			gr_top_block_sptr gt = gr_make_top_block("DeInterleave_PSDU");
			bbVLCDeInterleaver::sptr deintlv = bbVLCDeInterleaver::Create(GF, N, K , psdu_len, len);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_in_deinterleaver.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_out_deinterleaver.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, deintlv, 0);
			gt->connect(deintlv, 0, i2f, 0);
			gt->connect(i2f, 0, f2c0, 0);
			gt->connect(fo, 0, f2c1, 0);
			gt->connect(f2c0, 0, mse, 0);
			gt->connect(f2c1, 0, mse, 1);
			gt->connect(mse, 0, vec, 0);
			gt->run();
			std::vector<float> data = vec->data();
			for (unsigned int i = 0; i < data.size(); i++)
   				CPPUNIT_ASSERT_DOUBLES_EQUAL(0, data[i], 10e-9);
		}
		
};
CPPUNIT_TEST_SUITE_REGISTRATION(TestVLCInterleaver);
