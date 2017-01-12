//
// Created by Ivan Jurin on 1/9/17.
//

#include <iostream>
#include <sstream>

void dotter( const std::string & readsPath, const std::string &referenceSequencePath){
#ifdef ENABLE_DOTTER
    std::cout << "Visualize with dotter" << std::endl;
    std::stringstream ss;
    ss<<"java -cp "
      <<GEPARD_JAR
      <<" org.gepard.client.cmdline.CommandLine  -matrix "
      <<GEPARD_MAT
      <<" -outfile /tmp/output.jpg -seq1 "
      <<referenceSequencePath
      <<" -seq2 "
      <<readsPath
      <<"; open /tmp/output.jpg";
    std::cout<<ss.str()<<std::endl;
    system(ss.str().c_str());
#endif
}