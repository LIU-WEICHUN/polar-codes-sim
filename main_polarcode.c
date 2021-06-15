#include <string.h>
int Menu();
void printHelp();


int main(int argc, char const *argv[])
{
    int index;
    if (argc == 1){                                                   
        index = Menu();                                                    
    }  
    else if (argc >= 2) {                                                                                                                                       
        if (strcmp(argv[1], "-h") == 0 ||                         
            strcmp(argv[1], "--help") == 0) {                          
            printHelp();                                               
            return 0;                                                  
        }                                                              
        // Run in quiet mode.                                          
        else if(strcmp(argv[1], "-s") == 0                          
                ||strcmp(argv[1], "--show") == 0) 
        {
                                                                           
        }else if(strcmp(argv[1], "--input") == 0                       
                ||strcmp(argv[1], "-i") == 0){                      
            ;                                                
        }else if (strcmp(argv[1], "wrong") == 0) {                      
            ;                                            
        }else {                                                         
            printf("Wrong arguments\n");                      
            printHelp();                                               
            return 1;                                                  
        }                                                              
    }                                                                  
    return 0;
}
