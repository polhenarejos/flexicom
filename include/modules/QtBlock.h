#ifndef _INC_QT_H_
#define _INC_QT_H_

#include <gr_block.h>
#include <gr_io_signature.h>
#include <qwt_plot.h>

class QtBlock : public gr_block
{
	protected:
		QtBlock(QwtPlot *, std::string &, gr_io_signature_sptr);
		QwtPlot *qp;
};
class Qt1D : public QtBlock
{
	public:
		typedef boost::shared_ptr<Qt1D> sptr;
		static sptr Create(QwtPlot *, int = 0);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		static const int maxXval = 8192;
		
	private:
		Qt1D(QwtPlot *, int = 0);
		int acc;
		double *xval;
};

#endif //_INC_QT_H_
