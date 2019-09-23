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

#include <stdexcept>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>


int main( int argc, char* argv[] )
{
  std::string testPath = (argc > 1) ? std::string(argv[1]) : "";

  // Create the event manager and test controller
  CppUnit::TestResult controller;

  // Add a listener that collects test result
  CppUnit::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CppUnit::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CppUnit::TestRunner runner;
  runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );   
  try
  {
    std::cout << "Running unit tests"  <<  testPath << std::endl;
    runner.run( controller, testPath );

    std::cerr << std::endl;

    // Print test in a compiler compatible format.
    CppUnit::CompilerOutputter outputter( &result, std::cerr );
    outputter.write();                      
  }
  catch ( std::invalid_argument &e )  // Test path not resolved
  {
    std::cerr  <<  std::endl  
               <<  "ERROR: "  <<  e.what()
               << std::endl;
    return 0;
  }

  return result.wasSuccessful() ? 0 : 1;
}
