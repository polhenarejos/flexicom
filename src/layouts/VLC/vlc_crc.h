#ifndef _VLC_CRC_H_
#define _VLC_CRC_H_

class vlc_crc {
	
	public:	
		vlc_crc (int );
		~vlc_crc();
		void generate_crc(int *, int *, int);
		void check_crc (int *, int *, bool *, int);
		
		
		
	
	private:
		int poly[4];
		int length_data;
		int crc_length;		
		
};

#endif /*_VLC_CRC_H_*/