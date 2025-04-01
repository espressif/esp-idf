/*******************************************************************************/
/*******************************************************************************/
 // Declare a void pointer
 void *ptr;

 // Determine the size of the void pointer
 size_t size = sizeof(ptr);

/*******************************************************************************/
/*******************************************************************************/
 // What is this?

 int (*a[10])(int);

/*******************************************************************************/
/*******************************************************************************/
// Define a structure with padding
struct Example {
    char a;     // 1 byte
    int b;      // 4 bytes
    char c;     // 1 byte
};

// What is the size of the struct?
// How the memory layout will look like? 
// How you can avoid padding?

/*******************************************************************************/
/*******************************************************************************/
/*
Assessment Requirement:
1. Implement a function that returns the address of a static variable.
2. The static variable should retain its value across multiple function calls.
3. The function should be memory-efficient, suitable for embedded systems (e.g., 8-bit MCUs).
*/
/*******************************************************************************/
/*******************************************************************************/

int main() 
{  
    foo();
    return 0;
}
  
  
void foo(void)
{	
    unsigned int a = 6;
    int b = -20; (a+b > 6) ? printf("> 6") : printf("<= 6");
}

// What os the output of this?
/*******************************************************************************/
/*******************************************************************************/