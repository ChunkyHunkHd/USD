//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef TF_STRINGUTILS_H
#define TF_STRINGUTILS_H

#include "pxr/base/arch/attributes.h"
#include "pxr/base/arch/inttypes.h"

#include "pxr/base/tf/enum.h"

#include <boost/type_traits/is_enum.hpp>
#include <boost/utility/enable_if.hpp>

#include <cstdarg>
#include <cstring>
#include <list>
#include <set>
#include <sstream>
#include <string>
#include <vector>

/*!
 * \file stringUtils.h
 * \ingroup group_tf_String
 * \brief Definitions of basic string utilities in tf.
 */



/*!
 * \brief Returns a string formed by a printf()-like specification.
 * \ingroup group_tf_String
 *
 * \c TfStringPrintf() is a memory-safe way of forming a string using
 * printf()-like formatting.  For example,
 * \code
 *  string formatMsg(const string& caller, int i, double val[])
 *  {
 *     return TfStringfPrintf("%s: val[%d] = %g\n", caller.c_str(), i, val[i]);
 *  }
 * \endcode
 *
 * The function is safe only to the extent that the arguments match
 * the formatting string.  In particular, be careful to pass strings
 * themselve into \c TfStringPrintf() as in the above example
 * (i.e. \c caller.c_str() as opposed to just passing \c caller).
 *
 * \note \c TfStringPrintf() is just a wrapper for \c ArchStringPrintf().
 */
std::string TfStringPrintf(const char *fmt, ...)
#ifndef doxygen
    ARCH_PRINTF_FUNCTION(1, 2)
#endif /* doxygen */
    ;

/*!
 * \brief Returns a string formed by a printf()-like specification.
 * \ingroup group_tf_String
 *
 * \c TfVStringPrintf() is equivalent to \c TfStringPrintf() except that
 * it is called with a \c va_list instead of a variable number of
 * arguments. \c TfVStringPrintf() does not call the \c va_end macro.
 * Consequently, the value of \c ap is undefined after the call.
 * A functions that calls \c TfVStringPrintf() should call \c va_end(ap)
 * itself afterwards.
 *
 * \note \c TfVStringPrintf() is just a wrapper for \c ArchVStringPrintf().
 */
std::string TfVStringPrintf(const std::string& fmt, va_list ap);

/*!
 * \brief Bloat-avoidance version of TfVStringPrintf()
 * \ingroup group_tf_String
 */
std::string TfVStringPrintf(const char *fmt, va_list ap)
#ifndef doxygen
    ARCH_PRINTF_FUNCTION(1, 0)
#endif /* doxygen */
    ;

/*!
 * \brief Safely create a std::string from a (possibly NULL) char*.
 * \ingroup group_tf_String
 *
 * If \p ptr is NULL, the empty string is safely returned.
 */
inline std::string TfSafeString(const char* ptr) {
    return ptr ? std::string(ptr) : std::string();
}

//! Returns the given integer as a string.
// \ingroup group_tf_String
inline std::string TfIntToString(int i) {
    return TfStringPrintf("%d", i);
}

/*!
 * \brief Converts text string to double
 * \ingroup group_tf_String
 *
 * This method converts strings to floating point numbers.  It is
 * similar to libc's atof(), but performs the conversion much more quickly.
 *
 * It expects somewhat valid input: it will continue parsing the input
 * until it hits an unrecognized character, as described by the regexp
 * below, and at that point will return the results up to that point.
 *
 *  (-?[0-9]+(\.[0-9]*)?|-?\.[0-9]+)([eE][-+]?[0-9]+)?
 *
 * It will not check to see if there is any input at all, or whitespace
 * after the digits.  Ie:
 *    TfStringToDouble("") == 0.0
 *    TfStringToDouble("blah") == 0.0
 *    TfStringToDouble("-") == -0.0
 *    TfStringToDouble("1.2foo") == 1.2
 *
 * \note \c TfStringToDouble is a wrapper around the extern-c TfStringToDouble
 */
