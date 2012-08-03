#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) 
{
	if (argc == 1)
	{
		std::cout<<"USE "<<argv[0]<<" FILE"<<std::endl;
		exit(-1);
	}
  fstream myfile;
  char buf[1024];
  sprintf(buf, "%s.dat", argv[1]);
  std::ofstream out(buf,std::ios_base::binary);
  string line;
  myfile.open(argv[1]);
  float dub;
  if  ( myfile.is_open() ) 
  {
    while ( getline( myfile, line) ) 
    {
      // getline( myfile, line);
      // conversion and output function goes here
      // this example just prints it to stdout
     dub = ::atof(line.c_str());
     out.write((char *)&dub,sizeof(float));
      //cout << line << endl;
    }
    myfile.close();
    out.close();
  } 
  else 
  {
    cerr << "could not open file" << endl;
  }
  return 0; 
}