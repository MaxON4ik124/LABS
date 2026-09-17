#include <pybind11/pybind11.h>

#include <string>

#ifndef IMYSTRING
#define IMYSTRING "MyString.h"
#endif

#include IMYSTRING

namespace py = pybind11;

PYBIND11_MODULE(my_string, module)
{
    module.doc() = "Python wrapper for the MyString C++ class";

    py::class_<MyString>(module, "MyString")
        .def(py::init<>())
        .def(py::init<const MyString&>())
        .def(py::init<const MyString&, int>())
        .def(py::init<int, char>())
        .def(py::init([](const std::string& value) {
            return MyString(value.c_str());
        }))
        .def(py::init([](const std::string& value, int count) {
            return MyString(value.c_str(), count);
        }))

        .def("clear", &MyString::clear)
        .def("shrink_to_fit", &MyString::shrink_to_fit)
        .def("set_str", &MyString::set_str)
        .def("set_size", &MyString::set_size)
        .def("set_capacity", &MyString::set_capacity)
        .def("assign", [](MyString& value, const std::string& source) {
            value = source;
        })
        .def("assign", [](MyString& value, const MyString& source) {
            value = source;
        })
        .def("assign", [](MyString& value, char source) {
            value = source;
        })
        .def("c_str", [](const MyString& value) {
            return std::string(value.c_str());
        })
        .def("size", &MyString::size)
        .def("capacity", &MyString::capacity)
        .def("empty", &MyString::empty)


        .def("insert", [](MyString& value, int index, int count, char ch) {
            value.insert(index, count, ch);
        })
        .def("insert", [](MyString& value, int index, const std::string& source) {
            value.insert(index, source.c_str());
        })
        .def("insert", [](MyString& value, int index, const MyString& source) {
            value.insert(index, source);
        })
        .def("insert", [](MyString& value, int index, const std::string& source, int count) {
            value.insert(index, source.c_str(), count);
        })
        .def("insert", [](MyString& value, int index, const MyString& source, int count) {
            value.insert(index, source, count);
        })
        .def("insert", [](MyString& value, int index, const std::string& source,
                          int source_index, int count) {
            value.insert(index, source.c_str(), source_index, count);
        })
        .def("insert", [](MyString& value, int index, const MyString& source,
                          int source_index, int count) {
            value.insert(index, source, source_index, count);
        })


        .def("append", [](MyString& value, int count, char ch) {
            value.append(count, ch);
        })
        .def("append", [](MyString& value, const std::string& source) {
            value.append(source.c_str());
        })
        .def("append", [](MyString& value, const MyString& source) {
            value.append(source);
        })
        .def("append", [](MyString& value, const std::string& source, int count) {
            value.append(source.c_str(), count);
        })
        .def("append", [](MyString& value, const MyString& source, int count) {
            value.append(source, count);
        })
        .def("append", [](MyString& value, const std::string& source,
                          int source_index, int count) {
            value.append(source.c_str(), source_index, count);
        })

        .def("erase", &MyString::erase)


        .def("replace", [](MyString& value, int index, int count,
                           const std::string& source) {
            value.replace(index, count, source.c_str());
        })
        .def("replace", [](MyString& value, int index, int count,
                           const MyString& source) {
            value.replace(index, count, source);
        })
        .def("replace", [](MyString& value, int index, int count,
                           const std::string& source, int source_count) {
            value.replace(index, count, source.c_str(), source_count);
        })
        .def("replace", [](MyString& value, int index, int count,
                           const MyString& source, int source_count) {
            value.replace(index, count, source, source_count);
        })
        .def("replace", [](MyString& value, int index, int count,
                           const std::string& source, int source_index,
                           int source_count) {
            value.replace(index, count, source.c_str(), source_index, source_count);
        })
        .def("replace", [](MyString& value, int index, int count,
                           const MyString& source, int source_index,
                           int source_count) {
            value.replace(index, count, source, source_index, source_count);
        })

        .def("substr", [](MyString& value, int index) {
            return value.substr(index);
        })
        .def("substr", [](MyString& value, int index, int count) {
            return value.substr(index, count);
        })

        .def("compare", [](MyString& value, MyString& other) {
            return value.compare(other);
        })
        .def("find", [](MyString& value, const std::string& source) {
            return value.find(source.c_str());
        })
        .def("find", [](MyString& value, const MyString& source) {
            return value.find(source);
        })
        .def("find", [](MyString& value, const std::string& source, int index) {
            return value.find(source.c_str(), index);
        })
        .def("find", [](MyString& value, const MyString& source, int index) {
            return value.find(source, index);
        })

        
        .def("__getitem__", [](MyString& value, int index) {
            return value[index];
        })
        .def("__setitem__", [](MyString& value, int index, char ch) {
            value[index] = ch;
        })
        .def("__len__", &MyString::size)
        .def("__str__", [](const MyString& value) {
            return std::string(value.c_str());
        })
        .def("__repr__", [](const MyString& value) {
            return "MyString(" + std::to_string(value.size()) + ")";
        })

        .def("__iadd__", [](MyString& value, const std::string& source) -> MyString& {
            value += source;
            return value;
        }, py::return_value_policy::reference_internal)
        .def("__iadd__", [](MyString& value, const MyString& source) -> MyString& {
            value += source;
            return value;
        }, py::return_value_policy::reference_internal)
        .def("__add__", [](const MyString& left, const std::string& right) {
            MyString result(left);
            result += right;
            return result;
        })
        .def("__add__", [](const MyString& left, const MyString& right) {
            MyString result(left);
            result += right;
            return result;
        })
        .def("__radd__", [](const MyString& right, const std::string& left) {
            MyString result(left.c_str());
            result += right;
            return result;
        })

        .def("__eq__", [](MyString& value, MyString& other) {
            return value == other;
        })
        .def("__ne__", [](MyString& value, MyString& other) {
            return value != other;
        })
        .def("__lt__", [](MyString& value, MyString& other) {
            return value < other;
        })
        .def("__le__", [](MyString& value, MyString& other) {
            return value <= other;
        })
        .def("__gt__", [](MyString& value, MyString& other) {
            return value > other;
        })
        .def("__ge__", [](MyString& value, MyString& other) {
            return value >= other;
        });

    module.def("pstr", &pstr);
}