double TfStringToDouble(const std::string& txt);

/*! See \c TfStringToDouble(const string& text);
 * \ingroup group_tf_String
 */
double TfStringToDouble(const char *text);

/*!
 * Convert a sequence of digits in \p txt to a long int value.  Caller is
 * responsible for ensuring that \p txt has content matching:
 *
 * \code
 * -?[0-9]+
 * \endcode
 *
 * If the digit sequence's value is out of range, set \p *outOfRange to true (if
 * \p outOfRange is not NULL) and return either std::numeric_limits<long>::min()
 * or max(), whichever is closest to the true value.
 */
long TfStringToLong(const std::string &txt, bool *outOfRange=NULL);

/*! See \c TfStringToLong()
 * \ingroup group_tf_String
 */
long TfStringToLong(const char *txt, bool *outOfRange=NULL);

/*!
 * Convert a sequence of digits in \p txt to an unsigned long value.  Caller is
 * responsible for ensuring that \p txt has content matching:
 *
 * \code
 * [0-9]+
 * \endcode
 *
 * If the digit sequence's value is out of range, set \p *outOfRange to true (if
 * \p outOfRange is not NULL) and return std::numeric_limits<unsigned
 * long>::max().
 */
unsigned long TfStringToULong(const std::string &txt, bool *outOfRange=NULL);

/*! See \c TfStringToULong()
 * \ingroup group_tf_String
 */
unsigned long TfStringToULong(const char *txt, bool *outOfRange=NULL);

/*!
 * Convert a sequence of digits in \p txt to an int64_t value.  Caller must
 * ensure that \p txt has content matching:
 *
 * \code
 * -?[0-9]+
 * \endcode
 *
 * If the digit sequence's value is out of range, set \p *outOfRange to true (if
 * \p outOfRange is not NULL) and return either
 * std::numeric_limits<int64_t>::min() or max(), whichever is closest to the
 * true value.
 */
int64_t TfStringToInt64(const std::string &txt, bool *outOfRange=NULL);

/*! See \c TfStringToInt64()
 * \ingroup group_tf_String
 */
int64_t TfStringToInt64(const char *txt, bool *outOfRange=NULL);

/*!
 * Convert a sequence of digits in \p txt to a uint64_t value.  Caller is
 * responsible for ensuring that \p txt has content matching:
 *
 * \code
 * [0-9]+
 * \endcode
 *
 * If the digit sequence's value is out of range, set \p *outOfRange to true (if
 * \p outOfRange is not NULL) and return std::numeric_limits<unsigned
 * long>::max().
 */
uint64_t TfStringToUInt64(const std::string &txt, bool *outOfRange=NULL);

/*! See \c TfStringToUInt64()
 * \ingroup group_tf_String
 */
uint64_t TfStringToUInt64(const char *txt, bool *outOfRange=NULL);

inline bool
Tf_StringStartsWithImpl(char const *s, size_t slen,
                        char const *prefix, size_t prelen)
{
    return slen >= prelen and strncmp(s, prefix, prelen) == 0;
}

//! Returns true if \p s starts with \p prefix.
// \ingroup group_tf_String
inline bool
TfStringStartsWith(const std::string& s, const char *prefix)
{
    return Tf_StringStartsWithImpl(
        s.c_str(), s.length(), prefix, strlen(prefix));
}
//! \overload
inline bool
TfStringStartsWith(const std::string& s, const std::string& prefix) {
    return TfStringStartsWith(s, prefix.c_str());
}
//! \overload
bool TfStringStartsWith(const std::string &s, const class TfToken& prefix);

inline bool
Tf_StringEndsWithImpl(char const *s, size_t slen,
                      char const *suffix, size_t suflen)
{
    return slen >= suflen and strcmp(s + (slen - suflen), suffix) == 0;
}

