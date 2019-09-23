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

#ifndef _INC_RX80211B_H_
#define _INC_RX80211B_H_

#include <gnuradio/hier_block2.h>
#include <gnuradio/msg_queue.h>
#include <QtCore/QThread>

class Rx80211bThread : public QThread
{
	public:
		Rx80211bThread(gr::msg_queue::sptr);
	protected:
		void run();
	private:
		gr::msg_queue::sptr queue;
};
class Layout80211b;
class Rx80211b : public gr::hier_block2
{
	public:
		typedef boost::shared_ptr<Rx80211b> sptr;
		static sptr Create(Layout80211b *);
		~Rx80211b();
		static std::vector<float> BarkerTaps(int);
		void stop();
		
	private:
		Rx80211b(Layout80211b *);
		gr::msg_queue::sptr msgq;
		Rx80211bThread *rxth;
		static int sinc(int, float, float *);
		Layout80211b *ly;
};

#endif //_INC_RX80211B_H_
