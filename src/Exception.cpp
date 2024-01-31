#include "./Exception.hpp"

using namespace adapsurf;

Exception::Exception(Error error, string& description) : _error{error}, _descr{description}
{
}

Exception::Error Exception::getErrorCode() const
{
	return _error;
}

const string& Exception::getErrorDescr() const
{
	return _descr;
}