//! Returns true if \p s ends with \p suffix.
// \ingroup group_tf_String
inline bool TfStringEndsWith(const std::string& s, const char *suffix)
{
    return Tf_StringEndsWithImpl(s.c_str(), s.length(),
                                 suffix, strlen(suffix));
}
//! \overload
inline bool
TfStringEndsWith(const std::string& s, const std::string& suffix)
{
    return TfStringEndsWith(s, suffix.c_str());
}
//! \overload
bool TfStringEndsWith(const std::string &s, const class TfToken& suffix);

//! Returns true if \p s contains \p substring.
// \ingroup group_tf_String
bool TfStringContains(const std::string& s, const char *substring);
//! \overload
inline bool
TfStringContains(const std::string &s, const std::string &substring) {
    return TfStringContains(s, substring.c_str());
}
//! \overload
bool TfStringContains(const std::string &s, const class TfToken& substring);

/*!
 * \brief Makes all characters in \p source lowercase, and returns the result.
 * \ingroup group_tf_String
 */
std::string TfStringToLower(const std::string& source);

/*!
 * \brief Makes all characters in \p source uppercase, and returns the result.
 * \ingroup group_tf_String
 */
std::string TfStringToUpper(const std::string& source);

/*!
 * \brief Returns a copy of the \p source string with only its first character
 * capitalized.  This emulates the behavior of Python's \c str.capitalize().
 * \ingroup group_tf_String
 */
std::string TfStringCapitalize(const std::string& source);

/*!
 * \brief Trims characters (by default, whitespace) from the left.
 * \ingroup group_tf_String
 *
 * Characters from the beginning of \p s are removed until a
 * character not in \p trimChars is found; the result is returned.
 */
std::string TfStringTrimLeft(const std::string& s,
                             const char* trimChars = " \n\t\r");

/*!
 * \brief Trims characters (by default, whitespace) from the right.
 * \ingroup group_tf_String
 *
 * Characters at the end of \p s are removed until a
 * character not in \p trimChars is found; the result is returned.
 */
std::string TfStringTrimRight(const std::string& s,
                              const char* trimChars = " \n\t\r");

/*!
 * \brief Trims characters (by default, whitespace) from the beginning and
 * end of string.
 * \ingroup group_tf_String
 *
 * Characters at the beginning and end of \p s are removed until a
 * character not in \p trimChars is found; the result is returned.
 */
std::string TfStringTrim(const std::string& s,
                         const char* trimChars = " \n\t\r");

/*!
 * \brief Returns the common prefix of the input strings, if any.
 * \ingroup group_tf_String
 *
 * Copies of the input strings are compared.  Returns a new string which is
 * the longest prefix common to both input strings.  If the strings have no
 * common prefix, an empty string is returned.
 */
std::string TfStringGetCommonPrefix(std::string a, std::string b);

/*!
 * \brief Returns the suffix of a string
 * \ingroup group_tf_String
 *
 * Returns characters after the final character \c delimiter (default ".")
 * of a string.  Thus suffix of "abc.def" is "def" using "." as the
 * delimiter.  If the delimiter does not occur, the empty string is returned.
 */
std::string TfStringGetSuffix(const std::string& name, char delimiter = '.');

/*!
 * \brief Returns the everthing up to the suffix of a string
 * \ingroup group_tf_String
 *
 * Returns characters before the final character \c delimiter (default ".")
 * of a string.  Thus not-suffix of "abc.def" is "abc" using "." as the
 * delimiter.  If the delimiter does not occur, the original string is returned.
 */
std::string TfStringGetBeforeSuffix(const std::string& name, char delimiter = '.');

//! Returns the base name of a file (final component of the path).
// \ingroup group_tf_String
std::string TfGetBaseName(const std::string& fileName);

