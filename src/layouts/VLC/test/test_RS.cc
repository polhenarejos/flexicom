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
#include "bbRSEnc.h"
#include "bbRSDec.h"
#include "bb_bit_removal.h"
#include <gnuradio/file_source.h>
#include <gnuradio/top_block.h>
#include <gnuradio/vector_sink_f.h>
#include <gnuradio/float_to_complex.h>
#include <vector>
#include <gnuradio/float_to_int.h>
#include <gnuradio/char_to_float.h>
#include <gnuradio/int_to_float.h>
#include "MSE.h"
#include "bbMatlab.h"

class TestReedSolomon : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TestReedSolomon);
	CPPUNIT_TEST(test_Encode15_7_PHR);
	CPPUNIT_TEST(test_Encode15_7_PSDU);
	CPPUNIT_TEST(test_Encode15_11_PSDU);
	CPPUNIT_TEST(test_Decode15_7_PHR);
	CPPUNIT_TEST(test_Decode15_7_PSDU);
	CPPUNIT_TEST(test_Decode15_11_PSDU);
	
	CPPUNIT_TEST_SUITE_END();
	
	public:
		void test_Encode15_7_PHR()
		{
			unsigned int GF = 4, N = 15, K = 7, phy = 0, len = 54;
			gr_top_block_sptr gt = gr_make_top_block("RSEncode15_7_PHR");
			bbRSEnc::sptr rs = bbRSEnc::Create(&GF, &N, &K, &phy, &len);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PHR_in_rs_15_7.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PHR_out_rs_15_7.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, rs, 0);
			gt->connect(rs, 0, i2f, 0);
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
		void test_Encode15_7_PSDU()
		{
			unsigned int GF = 4, N = 15, K = 7, phy = 0, len = 80;
			gr_top_block_sptr gt = gr_make_top_block("RSEncode15_7_PSDU");
			bbRSEnc::sptr rs = bbRSEnc::Create(&GF, &N, &K, &phy, &len);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_in_rs_15_7.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_out_rs_15_7.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, rs, 0);
			gt->connect(rs, 0, i2f, 0);
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
		void test_Encode15_11_PSDU()
		{
			unsigned int GF = 4, N = 15, K = 11, phy = 0, len = 80;
			gr_top_block_sptr gt = gr_make_top_block("RSEncode15_11_PSDU");
			bbRSEnc::sptr rs = bbRSEnc::Create(&GF, &N, &K, &phy, &len);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_in_rs_15_11.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_out_rs_15_11.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, rs, 0);
			gt->connect(rs, 0, i2f, 0);
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
		
		void test_Decode15_7_PHR()
		{
			unsigned int GF = 4, N = 15, K = 7, phy = 0, len = 30, raw_length=54;
			gr_top_block_sptr gt = gr_make_top_block("Decode15_7_PHR");
			bbRSDec::sptr rs_dec = bbRSDec::Create(GF, N, K, phy, len);
			bb_bit_removal::sptr br = bb_bit_removal::Create(rs_dec->out_rs_dec,raw_length);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PHR_out_rs_15_7.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PHR_data.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, rs_dec, 0);
			gt->connect(rs_dec,0, br, 0);
			gt->connect(br,0,i2f,0);
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
		
		void test_Decode15_7_PSDU()
		{
			unsigned int GF = 4, N = 15, K = 7, phy = 0, len = 45, raw_length=80;
			gr_top_block_sptr gt = gr_make_top_block("Decode15_7_PSDU");
			bbRSDec::sptr rs_dec = bbRSDec::Create(GF, N, K, phy, len);
			bb_bit_removal::sptr br = bb_bit_removal::Create(rs_dec->out_rs_dec,raw_length);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_out_rs_15_7.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_data.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, rs_dec, 0);
			gt->connect(rs_dec,0, br, 0);
			gt->connect(br,0,i2f,0);
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
		
		void test_Decode15_11_PSDU()
		{
			unsigned int GF = 4, N = 15, K = 11, phy = 0, len = 28, raw_length=80;
			gr_top_block_sptr gt = gr_make_top_block("Decode15_11_PSDU");
			bbRSDec::sptr rs_dec = bbRSDec::Create(GF, N, K, phy, len);
			bb_bit_removal::sptr br = bb_bit_removal::Create(rs_dec->out_rs_dec,raw_length);
			gr_file_source_sptr fi = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_out_rs_15_11.dat");
			gr_file_source_sptr fo = gr_make_file_source(sizeof(float), "src/layouts/VLC/test/vecs/PSDU_data.dat");
			MSE::sptr mse = MSE::Create();
			gr_float_to_complex_sptr f2c0 = gr_make_float_to_complex();
			gr_float_to_complex_sptr f2c1 = gr_make_float_to_complex();
			gr_vector_sink_f_sptr vec = gr_make_vector_sink_f(1);
			gr_float_to_int_sptr f2i = gr_make_float_to_int();
			gr_int_to_float_sptr i2f = gr_make_int_to_float(1, 1.0);
			gt->connect(fi, 0, f2i, 0);
			gt->connect(f2i, 0, rs_dec, 0);
			gt->connect(rs_dec,0, br, 0);
			gt->connect(br,0,i2f,0);
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
CPPUNIT_TEST_SUITE_REGISTRATION(TestReedSolomon);
