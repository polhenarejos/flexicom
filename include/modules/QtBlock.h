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

#ifndef _INC_QT_H_
#define _INC_QT_H_

#include <gnuradio/block.h>
#include <gnuradio/io_signature.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <QtCore/QObject>
#include <QtCore/QMutex>

/*! \brief Qtblock class is the base definition of a plot object to be inserted in the GUI
*/

class QtBlock : public QObject, public gr::block
{
	Q_OBJECT
	
	protected:
		QtBlock(QwtPlot *, std::string , gr::io_signature::sptr);
		QwtPlot *qp;
		QwtPlotCurve **qc;
		QMutex mutex;
		static const int maxCurves = 4;
		double *yval[maxCurves];
		QwtPlotDirectPainter *dp;
		
	public slots:
		void DrawPlot();
	
	signals:
		void Replot();
};

/*! \brief Qt1D class inherites from QtBlock and is the GNURadio block which handles the plot drawing process inside the GNURadio process 
*/


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
