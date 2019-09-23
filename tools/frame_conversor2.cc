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