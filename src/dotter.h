//
// Created by Ivan Jurin on 1/9/17.
//

#pragma once

void dotter(const std::string & readsPath, const std::string &referenceSequencePath){

    std::stringstream ss;
    ss<<"java -cp "
      <<GEPARD_JAR
      <<" org.gepard.client.cmdline.CommandLine  -matrix "
      <<GEPARD_MAT
      <<" -outfile /tmp/output.jpg -seq1 "
      <<readsPath
      <<" -seq2 "
      <<referenceSequencePath
      <<"; open /tmp/output.jpg";
    std::cout<<ss.str()<<std::endl;
    system(ss.str().c_str());
}