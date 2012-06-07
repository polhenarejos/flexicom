#ifndef _INC_QT_H_
#define _INC_QT_H_

#include <gr_block.h>
#include <gr_io_signature.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <QObject>
#include <QMutex>

class QtBlock : public QObject, public gr_block
{
	Q_OBJECT
	
	protected:
		QtBlock(QwtPlot *, std::string &, gr_io_signature_sptr);
		QwtPlot *qp;
		QwtPlotCurve **qc;
		QMutex mutex;
		static const int maxCurves = 4;
		double yval[4][8192*2];
		QwtPlotDirectPainter *dp;
		
	public slots:
		void DrawPlot();
	
	signals:
		void Replot();
};
class Qt1D : public QtBlock
{
	public:
		typedef boost::shared_ptr<Qt1D> sptr;
		static sptr Create(QwtPlot *, int = 0);
		int general_work(int, gr_vector_int &, gr_vector_const_void_star &, gr_vector_void_star &);
		static const int maxXval = 8192*2;
		
	private:
		Qt1D(QwtPlot *, int = 0);
		int acc;
		double xval[8192*2];
};

#endif //_INC_QT_H_
