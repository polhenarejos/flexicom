/* 
 * This file is part of the FlexiCom distribution (https://github.com/polhenarejos/flexicom).
 * Copyright (c) 2012-2020 Pol Henarejos, at Centre Tecnologic de Telecomunicacions de Catalunya (CTTC).
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/* $Id$ */
/* $Format:%ci$ ($Format:%h$) by $Format:%an%$ */

#include "QtBlock.h"
#include "compat.h"
#include <volk/volk.h>
#ifdef _WIN
#include <windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#endif

QtBlock::QtBlock(QwtPlot *_qp, std::string _s, gr::io_signature::sptr _io) :
	gr::block(_s, _io, gr::io_signature::make(0, 0, 0)), qp(_qp)
{
	QObject::connect(this, SIGNAL(Replot()), this, SLOT(DrawPlot()), Qt::BlockingQueuedConnection);
}
void QtBlock::DrawPlot()
{
	//printf("in");
	mutex.lock();
	/*
	QwtPlotItemList list = qp->itemList(QwtPlotItem::Rtti_PlotCurve);
	for (int i = 0; i < list.size(); i++)
	{
		QwtPlotCurve *qc = (QwtPlotCurve *)list[i];
		if (qc->dataSize())
		{
			//double *yval = (double *)((QwtCPointerData *)(qc->data()))->yData();
			//printf("fsamp out %lf (ds = %d)\n", yval[0], qc->dataSize());
			//delete ((QwtCPointerData *)qc->data())->yData();
		}
	}
	*/
	qp->replot();
	//qp->detachItems(QwtPlotItem::Rtti_PlotCurve);
	mutex.unlock();
}
Qt1D::sptr Qt1D::Create(QwtPlot *_qp, int _acc)
{
	return sptr(new Qt1D(_qp, _acc));
}
Qt1D::Qt1D(QwtPlot *_qp, int _acc) :
	QtBlock(_qp, std::string("Qt1D"), gr::io_signature::make(1, maxCurves, sizeof(float))), acc(_acc)
{
	//float *xfval = (float *)_aligned_malloc(sizeof(float)*maxXval, 16);
	//__m128 *pxfval = (__m128 *)xval;
	//for (int i = 0; i < maxXval; i += 4) //asume sizeof(float) = 4
	//	pxfval[i/4] = _mm_set_ps(i+3, i+2, i+1, i);
	//xval = (double *)malloc(sizeof(double)*maxXval);
	for (int i = 0; i < maxXval; i++)
		xval[i] = (double)i;
	//volk_32f_convert_64f_a(xval, xfval, maxXval);
	qc = new QwtPlotCurve *[maxCurves];
	for (int i = 0; i < maxCurves; i++)
	{
		qc[i] = new QwtPlotCurve("C");
		qc[i]->attach(qp);
		yval[i] = (double *)volk_malloc(sizeof(double)*8192*2, 16);
	}
	
}
int Qt1D::general_work(int no, gr_vector_int &ni, gr_vector_const_void_star &i, gr_vector_void_star &o)
{
	const float **in = (const float **)&i[0];
	mutex.lock();
	for (uint n = 0; n < i.size(); n++)
	{
		if (is_unaligned())
			volk_32f_convert_64f_u(yval[n], in[n], no);
		else
			volk_32f_convert_64f_a(yval[n], in[n], no);
		//printf("fsamp in %lf (ds = %d)\n", yval[0][0], no);
#ifdef _OSX
		qc[n]->setRawData((const double *)xval, (const double *)yval[n], no);
#else
		qc[n]->setRawSamples((const double *)xval, (const double *)yval[n], no);
#endif
	}
	mutex.unlock();
	//qp->replot();
	emit Replot();
	consume_each(no);
#ifdef _WIN
	Sleep(10);
#else
	usleep(10);
#endif
	return 0;
}
