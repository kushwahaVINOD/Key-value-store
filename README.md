# Key-value-store
Team-mates:
Basant 19305R006
Raja 193050090
Vinod 190350059
Billoh 193051001

Server can be run using 
./server
./server -threadPoolSize=YYY -numSetsInCache=ZZZ -sizeOfSet=AAA (Case sensitive)
No parameter is mandatory
Default values of threadpoolsize = 20, number of sets = 2, size of set = 4. These are the default values if command line parameters are not given to change these

For Inet address and port, please change in constants.cpp file. We are unable to provide command line support for them. Both server and client access this constants.cpp file

client can be run using 
./client - for interactive mode
./client <filepath> for batch mode. A file called batchResponses.txt is created that contains responses

file0, file1, ... filen are created that are corresponding to the number of sets
There is a 'Total' file that contains all the key-value pairs which is the persistence storage for the KV-Cache
