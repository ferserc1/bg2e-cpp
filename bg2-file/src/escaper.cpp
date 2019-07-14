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


#include <bg2file/escaper.hpp>

#include <cassert>
#include <stack>
#include <sstream>
#include <list>
#include <iomanip>
#include <fstream>

namespace bg2file {
namespace json {

std::string Escaper::EscapeMinimumCharacters(const std::string &str) {
	std::stringstream result;

	// For each character in the string.
	for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
		if (*i == Strings::Std::QUOTATION_MARK) {
			result << Strings::Json::QUOTATION_MARK;

		}
		else if (*i == Strings::Std::REVERSE_SOLIDUS) {
			result << Strings::Json::REVERSE_SOLIDUS;

		}
		else if (*i == Strings::Std::BACKSPACE) {
			result << Strings::Json::BACKSPACE;

		}
		else if (*i == Strings::Std::FORM_FEED) {
			result << Strings::Json::FORM_FEED;

		}
		else if (*i == Strings::Std::LINE_FEED) {
			result << Strings::Json::LINE_FEED;

		}
		else if (*i == Strings::Std::CARRIAGE_RETURN) {
			result << Strings::Json::CARRIAGE_RETURN;

		}
		else if (*i == Strings::Std::TAB) {
			result << Strings::Json::TAB;

		}
		else if (*i >= '\0' && *i <= '\x1f') {
			result << EscapeToUnicode(*i);

		}
		else {
			result << *i;
		}
	}

	return result.str();
}

std::string Escaper::EscapeAllCharacters(const std::string &str) {
	std::stringstream result;

	// For each character in the string.
	for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
		if (*i == Strings::Std::QUOTATION_MARK) {
			result << Strings::Json::QUOTATION_MARK;

		}
		else if (*i == Strings::Std::REVERSE_SOLIDUS) {
			result << Strings::Json::REVERSE_SOLIDUS;

		}
		else if (*i == Strings::Std::SOLIDUS) {
			result << Strings::Json::SOLIDUS;

		}
		else if (*i == Strings::Std::BACKSPACE) {
			result << Strings::Json::BACKSPACE;

		}
		else if (*i == Strings::Std::FORM_FEED) {
			result << Strings::Json::FORM_FEED;

		}
		else if (*i == Strings::Std::LINE_FEED) {
			result << Strings::Json::LINE_FEED;

		}
		else if (*i == Strings::Std::CARRIAGE_RETURN) {
			result << Strings::Json::CARRIAGE_RETURN;

		}
		else if (*i == Strings::Std::TAB) {
			result << Strings::Json::TAB;

		}
		else if (*i >= '\0' && *i <= '\x1f') {
			result << EscapeToUnicode(*i);
		}
		else {
			result << *i;
		}

	}

	return result.str();
}


std::string Escaper::EscapeToUnicode(char charToEscape) {
	std::stringstream result;

	if (charToEscape >= '\0' && charToEscape <= '\x1f') {
		result << "\\u00";
		result << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(charToEscape);
	}

	return result.str();
}



Escaper::Escaper() : afterBackSlash(false), inString(false) {
}

std::streambuf::int_type Escaper::operator()(std::streambuf &destination,
                                                std::streambuf::int_type character) {
    bool notEscaped = true;
    std::streambuf::char_type tmpChar = std::streambuf::traits_type::to_char_type(character);

    // If we encounter a quotation mark.
    if (tmpChar == Structural::BEGIN_END_STRING) {
        // If we're not in a string, we change that. If we're in a string,
        // we change that only if we're not after an escape back slash.
        inString = !inString || (afterBackSlash);

    } else if (inString && !afterBackSlash) {
        // If we are in a string definition and we're not after a backslash
        // escape.
        if (tmpChar == Strings::Std::REVERSE_SOLIDUS) {
            destination.sputn(Strings::Json::REVERSE_SOLIDUS.c_str(), Strings::Json::REVERSE_SOLIDUS.size());
            notEscaped = false;

        } else if (tmpChar == Strings::Std::BACKSPACE) {
            destination.sputn(Strings::Json::BACKSPACE.c_str(), Strings::Json::BACKSPACE.size());
            notEscaped = false;

        } else if (tmpChar == Strings::Std::FORM_FEED) {
            destination.sputn(Strings::Json::FORM_FEED.c_str(), Strings::Json::FORM_FEED.size());
            notEscaped = false;

        } else if (tmpChar == Strings::Std::LINE_FEED) {
            destination.sputn(Strings::Json::LINE_FEED.c_str(), Strings::Json::LINE_FEED.size());
            notEscaped = false;

        } else if (tmpChar == Strings::Std::TAB) {
            destination.sputn(Strings::Json::TAB.c_str(), Strings::Json::TAB.size());
            notEscaped = false;

        } else if (tmpChar >= '\0' && tmpChar <= '\x1f') {
            std::string tmp(Escaper::EscapeToUnicode(tmpChar));
            destination.sputn(tmp.c_str(), tmp.size());
            notEscaped = false;
        }

    }

    // We determine if we start a backslash escape or not.
    afterBackSlash = inString && !afterBackSlash && (tmpChar == Strings::Json::Escape::BEGIN_ESCAPE);
    return (notEscaped) ? (destination.sputc(tmpChar)) : (0);
}

}
}