    1. Vs Code Remote Server in WSL 2 is required to run this code.
    2. because sys/socket.h is not supported in windows os 
    3. when we open via remote server , the include paths will be of linux ubuntu
    4. ubuntu has its own include paths library
    5. how include path , by writing command , (sudo apt install build-essential)
    6. this above command will install the gcc , g++ compiler and other libraries
    7. here localhost:8080 is same in ubuntu and windows so we can use it