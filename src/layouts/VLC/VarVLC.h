// $Id: GUI.cc 1071 2010-12-13 10:26:46Z phenarejos $
#ifndef _INC_VARVLC_H_
#define _INC_VARVLC_H_
#include <QRadioButton>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

typedef struct
{
	QLabel *l_tx_mode;
	QComboBox *cb_tx_mode;
	
	QLabel *l_psdu_units;
	QSpinBox *sp_psdu_units[2];
	
	QLabel *l_phy_type;
	QRadioButton *rb_phy_type[2];
	
	QLabel *l_flp_length;
	QSpinBox *sp_flp_length;
	
	QLabel *l_phy_modulation;
	QRadioButton *rb_phy_modulation	[2];
			
	QLabel *l_phy_op_mode;
	QComboBox *cb_phy_op_mode[4];
	
	QLabel *l_frame_size; //bytes
	QSpinBox *sp_frame_size[2];
	
	
}VarVLC;
#endif //_INC_VARVLC_H_
