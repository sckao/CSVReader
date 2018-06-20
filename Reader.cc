#include "Reader.h"
#include <gsl/gsl_fit.h>
#include <gsl/gsl_statistics.h>

int is = 0 ;
static bool ScanSort( vec s1, vec s2) { return (s1[is] < s2[is]) ; }   
 
Reader::Reader( ) {

  Input = AnaInput::Instance() ;

  Input->GetParameters("Path",          &hfolder ) ; 
  Input->GetParameters("CSVDIR",        &cfolder ) ; 
  Input->GetParameters("CSVFile",       &cFileName );

  Input->GetParameters("NSkipLine",     &nSkipLine );
  Input->GetParameters("Column",        &colV );

  Input->GetParameters("OutputFile",    &outFileName );
  Input->GetParameters("HFileName",     &hFileName );
  Input->GetParameters("PlotName",      &plotName );
  Input->GetParameters("PlotType",      &plotType ) ; 

  Input->GetParameters("debug",         &debug );
}

Reader::~Reader(){

  cout<<" done ! "<<endl ;
}


void Reader::GetDataFromCSV( ) {

    // Read data from CSV file
    string kfile = cfolder + cFileName ;
    printf(" file = %s \n", kfile.c_str() ) ;

    vector<vec> data0 ;
    data0.clear() ;
    Input->ReadFreeCSV( kfile, data0, nSkipLine ) ;
    cout<<" data size "<< data0.size() << endl ;

    vector<svec> sdata ;
    sdata.clear() ;
    Input->ReadStringCSV( kfile, sdata, 0, 1 ) ;

    // Record the result
    string logFileName = hfolder +  outFileName ;
    FILE* logfile = fopen( logFileName.c_str() ,"w");

    // Sort the data
    //vector<vec> data = Sort2D( data0, 13, 14 ) ;
    vector<vec> data = Sort2D( data0, 1, 2 ) ;
    printf(" data sz = %d , data1 sz = %d \n", (int)data0.size(), (int)data.size() ) ; 


    // Get all index for each data if not specified
    if ( colV[0] == -1 )  {
       colV[0] = 0 ;
       for (int i=1; i< (int)data[0].size(); i++) {
           colV.push_back( i ) ;
       }
    }
    
    //  Print out the variable names
    //cout<<" sdata size "<< sdata.size() <<" -> "<< sdata[0].size()<< endl ;
    for (size_t j = 0 ; j < colV.size(); j++  ) {
        fprintf( logfile,"%s, ", sdata[0][colV[j]].c_str() )  ;
        if ( j == colV.size() - 1 ) fprintf( logfile, "\n" ) ;
    }

    // Output data
    for ( size_t i= 0 ; i< data.size(); i++ ) {
 
        for (size_t j = 0 ; j < colV.size(); j++  ) {

            // remove consolidate data
            if ( data[i][colV[0]] == -1 ) continue ;
            // auto integer detect 
            //if ( data[i][colV[j]] == floor( data[i][colV[j]] ) ) 
            if ( colV[j] < 0 ) 
               fprintf( logfile,"%.0f, ", data[i][colV[j]] )  ; 
            else 
               fprintf( logfile,"%f, ", data[i][colV[j]] )  ;

            if ( j == colV.size() - 1 ) fprintf( logfile, "\n" ) ;
        }
    }

    fclose( logfile ) ;

}


// Sort data on axis1 , within same axis1, sort by axis2
vector<vec> Reader::Sort2D( vector<vec>& data, int axis1, int axis2 ) {

    vector<vec> data1 ;

    // Sort the data - Example
    is = axis1 ;
    if ( data.size() > 1 ) { 
       sort( data.begin(), data.end(), ScanSort );
       //printf(" small d = %.1f ~ %.1f \n", data[0], data[ data.size() - 1 ] ) ;
    }

    vector<vec> col ;
    col.clear() ;
    bool nextCol = false ;
    double x_ = data[0][axis1] ;
    is = axis2 ;

    for (size_t i=0; i< data.size(); i++ ) { 
        // Accumulate the same column data 
        if ( fabs(data[i][axis1] - x_ ) < 0.1 ) {
           col.push_back( data[i] ) ;
           if ( i == data.size() -1 ) nextCol = true ;
        } else { 
           nextCol = true ;
        }

        // Sort the column
        if ( nextCol ) {
           cout<<" col size = "<< col.size() <<endl ;
           sort( col.begin(), col.end(), ScanSort ) ;

           // Fill up the new sorted data
           for ( size_t j=0; j < col.size() ; j++ ) {
               data1.push_back( col[j] ) ;    
           }
           col.clear() ;
           col.push_back( data[i] ) ;
           nextCol = false ;
        }
        x_ = data[i][axis1] ;
    }

    return data1 ;

}


double Reader::Pull( double* xa, double* ya, int csz ) {

     const int sz = csz -1 ;
     double x[sz], y[sz] ; 
     double c0,c1,cov00, cov01, cov11, sumsq ;
     int k = 0 ;
     double ye(0), dy(0), stdy(0) ;
     for ( int i=0; i< csz ; i++) {
         k = 0 ;
         for ( int j= 0; j< sz; j++ ) {
             if ( j == i ) k = j+1  ;
             x[j] = xa[k] ;
             y[j] = ya[k] ;
             k++ ;
         }
         gsl_fit_linear( x, 1, y, 1, sz, &c0, &c1, &cov00, &cov01, &cov11, &sumsq ) ;
         ye = (c1*xa[i]) + c0 ;
         dy = (ya[i] - ye)*(ya[i] - ye) ;
         stdy += dy ;
     }
     double dof = (double)(csz -1) ;
     stdy = sqrt( stdy/ dof  ) ;
     return stdy ;
}

