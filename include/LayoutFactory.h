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

#ifndef _INC_LAYOUTFACTORY_H_
#define _INC_LAYOUTFACTORY_H_

#include <gnuradio/hier_block2.h>
#include <boost/noncopyable.hpp>

class MainWindow;
class Panel;

class GeneralSink : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<GeneralSink> sptr;
		static sptr Create(int, int, Panel *);
		
	private:
		GeneralSink(int, int, Panel *);
};
class GeneralSource : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<GeneralSource> sptr;
		static sptr Create(int, int, Panel *);
		
	private:
		GeneralSource(int, int, Panel *);
};
class GeneralFlip : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<GeneralFlip> sptr;
		static sptr Create(int);
		
	private:
		GeneralFlip(int);
};

class LayoutFactory : boost::noncopyable
{
	private:
		int devs;
		int dev;
		bool rx;
		
	protected:
		LayoutFactory(MainWindow *, int);
		
	public:
		GeneralSink::sptr gen_sink;
		GeneralSource::sptr gen_source;
		virtual const char *Name() = 0;
		typedef boost::shared_ptr<LayoutFactory> sptr;
		virtual void Run() = 0;
		virtual void Stop() = 0;
		GeneralSource::sptr Source();
		GeneralSink::sptr Sink();
		void Setup();
		MainWindow *mw;
		int radioID;
};

#endif //_INC_LAYOUTFACTORY_H_
