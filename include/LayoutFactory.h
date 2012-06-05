#ifndef _INC_LAYOUTFACTORY_H_
#define _INC_LAYOUTFACTORY_H_

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

class MainWindow;
class LayoutFactory : boost::noncopyable
{
	private:
	protected:
		LayoutFactory() {};
	public:
		virtual const char *Name() = 0;
		typedef boost::shared_ptr<LayoutFactory> sptr;
		static sptr Create(MainWindow * = NULL, int = 0);
		virtual void Run() = 0;
		virtual void Stop() = 0;
};

#endif //_INC_LAYOUTFACTORY_H_
