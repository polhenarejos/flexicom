// $Id$

#ifndef _INC_LAYOUTFACTORY_H_
#define _INC_LAYOUTFACTORY_H_

#include <gr_hier_block2.h>
#include <boost/noncopyable.hpp>

class MainWindow;
class Panel;

class GeneralSink : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<GeneralSink> sptr;
		static sptr Create(int, int, Panel *);
		
	private:
		GeneralSink(int, int, Panel *);
};
class GeneralSource : public gr_hier_block2
{
	public:
		typedef boost::shared_ptr<GeneralSource> sptr;
		static sptr Create(int, int, Panel *);
		
	private:
		GeneralSource(int, int, Panel *);
};
class GeneralFlip : public gr_hier_block2
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
