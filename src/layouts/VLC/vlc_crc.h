#ifndef _VLC_CRC_H_
#define _VLC_CRC_H_

class vlc_crc {
	
	public:	
		vlc_crc (int );
		~vlc_crc();
		void generate_crc(int *, int *);
		void check_crc (int *, int *, bool *);
		
		
	
	private:
		int *poly;
		int crc_length;		
		int length_data;
};

#endif /*_VLC_CRC_H_*/