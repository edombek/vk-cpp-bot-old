#ifndef PY_H_INCLUDED
#define PY_H_INCLUDED

#include <boost/python.hpp>
#include <Python.h>

namespace py = boost::python;

namespace pyF
{
    py::dict toPythonDict(table map);
    table toTable(py::dict dict);
    string getTime();

    string vk_send(string method, py::dict params = {}, bool sendtoken = true);
    string net_send(string url, py::dict param = {}, bool post = true);

	string error();
}

#endif // PY_H_INCLUDED
