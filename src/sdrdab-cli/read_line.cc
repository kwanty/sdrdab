/**
 * @file read_line.cc
 * @brief ReadLine namespace implementation
 * @note based on http://web.mit.edu/gnu/doc/html/rlman_2.html
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

#include "read_line.h"
/// @cond
#include <cstdlib>
/// @endcond
#include <cstdarg>

#ifdef __MACH__
#define whitespace(c) (((c) == ' ') || ((c) == '\t'))
#endif

namespace ReadLine {
    const char * const prompt_ = "DAB> ";

    void Init(void) {
        using_history();
        add_history("");

        rl_set_keymap(rl_make_bare_keymap());
        rl_completion_entry_function = ReadLine::Dummy;
        rl_attempted_completion_function = ReadLine::CommandCompletition;

        std::atexit(ReadLine::DeInit);
    }

    void DeInit(void) {
        rl_callback_handler_remove();
    }

    bool printf(const char *msg, ...) {
        bool need_solution = (rl_readline_state & RL_STATE_READCMD) > 0;
        bool return_value;

        char* saved_line = rl_copy_text(0, rl_end);
        int saved_point = rl_point;

        if (need_solution) {
            rl_set_prompt("");
            rl_replace_line("", 0);
            rl_redisplay();
        }

        va_list args;
        va_start(args, msg);
        return_value = vprintf(msg, args) >= 0;
        va_end(args);

        if (need_solution) {
            rl_set_prompt(ReadLine::prompt_);
            rl_replace_line(saved_line, 0);
            rl_point = saved_point;
            rl_redisplay();
        }

        free(saved_line);
        return return_value;
    }

    parsed_command_t ParseLine(char *line) {
        parsed_command_t return_value = { COMMAND_NOT_FOUND, NULL };

        if (line == NULL)
            return return_value;

        const char *begin = ReadLine::FirstNonWhitespace(line);

        if (begin != '\0' &&
                ( //history empty or unique line (in regard to previous)
                  current_history() == NULL ||
                  strncmp(current_history()->line, begin, HISTORY_ENTRY_LEN) != 0
                ))
        {
            add_history(begin);
        }

        int command_id;
        const char *word;

        //isolate the command word
        size_t i = 0;
        while (line[i] != '\0' && whitespace(line[i]))
            ++i;
        word = line + i;

        while (line[i] != '\0' && !whitespace(line[i]))
            ++i;

        if (line[i] != '\0')
            line[i++] = '\0';

        command_id = ReadLine::FindCommand(word);

        if (command_id == COMMAND_NOT_FOUND) {
            return return_value;
        }

        //get command's argument, if any
        while (whitespace(line[i]))
            ++i;

        return_value.arg_ = line + i;
        return_value.command_id_ = command_id;

        return return_value;
    }

    // "Private" namespace
    /// @cond
    namespace {

        int FindCommand(const char *name) {
            //exact search
            for (size_t i = 0; command_vector[i].name_ != NULL; ++i)
                if (strcmp(name, command_vector[i].name_) == 0)
                    return command_vector[i].id_;

            //coarse search
            size_t len = strlen(name);
            int found_id = COMMAND_NOT_FOUND;
            for (size_t i = 0; command_vector[i].name_ != NULL; ++i) {
                if (strncmp(name, command_vector[i].name_, len) == 0) {
                    if (found_id != COMMAND_NOT_FOUND)
                        return COMMAND_NOT_FOUND;
                    found_id = command_vector[i].id_;
                }
            }

            return found_id;
        }

        char **CommandCompletition(const char *text, int start, int end) {
            /* If this word is at the start of the line, then it is a command to
             * complete.  Otherwise it is a parameter (which we do not complete). */
            if (start == 0)
                return rl_completion_matches(text, ReadLine::CommandGenerator);
            else
                return NULL;
        }

        char *CommandGenerator(const char *text, int state) {
            static int list_index, len;

            /* If this is a new word to complete, initialize now.  This includes
             * saving the length of TEXT for efficiency, and initializing the index
             * variable to 0. */
            if (state == 0) {
                list_index = 0;
                len = strlen(text);
            }

            const char *name;

            //Return the next name which partially matches from the command list.
            while (command_vector[list_index].name_ != NULL) {
                name = command_vector[list_index++].name_;

                if (strncmp(name, text, len) == 0)
                    return ReadLine::DuplicateString(name);
            }

            return NULL;
        }

        char *DuplicateString(const char *s) {
            char *r;

            r = static_cast<char *>( malloc((strlen(s) + 1)*sizeof(char)) );
            strcpy(r, s);
            return r;
        }

        char *FirstNonWhitespace(char *s) {
            if (s == NULL)
                return NULL;

            if (s[0] == '\0')
                return s;

            char *new_first = s;

            while(whitespace(new_first[0])) {
                ++new_first;

                if (new_first[0] == '\0') {
                    return new_first;
                }
            }

            return new_first;
        }

    }; //end anonymous namespace
    /// @endcond

}; //end namespace ReadLine
