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

py::list pyF::toPythonList(args a)
{
    py::list out;
    for(auto s: a)
        out.append(s);
    return out;
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

int pyF::user_get(int id)
{
	message m;
	m.user_id=id;
	m.chat_id=0;
	return module::user::get(&m);
}

#include <iostream>
// decode a Python exception into a string
string pyF::error()
{
	PyObject *exc, *val, *tb;
    PyErr_Fetch(&exc, &val, &tb);
    PyErr_NormalizeException(&exc, &val, &tb);
    py::handle<> hexc(exc), hval(py::allow_null(val)), htb(py::allow_null(tb));
    py::object format_exception = py::import("traceback").attr("format_exception");
    return py::extract<string>(py::str("").join(format_exception(hexc, hval, htb)));
}
