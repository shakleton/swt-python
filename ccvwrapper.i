%module ccvwrapper
%begin %{

%}
%{
#include "ccvwrapper.h"
%}

%typemap(out) int* swt {
	int i;
	$result = PyList_New($1[0]);
	for(i = 0; i < $1[0]; i++){
		PyObject *o = PyInt_FromLong($1[i+1]);
		PyList_SetItem($result, i, o);
	}
	free($1);
}

%typemap(in) (char *bytes, int array_length) {
    Py_ssize_t len;
    PyBytes_AsStringAndSize($input, &$1, &len);
    $2 = (int)len;
}

%include "ccvwrapper.h"


