/**
 * @file read_line.h
 * @brief GNU libreadline interaction
 *
 * @author Krzysztof Szczęsny, kaszczesny@gmail.com
 * @date Created on: 4 June 2015
 * @version 1.0
 * @copyright Copyright (c) 2015 Krzysztof Szczęsny
 * @pre libreadline6
 * @par License
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see \<http://www.gnu.org/licenses/\>.
 */
#ifndef READ_LINE_H_
#define READ_LINE_H_

#include <cstdio>
#include <readline/readline.h>
#include <readline/history.h>

/**
 * Enables easy libreadline interaction by providing some namespaced functions.
 * Anonymous namespace serves as "private" specificatior.
 * Also provides general I/O enhancements ("overloaded" printf function).
 * @note Large parts of code and general idea are based on
 * http://web.mit.edu/gnu/doc/html/rlman_2.html
 * @copydetails read_line.h
 */
namespace ReadLine {

    /// Return format for ReadLine::ParseLine.
    struct parsed_command_t {
        int command_id_;   ///< id of the command
        const char *arg_; ///< argument passed to the command
    };

    /// Structure for storing information about commands.
    struct command_t {
        const char *name_; ///< human-readable function name
        int id_;           ///< id of the function for better lookup than name comparison
        const char *doc_;  ///< function 'documentation'
    };

    /// ReadLine magic constants.
    enum {
        COMMAND_NOT_FOUND,      ///< ParseLine was unable to find the command
        HISTORY_ENTRY_LEN = 64, ///< don't add to history lines exceeding this limit
    };

    extern const char * const prompt_; ///< displayed prompt
    extern const command_t command_vector[]; ///< pointer to vector of commands

    /**
     * Initializes libreadline environment.
     * @note Every cstdio/iostream output to terminal between calling Init and
     * DeInit will distort prompt - ReadLine::printf should be used instead.
     */
    void Init(void);

    /// Releases handlers
    void DeInit(void);

    /**
     * @brief Prints given string so that prompt remains untouched.
     * This is a wrapper function for printf and is meant to replace it for as long
     * as libreadline is being used.
     * @param[in] msg format string for printf
     * @param[in] ... additional parameters for printf
     * @return false on error
     * @note based on http://stackoverflow.com/a/3001459
     * and http://stackoverflow.com/a/5070889
     */
    bool printf(const char *msg, ...)
        /// @cond
        __attribute__((format(printf, 1, 2)))
        /// @endcond
        ;

    /**
     * Tries to find command in given line.
     * Also saves line in the history.
     * @param[in,out] line line to parse (it might be altered)
     * @return parsed_command_t structure, command_id_ == COMMAND_NOT_FOUND if
     * not found.
     */
    parsed_command_t ParseLine(char *line);

    /**
     * "Private" namespace
     */
    namespace {

        /**
         * Disables filename suggestions.
         * @param[in] s irrelevant
         * @param[in] i irrelevant
         * @return always NULL
         */
        inline char *Dummy(const char *s, int i) {
            return NULL;
        }

        /**
         * Attempt to complete contents of \c text.
         * @param[in] text command stub
         * @param[in] start beginning of the word to complete in text
         * @param[in] end end of the word to complete in text
         * @return array of matches or NULL if there aren't any
         */
        char **CommandCompletition(const char *text, int start, int end);

        /**
         * Generator function for command completion.
         * @param[in] text command stub
         * @param[in] state if 0, starts from top of the list
         * @return generated stuff
         */
        char *CommandGenerator(const char *text, int state);

        /**
         * Creates duplicate of the given string on the heap (via malloc).
         * @param[in] s string to duplicate
         * @return duplicated string
         */
        char *DuplicateString(const char *s);

        /**
         * Finds first non-whitespace character in given string.
         * @param[in] s string to scan
         * @return pointer to first non-whitespace char inside s
         */
        char *FirstNonWhitespace(char *s);

        /**
         * Looks up command by its name
         * @param[in] name command to lookup
         * @return command id or COMMAND_NOT_FOUND
         */
        int FindCommand(const char *name);

    }; //end anonymous namespace

}; //end namespace ReadLine

#endif /* READ_LINE_H_ */
