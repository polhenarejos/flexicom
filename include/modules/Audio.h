// $Id: InterPunct.h 179 2012-08-22 11:46:41Z phenarejos $

#ifndef _INC_AUDIO_H_
#define _INC_AUDIO_H_

#include <gr_hier_block2.h>

class AudioSource : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<AudioSource> sptr;
		static sptr Create(unsigned int);
	
	private:
		AudioSource(unsigned int);
};

class AudioSink : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<AudioSink> sptr;
		static sptr Create(unsigned int);
	
	private:
		AudioSink(unsigned int);
};

#endif //_INC_AUDIOH_