/*!
 * \brief Returns the path component of a file (complement of TfGetBaseName()).
 * \ingroup group_tf_String
 *
 * The returned string ends in a '/', unless there were no slashes in
 * \c fileName, in which case the empty string is returned.  In particular,
 * \c TfGetPathName(s)+TfGetBaseName(s) == \c s for any string \c s (
 * as long as \c s doesn't end with multiple adjacent slashes,
 * which is illegal).
 */
std::string TfGetPathName(const std::string& fileName);

/*!
 * \brief Replaces all occurences of string \p from with \p to in \p source
 * \ingroup group_tf_String
 *
 * Returns a new string which is created by copying \p string
 * and replacing every occurence of \p from with \p to.
 * Correctly handles the case in which \p to contains \p from.
 */
std::string TfStringReplace(const std::string& source, const std::string& from,
                            const std::string& to);

/*!
 * \brief Catenates the strings (\p begin, \p end), with default separator.
 * \ingroup group_tf_String
 *
 * Returns the catenation of the strings in the range \p begin to \p end, with
 * \p separator (by default, a space) added between each successive pair of
 * strings.
 */
template <class ForwardIterator>
std::string TfStringJoin(
    ForwardIterator begin, ForwardIterator end,
    const char* separator = " ")
{
    if (begin == end)
        return std::string();

    size_t distance = std::distance(begin, end);
    if (distance == 1)
        return *begin;

    std::string retVal;

    size_t sum = 0;
    ForwardIterator i = begin;
    for (i = begin; i != end; ++i)
        sum += i->size();
    retVal.reserve(sum + strlen(separator) * (distance - 1));

    i = begin;
    retVal.append(*i);
    while (++i != end) {
        retVal.append(separator);
        retVal.append(*i);
    }

    return retVal;
}

/*!
 * \brief Catenates \p strings, with default separator.
 * \ingroup group_tf_String
 *
 * Returns the catenation of the strings in \p strings, with \p separator
 * (by default, a space) added between each successive pair of strings.
 */
std::string TfStringJoin(const std::vector<std::string>& strings,
                         const char* separator = " ");

/*!
 * \brief Catenates \p strings, with default separator.
 * \ingroup group_tf_String
 *
 * Returns the catenation of the strings in \p strings, with \p separator
 * (by default, a space) added between each successive pair of strings.
 */
std::string TfStringJoin(const std::set<std::string>& strings,
                         const char* separator = " ");

/*!
 * \brief Breaks the given string apart, returning a vector of strings.
 * \ingroup group_tf_String
 *
 * The string \p source is broken apart into individual words, where a word
 * is delimited by the string \p separator. This function behaves like
 * pythons string split method.
 */

std::vector<std::string> TfStringSplit(std::string const &src,
                                       std::string const &separator);

/*!
 * \brief Breaks the given string apart, returning a vector of strings.
 * \ingroup group_tf_String
 *
 * The string \p source is broken apart into individual words, where a word
 * is delimited by the characters in \p delimiters.  Delimiters default
 * to white space (space, tab, and newline).
 */

std::vector<std::string> TfStringTokenize(const std::string& source,
                                          const char* delimiters = " \t\n");

/*!
 * \brief Breaks the given string apart, returning a set of strings.
 * \ingroup group_tf_String
 *
 * Same as TfStringTokenize, except this one
 * returns a set.
 */

std::set<std::string> TfStringTokenizeToSet(const std::string& source,
                                            const char* delimiters = " \t\n");

/*!
 * \brief Breaks the given quoted string apart, returning a vector of strings.
 * \ingroup group_tf_String
 *
 * The string \p source is broken apart into individual words, where a word
 * is delimited by the characters in \p delimiters.  This function is similar 
 * to \c TfStringTokenize, except it considers a quoted string as a single
 * word. The function will preserve quotes that are nested within other quotes
 * or are preceded by a backslash character. \p errors, if provided, contains 
 * any error messages. Delimiters default to white space (space, tab, and 
 * newline).
 */

