#include "Qt.h"
#include <volk_32f_convert_64f_a.h>
#include <qwt_plot_curve.h>

QtBlock::QtBlock(QwtPlot *_qp, std::string &_s, gr_io_signature_sptr _io) :
	gr_block(_s, _io, gr_make_io_signature(0, 0, 0)), qp(_qp)
{
}
Qt1D::sptr Qt1D::Create(QwtPlot *_qp, int _acc)
{
	return sptr(new Qt1D(_qp, _acc));
}
Qt1D::Qt1D(QwtPlot *_qp, int _acc) :
	QtBlock(_qp, std::string("Qt1D"), gr_make_io_signature(1, gr_io_signature::IO_INFINITE, sizeof(float))), acc(_acc)
{
	float *xfval = (float *)_aligned_malloc(sizeof(float)*maxXval, 16);
	__m128 *pxfval = (__m128 *)xval;
	for (int i = 0; i < maxXval; i += 4) //asume sizeof(float) = 4
		pxfval[i/4] = _mm_set_ps(i+3, i+2, i+1, i);
	xval = (double *)_aligned_malloc(sizeof(double)*maxXval, 16);
	volk_32f_convert_64f_a_sse2(xval, xfval, maxXval);
}
int Qt1D::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &i, gr_vector_void_star &o)
{
	const float **in = (const float **)&i[0];
	for (int n = 0; n < i.size(); n++)
	{
		QwtPlotCurve *qc = new QwtPlotCurve();
		double *yval = (double *)_aligned_malloc(sizeof(double)*no, 16);
		volk_32f_convert_64f_a_sse2(yval, in[n], no);
		qc->setRawSamples(xval, yval, no);
		qc->attach(qp);
	}
	qp->replot();
	consume_each(no);
	return 0;
}
