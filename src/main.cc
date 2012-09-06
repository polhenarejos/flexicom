// $Id$
#include <QApplication>
#include "MainWindow.h"
#include <gr_top_block.h>
#include <gr_uhd_usrp_source.h>
#include <gr_null_sink.h>
#include <QtGui>

class App : public QApplication
{
	public:
		App(int argc, char **argv, QApplication::Type t) : QApplication(argc, argv, t) {}
		virtual ~App() {}
		virtual bool notify(QObject * receiver, QEvent * event) 
		{
    		try 
    		{
    	  		return QApplication::notify(receiver, event);
    		} 
    		catch(std::exception &e) 
    		{
      			qCritical() << "Exception thrown:" << e.what();
    		}
    		return false;
		}
};

/*! \mainpage Flexicom: Rapid Prototyping communications systems using Software Defined Radio

* Flexicom is a multi-library platform whose main objective is to provide a common framework to include an implementation of different communication systems, either the transmitter chain or the receiver chain.\n
* Basically, Flexicom is a QT application which can be configured easily to launch a collection of blocks which implements and draws the performance of the selected communication scheme. \n
* Flexicom serves as a repository to collect the custom radio processing blocks, which have been conveniently tested using the CPPUnit framework included inside Flexicom. Currently, this framework includes:
* <ul>
	<li>IEEE 802.11 Receiver\n
	<li>IEEE 802.15.7 Transmitter & Receiver \n
</ul> 
\image html VLC.jpg "Flexicom GUI and VLC configuration tab"
*/
#include <iostream>
#include <string>
#include <fstream>
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <lz4.h>
#include <lz4hc.h>
using namespace rapidxml;

xml_document<> doc;

// allocate the given string from xml document memory
// to ensure that it says alive for the duration of xml
// operations
inline const char* stralloc(const std::string& str)
{
return doc.allocate_string(str.c_str());
}

// create a node
inline xml_node<>* node(const std::string& name, const std::string& content = "")
{
return doc.allocate_node(node_element, stralloc(name), stralloc(content));
}
unsigned int base64_encode(unsigned char *data, unsigned int len, unsigned char *out) 
{
	static unsigned char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned int bound = len/3;
	for (int i = 0; i < bound; i++)
	{
		*out++ = base64_chars[(*data & 0xfc) >> 2];
		*out++ = base64_chars[((*data & 0x03) << 4) + ((*(data+1) & 0xf0) >> 4)];
		*out++ = base64_chars[((*(data+1) & 0x0f) << 2) + ((*(data+2) & 0xc0) >> 6)];
		*out++ = base64_chars[*(data+2) & 0x3f];
		data += 3;
	}
	int mod = len%3;
	if (mod)
	{
		*out++ = base64_chars[(*data & 0xfc) >> 2];
		if (mod == 2)
		{
			*out++ = base64_chars[((*data & 0x03) << 4) + ((*(data+1) & 0xf0) >> 4)];
			*out++ = base64_chars[((*(data+1) & 0x0f) << 2)];
		}
		else
		{
			*out++ =  base64_chars[((*data & 0x03) << 4)];
			*out++ = '=';
		}
		*out++ = '=';
	}
	return (unsigned int)((float)len/3+.5)*4;
}
unsigned int base64_decode(unsigned char *data, unsigned int len, unsigned char *out)
{
	int bound = len/4;
	for (int i = 0; i < bound; i++)
	{
		*out++ = 
	}
	 int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}
#if defined(_WIN) && !defined(CMDLINE)
#include <windows.h>
#include <QStringList>
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	QString s((const char *)lpCmdLine);
	QStringList sl = s.split(' ');
	char **argv = new char *[sl.size()];
	for (uint i = 0; i < sl.size(); i++)
		argv[i] = sl.at(i).toAscii().data();
	int argc = sl.size();
#else
int main(int argc, char **argv)
{
#endif
// root node
char *mejele = "mejele melej";
float aa[] = { .92348 , .32894 , .832948 , .3724234 , .3724234 , .273846 , .2374681 ,.982340 , .238849 ,.283426873 ,.129387923 ,.9236747 };
char *aad = (char *)malloc(sizeof(char)*strlen(mejele)*2);
int len = base64_encode((unsigned char *)mejele, strlen(mejele), (unsigned char *)aad);
aad[len]=  0;
	printf("!%d %s\n",len, aad);
xml_node<>* root = node(stralloc("VoIP"), aad);
doc.append_node(root);
xml_node<>* text = node(stralloc("Text"), "patata osea si mmmm esto no comprime jorl jorl");
doc.append_node(text);
std::string xml_as_string;
// watch for name collisions here, print() is a very common function name!
print(std::back_inserter(xml_as_string), doc, print_no_indenting);
	char *dest = (char *)malloc(LZ4_compressBound(xml_as_string.size()));
	int len2 = LZ4_compressHC(xml_as_string.c_str(), dest, xml_as_string.size());
	printf("LEN 1 %d, LEN 2 %d\n",xml_as_string.size(),len2);
	std::cout<<xml_as_string;
	App app(argc, argv, QApplication::GuiClient);
	app.setOrganizationName(QString("FlexiCom"));
    app.setOrganizationDomain(QString("flexicom.com"));
    app.setApplicationName(QString("FlexiCom"));
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec(); 
}
