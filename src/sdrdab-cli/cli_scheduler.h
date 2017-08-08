/**
 * @class CLIScheduler
 * @brief Class derived from sdrdab's Scheduler.
 *
 * @author Krzysztof Szczęsny, kaszczesny@gmail.com
 * @date Created on: 3 June 2015
 * @version 1.0
 * @copyright Copyright (c) 2015 Krzysztof Szczęsny
 * @pre sdrdab + rtlsdr (dynamic linkage)
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

#ifndef CLI_SCHEDULER_H_
#define CLI_SCHEDULER_H_

#include "scheduler.h"
#include "user_input.h"
#include "user_data.h"
#include "read_line.h"

/**
 * @brief User-library interaction.
 * Class demonstrating how to use sdrdab and how to set user-defined behavior
 * when library reports occurrence of interesting events.
 * Also features second event loop for interaction with user.
 * Key points of this class are:
 * <ul><li> Scheduler::Start() call - does the
 * processing,</li>
 * <li>implementation of ParametersFromSDR(UserFICData_t*) - tells Scheduler's
 * thread what to do when new information from Fast Info Channel arrives,</li>
 * <li>implementation of ParametersFromSDR(scheduler_error_t) - provides an
 * interface for Scheduler to indicate errors,</li>
 * <li>ParametersToSDR(sdr_parameter_t, uint8_t) call - tells Scheduler to change station or other sdr parameter,</li>
 * <li>Scheduler::Stop() call - signals to Scheduler
 * thread that it should stop processing.</li></ul>
 * @copydetails cli_scheduler.h
 */
class CLIScheduler: public Scheduler {
    public:

        /// Possible commands given by user.
        enum user_command_t {
            COMMAND_UNKNOWN = ReadLine::COMMAND_NOT_FOUND, ///< unknown command
            COMMAND_STATION, ///< change current station
            COMMAND_LIST,    ///< list stations on current multiplex
            COMMAND_QUIT,    ///< quit
            COMMAND_MUTE,    ///< mute audio
            COMMAND_VOLUME,  ///< change current volume
            COMMAND_IMAGE,   ///< query for current image
            COMMAND_HELP,    ///< display help message
            COMMAND_RESTART, ///< restart
            COMMAND_CHALG,   ///< change convolutional decoder alg
        };

        /**
         * Constructor; runs argp parsing functions of user_input_.
         * @param[in] argc argc from main()
         * @param[in] argv argv from main()
         */
        CLIScheduler(int argc, char **argv);

        /// Destructor; nothing fancy here.
        virtual ~CLIScheduler();

        /**
         * Redirects stderr to /dev/null if verbose option wasn't specified and
         * stderr points to a terminal (wasn't redirected).
         * Without calling this method, stderr output from library might visually
         * break the prompt (as library isn't aware that it should call
         * ReadLine::printf() )
         * @return false on error
         */
        bool SuppressStderr(void);

        /**
         * Prints device listing if it was requested by user via commandline
         * <tt>\--list-devices</tt> argument.
         * @note Calls std::exit(EXIT_SUCCESS), should listing be requested.
         */
        void RequestedList(void);

        /**
         * Calls Scheduler::Start() in separate thread via CLIScheduler::Start().
         * @note Calls std::exit if thread creation fails.
         */
        void FireSchedulerThread(void);

        /**
         * @brief Event loop of the CLI.
         * Reacts to user input, requesting changes in configuration.
         * @note Blocking function readline() is used, so if Scheduler finishes its
         * work, the loop will get stuck until user presses Enter (although a
         * message prompting to press Enter will be displayed via
         * CLIScheduler::ParametersFromSDR(scheduler_type_t) called by Scheduler's
         * thread).
         * @return true if last command was 'restart'
         */
        bool Process(void);

        /**
         * "Revives" object by deleting the current one and returning a new one.
         * @return Pointer to new CLIScheduler instance.
         */
        CLIScheduler *Revive(void);

        /**
         * @brief Stores last error value set by
         * CLIScheduler::ParametersFromSDR(scheduler_error_t).
         * @note Thread-safety shouldn't be an issue as long as all errors are
         * fatal, i.e. this isn't overwritten over and over.
         */
        volatile static scheduler_error_t errno_;

    private:

        /// CLIScheduer magic values
        enum {
            /// errror code returned by main in case pthread_create fails
            PTHREAD_CREATE_FAIL = 5000,
        };

        /**
         * Wrapper static method for starting Scheduler::Start in separate thread.
         * @param[in,out] this_ptr pointer to this, casted to void *
         * @return NULL
         */
        static void *Start(void *this_ptr);

        /**
         * Called when user uses \c station command.
         * Queries for current station or changes it.
         * @note Should be used when user_data_mutex_ is locked.
         * @param[in] arg name or number of new station
         */
        void CommandStation(const char *arg);

        /**
         * Called when user uses \c list command.
         * Lists all detected stations on current multiplex.
         * @note Should be used when user_data_mutex_ is locked.
         */
        void CommandList(void);

        /**
         * "Callback" executed whenever something interesting happens.
         * Error code variant. Causes CLIScheduler's loop to end in reaction to
         * failure reported by library (or just prints a notice if it isn't serious
         * error).
         * @param[in] error_code error code
         */
        virtual void ParametersFromSDR(scheduler_error_t error_code);

        /**
         * "Callback" executed whenever something interesting happens.
         * SNR measurement variant.
         * @note SNR measurement isn't currently implemented in the library.
         * @param[in] snr current SNR level [dB]
         */
        virtual void ParametersFromSDR(float snr);

        /**
         * "Callback" executed whenever something interesting happens.
         * FIG & SlideShow variant.
         * @param[in] user_fic_extra_data pointer to the structure
         * @note user_fic_extra_data has to be freed before return!
         * @todo Save image to file (isn't currently implemented in the library).
         */
        virtual void ParametersFromSDR(UserFICData_t *user_fic_extra_data);

        /**
         * "Callback" executed whenever something interesting happens.
         * RDS variant. It should display the text.
         * @param[in] text RDS text
         * @note Text has to be freed before return!
         * @note Isn't currently implemented in the library.
         */
        virtual void ParametersFromSDR(std::string *text);

        /**
         * Prints to stdout error message associated with current value of
         * CLIScheduler::errno_
         * @note Intended only for critical errors
         */
        void PrintErrorMsg(void);

        UserInput *user_input_; ///< user input parser
        pthread_t scheduler_thread_; ///< Scheduler::Start() thread
        /// tells if Scheduler thread hasn't stopped operation yet
        bool scheduler_thread_running_;
        UserData user_data_; ///< current information from SDR; protected by mutex
        /// mutex for accessing CLIScheduler::user_data_
        pthread_mutex_t user_data_mutex_;
};

#endif /* CLI_SCHEDULER_H_ */
