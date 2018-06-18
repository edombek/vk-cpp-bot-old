#include "common.h"
#include "py.h"

py::dict pyF::toPythonDict(table map) {
	typename table::iterator iter;
	py::dict dictionary;
	for (iter = map.begin(); iter != map.end(); ++iter) {
		dictionary[iter->first] = iter->second;
	}
	return dictionary;
}

table pyF::toTable(py::dict dict)
{
	table dic;
	for(unsigned int i =0; i < py::len(dict.keys()); i++)
		dic[py::extract<string>(dict.keys()[i])] = py::extract<string>(dict[dict.keys()[i]]);
	return dic;
}

string pyF::getTime()
{
	return other::getTime();
}

string pyF::vk_send(string method, py::dict params, bool sendtoken)
{
	return vk::send(method, pyF::toTable(params), sendtoken).dump(4);
}

string pyF::net_send(string url, py::dict param, bool post)
{
	return net::send(url, pyF::toTable(param), post);
}

// decode a Python exception into a string
string pyF::error()
{
	PyObject *exc,*val,*tb;
	py::object formatted_list, formatted;
	PyErr_Fetch(&exc,&val,&tb);
	py::handle<> hexc(exc),hval(py::allow_null(val)),htb(py::allow_null(tb)); 
	py::object traceback(py::import("traceback"));
	py::object format_exception(traceback.attr("format_exception"));
	formatted_list = format_exception(hexc,hval,htb);
	formatted = py::str("").join(formatted_list);
	py::handle_exception();
	PyErr_Clear();
	return py::extract<string>(formatted);
}