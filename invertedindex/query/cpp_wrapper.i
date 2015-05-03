/* example.i */
%module cpp_wrapper
%{
/* Put header files here or function declarations like below */
extern void get_result(char* query);
%}

extern void get_result(char* query);