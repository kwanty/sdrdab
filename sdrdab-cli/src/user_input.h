/**
 * @file user_input.h
 * @brief Provides class for storage and parsing of supplied commandline arguments
 *
 * @author Krzysztof Szczęsny, kaszczesny@gmail.com
 * @date Created on: 1 May 2015
 * @version 1.0
 * @copyright Copyright (c) 2015 Krzysztof Szczęsny
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

#ifndef USERINPUT_H_
#define USERINPUT_H_

#include <argp.h>
/// @cond
#include <inttypes.h>
/// @endcond

/**
 * Stores results of parsed commandline arguments
 * @copydetails user_input.h
 */
class UserInput {
  public:

	/// Magic UserInput constants.
	enum {
		NO_TUNER_SELECTED = -1,     ///< special TUNER_NO for having no tuner selected
									///< (this isn't the same as 0)
		ARGP_EXIT_ERR_CODE = 1000,	///< value for argp_err_exit_status
		DEFAULT_FREQ_kHz = 229072,	///< default frequency: 229072 kHz (DAB+ in Cracov)
		MIN_DAB_FREQ_kHz = 174000,	///< minimum possible DAB frequency [kHz]
		MAX_DAB_FREQ_kHz = 230000,	///< maximum possible DAB frequency [kHz]
		DEFAULT_FS_kHz = 2048,		///< default sampling frequency [kHz]
	};

	/**
	 * Constructor; sets 0/NULL/false for every member, except:
	 * <ul><li>rds_ (as it is to be true by default),</li>
	 * <li>tuner_ (NO_TUNER indicates that it wasn't touched).</li></ul>
	 */
	UserInput();

	/**
	 * Destructor; nothing fancy here.
	 */
	virtual ~UserInput();

	/**
	 * Runs argp-related methods.
	 * @note May call std::exit on error
	 * @param[in] argc from main
	 * @param[in] argv from main
	 */
	void Process(int argc, char **argv);

	/**
	 * Argp parser callback.
	 * @param[in] key option that is currently processed
	 * @param[in] arg argument supplied with option
	 * @param[in,out] state argp parser state
	 * @return error code or 0
	 */
	static int Parser(int key, char *arg, argp_state *state);

	/**
	 * Checks, based on user input, whether tuner or file should be used
	 * @return true for tuner, false for file
	 */
	bool FromTuner(void);

	uint32_t freq_;			///< selected carrier frequency [Hz]
	uint32_t sampling_rate_;///< selected sampling rate [Hz]
	uint8_t channel_nr;		///< initial channel number
	bool list_;				///< list flag (list available tuners and exit)
	int tuner_;				///< given tuner number
							///< (-1 indicates that it wasn't touched)
	const char *file_;		///< give input file name
	bool silent_; 			///< silence flag (don't play on speakers)
	bool rds_;				///< RDS flag (whether RDS-like text is to be displayed)
	bool info_;				///< channel info flag (dumps very specific channel info)
	bool verbose_; 			///< verbosity flag
	const char *graphic_;	///< directory to save SlideShow graphics
	const char *output_;	///< file to save audio


  private:
	/**
	 * Sets argp options and runs argp.
	 * @param[in] argc from main
	 * @param[in] argv from main
	 * @return error code or 0
	 */
	int RunArgp(int argc, char **argv);

	/**
	 * Apply bugfix described in UserInput::bugfix_applied_
	 * (or rather mark bugfix as being applied).
	 */
	inline void ApplyBugfix(void) {
		this->bugfix_applied_ = true;
	}

	/**
	 * @brief Applies default values and prints verbose notices.
	 * Applies default values where needed.
	 * Prints to stderr input-related verbose notices.
	 * @param[in] state argp parser state
	 */
	void Postprocessing(argp_state *state);

	/**
	 * @brief True if bugfix was applied.
	 * getopt library, which is used by argp, has a bug (or maybe it's
	 * a feature?) - all options with optional argument require '=',
	 * which is counter-intuitive. My workaround is to set this to true
	 * and then check whether ARGP_KEY_ARG is present.
	 * Also a warning will be issued to stderr.
	 */
	bool bugfix_applied_;

};

#endif /* USERINPUT_H_ */
