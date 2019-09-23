// $Id$

#ifndef _INC_AUDIO_H_
#define _INC_AUDIO_H_

#include <gnuradio/hier_block2.h>

class AudioSource : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<AudioSource> sptr;
		static sptr Create(unsigned int);
	
	private:
		AudioSource(unsigned int);
};

class AudioSink : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<AudioSink> sptr;
		static sptr Create(unsigned int);
	
	private:
		AudioSink(unsigned int);
};

#endif //_INC_AUDIOH_
