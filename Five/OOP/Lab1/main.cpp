#include <pybind11/pybind11.h>

#include <string>

#include "MyString.hpp"

namespace py = pybind11;

PYBIND11_MODULE(my_string, module)
{
    module.doc() = "Python wrapper for the MyString C++ class";

    py::class_<MyString>(module, "MyString")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def(py::init<const std::string&, int>())
        .def(py::init<const MyString&>())
        .def(py::init<const MyString&, int>())

        .def_readwrite("capacity_", &MyString::capacity_)
        .def_readwrite("size_", &MyString::size_)

        .def("clear", &MyString::clear)
        .def("shrink_to_fit", &MyString::shrink_to_fit)
        .def("c_str", [](const MyString& value) {
            return std::string(value.str);
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
        .def("insert", [](MyString& value, int index, const std::string& source, int count) {
            value.insert(index, source.c_str(), count);
        })
        .def("insert", [](MyString& value, int index, const std::string& source,
                          int source_index, int count) {
            value.insert(index, source.c_str(), source_index, count);
        })

        .def("append", [](MyString& value, int count, char ch) {
            value.append(count, ch);
        })
        .def("append", [](MyString& value, const std::string& source) {
            value.append(source.c_str());
        })
        .def("append", [](MyString& value, const std::string& source, int count) {
            value.append(source.c_str(), count);
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
                           const std::string& source, int source_count) {
            value.replace(index, count, source.c_str(), source_count);
        })
        .def("replace", [](MyString& value, int index, int count,
                           const std::string& source, int source_index,
                           int source_count) {
            value.replace(index, count, source.c_str(), source_index, source_count);
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
        .def("find", [](MyString& value, const std::string& source, int index) {
            return value.find(source.c_str(), index);
        })

        .def("__getitem__", [](MyString& value, int index) -> char& {
            return value[index];
        }, py::return_value_policy::reference_internal)
        .def("__setitem__", [](MyString& value, int index, char ch) {
            value[index] = ch;
        })
        .def("__len__", &MyString::size)
        .def("__str__", [](const MyString& value) {
            return std::string(value.str);
        })
        .def("__repr__", [](const MyString& value) {
            return "MyString(" + std::to_string(value.size_) + ")";
        })
        .def("__iadd__", [](MyString& value, const std::string& source) -> MyString& {
            value.append(source.c_str());
            return value;
        }, py::return_value_policy::reference_internal)
        .def("__add__", [](const MyString& value, const std::string& source) {
            MyString result(value);
            result.append(source.c_str());
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

