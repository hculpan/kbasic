#ifndef _VALUE_HPP_
#define _VALUE_HPP_

#include "main.hpp"

enum ValueType { vt_null, vt_string, vt_integer, vt_real, vt_bool };


class Value {
    public:
        Value();
        Value(string s);
        Value(int i);
        Value(float f);
        Value(double f);
        Value(bool b);

        bool isNull() const { return m_type == vt_null; }
        bool isNumeric() const { return m_type == vt_integer || m_type == vt_real; }
        bool isString() const { return m_type == vt_string; }
        bool isBoolean() const { return m_type == vt_bool; }
        bool isInteger() const { return m_type == vt_integer; }
        bool isReal() const { return m_type == vt_real; }

        bool equals(const Value &v);
        bool isGreaterThan(const Value &v);
        bool isLessThan(const Value &v);

        string string() const;
        bool boolean() const;
        int integer() const;
        float real() const;
        ValueType type() const { return m_type; }

    private:
        std::string svalue;
        int ivalue;
        float rvalue;
        bool bvalue;

        ValueType m_type;
};

#endif