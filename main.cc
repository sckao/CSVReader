#include <iostream> 
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <TString.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TMinuit.h>

//#include <pthread.h>
//#include <unistd.h>
//#include "TThread.h"

#include "AnaInput.h"
#include "Reader.h"

using namespace std; 


int main( int argc, const char* argv[] ) { 

  string datacardfile = ( argc > 1 ) ? argv[1] : "DataCard.txt";
  //AnaInput  *Input = new AnaInput( datacardfile );
  AnaInput  *Input = AnaInput::Instance() ;
  Input->SetDatacard( datacardfile ) ;
  

  int module = -1 ;
  Input->GetParameters( "Module", & module ) ;

  if ( module == 0 ) {
     Reader  *reader  = new Reader( ) ;
     reader->GetDataFromCSV();
     delete reader ;
  }



  delete Input ;
  cout<<" Finished !!!"<<endl ;

  return 0;

}