std::vector<std::string> 
TfQuotedStringTokenize(const std::string& source, 
                       const char* delimiters = " \t\n", 
                       std::string *errors = NULL);

/*!
 * \brief Breaks the given string apart by matching delimiters.
 * \ingroup group_tf_String
 *
 * The string \p source is broken apart into individual words, where a word
 * begins with \p openDelimiter and ends with a matching \p closeDelimiter.
 * Any delimiters within the matching delimeters become part of the word, and
 * anything outside matching delimeters gets dropped.
 * For example, 
 * \c TfMatchedStringTokenize("{a} string {to {be} split}", '{', '}')
 * would return a vector containig "a" and "to {be} split".
 * If \p openDelimiter and \p closeDelimiter cannot be the same.
 * \p errors, if provided, contains any error messages.
 */

std::vector<std::string> 
TfMatchedStringTokenize(const std::string& source, 
                        char openDelimiter, 
                        char closeDelimiter, 
                        char escapeCharacter = '\0',
                        std::string *errors = NULL);

/*!
 * \overload
 *
 * This overloaded version of \c TfMatchedStringTokenize does
 * not take an \c escapeCharacter parameter but does take
 * \param errors.  It allows \c TfMatchedStringTokenize to be
 * called with or without an \c escapeCharacter and with or without
 * \c errors.
 */

inline
std::vector<std::string> 
TfMatchedStringTokenize(const std::string& source, 
                        char openDelimiter, 
                        char closeDelimiter, 
                        std::string *errors)
{
    return TfMatchedStringTokenize(source, openDelimiter,
                                   closeDelimiter, '\0', errors);
}

/*!
 * \class TfDictionaryLessThan StringUtils.h pxr/base/tf/stringUtils.h
 * \ingroup group_tf_String
 * \brief Provides dictionary ordering binary predicate function on strings.
 *
 * The \c TfDictionaryLessThan class is a functor as defined by the
 * STL standard.  It compares strings using "dictionary" order: for example,
 * the following strings are in dictionary order:
 * ["abacus", "Albert", "albert", "baby", "Bert", "file01", "file001", "file2",
 * "file10"]
 *
 * Note that capitalization matters only if the strings differ by capitalization
 * alone.
 *
 * Characters whose ASCII value are inbetween upper- and lowercase letters,
 * such as underscore, are sorted to come after all letters.
 *
 */
struct TfDictionaryLessThan {
    /*!
     * \brief Return true if \p lhs is less than \p rhs in dictionary order.
     *
     * Normally this functor is used to supply an ordering functor
     * for STL containers: for example,
     * \code
     *   map<string, DataType, TfDictionaryLessThan>  table;
     * \endcode
     *
     * If you simply need to compare two strings, you can do so as follows:
     * \code
     *     bool aIsFirst = TfDictionaryLessThan()(aString, bString);
     * \endcode
     */
    bool operator()(const std::string &lhs, const std::string &rhs) const;
};

/*!
 * \brief Convert an arbitrary type into a string
 * \ingroup group_tf_String
 *
 * Use the type's stream output operator to convert it into a string. You are
 * free to use the stream operators in ostreamMethods.h, but are not required
 * to do so.
 */
template <typename T>
typename boost::disable_if<boost::is_enum<T>, std::string>::type
TfStringify(const T& v)
{
    std::ostringstream stream;
    stream << v;
    return stream.str();
}

template <typename T>
typename boost::enable_if_c<boost::is_enum<T>::value, std::string>::type
TfStringify(const T& v)
{
    return TfEnum::GetName(v);
}

std::string TfStringify(bool v);
std::string TfStringify(std::string const&);
std::string TfStringify(float);
std::string TfStringify(double);

/*!
 * \brief Convert a string to an arbitrary type
 * \ingroup group_tf_String
 *
 * Use the type's stream input operator to get it from a string.
 * If \p status is non-NULL and \p instring cannot be converted
 * to a \c T, \p *status is set to \c false; otherwise, \p *status
 * is not modified.
 */
