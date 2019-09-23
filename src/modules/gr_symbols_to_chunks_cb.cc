/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gr_symbols_to_chunks_cb.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <string.h>

gr_symbols_to_chunks_cb_sptr
gr_make_symbols_to_chunks_cb (const std::vector<gr_complex> &symbol_table)
{
  return gnuradio::get_initial_sptr (new gr_symbols_to_chunks_cb (symbol_table));
}

gr_symbols_to_chunks_cb::gr_symbols_to_chunks_cb (const std::vector<gr_complex> &symbol_table)
  : gr::sync_block ("symbols_to_chunks_cb",
			  gr::io_signature::make (1, -1, sizeof (gr_complex)),
			  gr::io_signature::make (1, -1, sizeof (unsigned char))),
  d_symbol_table (symbol_table)
{
}

int
gr_symbols_to_chunks_cb::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  int nstreams = input_items.size();

  for (int m=0;m<nstreams;m++) {
    const gr_complex *in = (gr_complex *) input_items[m];
    unsigned char *out = (unsigned char *) output_items[m];

    // per stream processing
    for (int i = 0; i < noutput_items; i++){
    	std::vector<float> dist;
    	for (int d = 0; d < d_symbol_table.size(); d++)
    		dist.push_back(std::norm(d_symbol_table[d]-in[i]));
    	*out++ = (unsigned char)(std::min_element(dist.begin(), dist.end())-dist.begin());
    }
    // end of per stream processing

  }
  return noutput_items;
}
