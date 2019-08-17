#include <iostream>
#include <fstream>

using std::cout;     using std::endl;

void except()
{
   std::ifstream ifile(__FILE__, std::ios::binary);

   // get length of file:
   ifile.seekg (0, ios::end);
   int fsize = ifile.tellg();
   ifile.seekg (0, ios::beg);

   cout<< "fsize = " << fsize << " bytes" <<endl;

   // allocate memory:
   char* buffer = new char [fsize];

   // set exception mask for ifile
   ifile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

   int nread = fsize + 1;
   // read data as a block of nread bytes
   try {
      ifile.read (buffer,nread);
   }
   catch (std::ifstream::failure e) {
      cout<< "Error reading " << nread << " bytes from " << __FILE__ << ": " << e.what() <<endl;
      return;
   }

   ifile.close();

   cout.write (buffer,fsize);
}