template <typename T>
T
TfUnstringify(const std::string &instring, bool* status = NULL)
{
    T v = T();
    std::istringstream stream(instring);
    stream >> v;
    if (status && !stream)
        *status = false;
    return v;
}
template <>
bool TfUnstringify(const std::string &instring, bool* status);

template <>
std::string TfUnstringify(const std::string &instring, bool* status);

/*! \brief Returns a string with glob characters converted to their regular
 * expression equivalents.
 *  \ingroup group_tf_String
 *
 *  Currently, this transforms strings by replacing all instances of '.' with
 *  '\.', '*' with '.*', and '?' with '.', in that order.
 */
std::string TfStringGlobToRegex(const std::string& s);

/*!
 * \brief Process escape sequences in ANSI C string constants.
 * \ingroup group_tf_String
 *
 * The following escape sequences are accepted:
 *
 * \li \\\\:    backslash
 * \li \\a:     ring the bell
 * \li \\b:     backspace
 * \li \\f:     form feed
 * \li \\n:     new line
 * \li \\r:     carriage return
 * \li \\t:     tab
 * \li \\v:     vertical tab
 * \li \\xdd:   hex constant
 * \li \\ddd:   octal constant
 *
 * So, if the two-character sequence "\\n" appears in the string, it is
 * replaced by an actual newline.  Each hex and octal constant translates
 * into one character in the output string.  Hex constants can be any 
 * length, while octal constants are limited to 3 characters.  Both
 * are terminated by a character that is not a valid constant.  Illegal 
 * escape sequences are replaced by the character following the backslash,
 * so the two character sequence "\\c" would become "c".  Processing
 * continues until the input hits a NUL character in the input string -
 * anything appearing after the NUL will be ignored.
 */
std::string TfEscapeString(const std::string &in);
void TfEscapeStringReplaceChar(const char** in, char** out);

/*! \brief Concatenate two strings containing '/' and '..' tokens like a file
 *  path or scope name.
 * \ingroup group_tf_String
 *
 * Tokenize the input strings using a '/' delimeter. Look for '..' tokens
 * in the suffix and construct the appropriate result.
 *
 * Examples:
 * 
 * \li TfStringCatPaths( "foo/bar", "jive" ) => "foo/bar/jive"
 * \li TfStringCatPaths( "foo/bar", "../jive" ) => "foo/jive"
 */

std::string TfStringCatPaths( const std::string &prefix, 
                              const std::string &suffix );

/*!
 * \brief Test whether \a identifier is valid.
 *
 * An identifier is valid if it follows the C/Python identifier convention;
 * that is, it must be at least one character long, must start with a letter or
 * underscore, and must contain only letters, underscores, and numerals.
 */
inline bool
TfIsValidIdentifier(const std::string &identifier)
{
    char const *p = identifier.c_str();
    if (not *p or (not (('a' <= *p and *p <= 'z') or
                        ('A' <= *p and *p <= 'Z') or 
                        *p == '_')))
        return false;

    for (++p; *p; ++p) {
        if (not (('a' <= *p and *p <= 'z') or
                 ('A' <= *p and *p <= 'Z') or
                 ('0' <= *p and *p <= '9') or
                 *p == '_')) {
            return false;
        }
    }
    return true;
}

/*!
 * \brief Produce a valid identifier (see TfIsValidIdentifier) from \p in by
 * replacing invalid characters with '_'.  If \p in is empty, return "_".
 */
std::string
TfMakeValidIdentifier(const std::string &in);

/*!
 * \brief Escapes characters in \a in so that they are valid XML.
 * \ingroup group_tf_String
 *
 * Returns the name with special characters (&, <, >, ", ') replaced with
 * the corresponding escape sequences.
 */
std::string TfGetXmlEscapedString(const std::string &in);

#endif /* TF_STRINGUTILS_H */