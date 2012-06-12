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
	//QSpinBox *sp_psdu_units;
	QSpinBox *sp_psdu_units[2];
	
	QLabel *l_phy_type;
	QRadioButton *rb_phy_type[2];
	
	QLabel *l_phy_modulation;
	QRadioButton *rb_phy_modulation	[2];
			
	QLabel *l_phy_op_mode;
	//QComboBox *cb_phy_op_mode;
	QComboBox *cb_phy_op_mode[4];
	
	QLabel *l_frame_size; //bytes
	//QSpinBox *sp_frame_size;
	QSpinBox *sp_frame_size[2];
	
	
}VarVLC;