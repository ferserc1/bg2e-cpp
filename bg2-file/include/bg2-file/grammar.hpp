/*
 *	bg2 engine license
 *	Copyright (c) 2016 Fernando Serrano <ferserc1@gmail.com>
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *	of the Software, and to permit persons to whom the Software is furnished to do
 *	so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 *	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 *	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *	OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*
 Some parts of the json namespace are extracted and adapted from JsonBox:
 https://github.com/anhero/JsonBox
 
 Copyright (c) 2011 Anhero Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

 */
 
#ifndef _bg2model_json_grammar_hpp_
#define _bg2model_json_grammar_hpp_
 
#include <string>

namespace bg2file {
namespace json {
 
// Structural characters.
namespace Structural {
	const char BEGIN_ARRAY = '[';
	const char BEGIN_OBJECT = '{';
	const char END_ARRAY = ']';
	const char END_OBJECT = '}';
	const char NAME_SEPARATOR = ':';
	const char VALUE_SEPARATOR = ',';
	const char BEGIN_END_STRING = '"';
}

// Whitespace characters.
namespace Whitespace {
	const char SPACE = ' ';
	const char HORIZONTAL_TAB = '\t';
	const char NEW_LINE = '\n';
	const char CARRIAGE_RETURN = '\r';
}

// Literals.
namespace Literals {
	const std::string FALSE_STRING = "false";
	const std::string TRUE_STRING = "true";
	const std::string NULL_STRING = "null";
}

// Numbers.
namespace Numbers {
	const std::string DIGITS = "0123456789ABCDEFabcdef";
	const char DECIMAL_POINT = '.';
	const char LOWER_EXP = 'e';
	const char UPPER_EXP = 'E';
	const char MINUS = '-';
	const char PLUS = '+';
}

// Strings.
namespace Strings {
	// C++ string characters.
	namespace Std {
		const char QUOTATION_MARK = '"';
		const char REVERSE_SOLIDUS = '\\';
		const char SOLIDUS = '/';
		const char BACKSPACE = '\b';
		const char FORM_FEED = '\f';
		const char LINE_FEED = '\n';
		const char CARRIAGE_RETURN = '\r';
		const char TAB = '\t';
	}
	// Json escape strings.
	namespace Json {
		const std::string QUOTATION_MARK = "\\\"";
		const std::string REVERSE_SOLIDUS = "\\\\";
		const std::string SOLIDUS = "\\/";
		const std::string BACKSPACE = "\\b";
		const std::string FORM_FEED = "\\f";
		const std::string LINE_FEED = "\\n";
		const std::string CARRIAGE_RETURN = "\\r";
		const std::string TAB = "\\t";
		const std::string BEGIN_UNICODE = "\\u";
		namespace Escape {
			const char BEGIN_ESCAPE = '\\';
			const char QUOTATION_MARK = '"';
			const char REVERSE_SOLIDUS = '\\';
			const char SOLIDUS = '/';
			const char BACKSPACE = 'b';
			const char FORM_FEED = 'f';
			const char LINE_FEED = 'n';
			const char CARRIAGE_RETURN = 'r';
			const char TAB = 't';
			const char BEGIN_UNICODE = 'u';
		}
	}
}

}
}
 
#endif
 