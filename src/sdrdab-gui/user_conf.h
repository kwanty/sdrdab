/*
 * user_conf.h
 *
 *  Created on: 16 maj 2016
 *
 */

#ifndef USER_CONF_H_
#define USER_CONF_H_
#include <argp.h>
/// @cond
#include <inttypes.h>
/// @endcond

class UserConf {
public:
    /**
     * Constructor; sets 0/NULL/false for every member, except:
     * <ul><li>rds_ (as it is to be true by default),</li>
     * <li>tuner_ (NO_TUNER indicates that it wasn't touched).</li></ul>
     */
    UserConf();
    /**
     * Destructor
     */
    virtual ~UserConf();

    /**
     * @brief Applies default values where needed.
     */
    void CompleteEmptyValues();

    /// Magic UserInput constants.
    enum {
        NO_TUNER_SELECTED = -1,     ///< special TUNER_NO for having no tuner selected
        ///< (this isn't the same as 0)
        ARGP_EXIT_ERR_CODE = 1000,  ///< value for argp_err_exit_status
        DEFAULT_FREQ_kHz = 229072,  ///< default frequency: 229072 kHz (DAB+ in Cracov)
        MIN_DAB_FREQ_kHz = 174000,  ///< minimum possible DAB frequency [kHz]
        MAX_DAB_FREQ_kHz = 230000,  ///< maximum possible DAB frequency [kHz]
        DEFAULT_FS_kHz = 2048,      ///< default sampling frequency [kHz]
    };

    /**
     * Checks, based on user input, whether tuner or file should be used
     * @return true for tuner, false for file
     */
    bool from_tuner_;
    uint32_t freq_;         ///< selected carrier frequency [Hz]
    uint32_t sampling_rate_;///< selected sampling rate [Hz]
    uint8_t channel_nr;     ///< initial channel number
    bool list_;             ///< list flag (list available tuners and exit)
    int tuner_;             ///< given tuner number
    ///< (-1 indicates that it wasn't touched)
    const char *file_;      ///< give input file name
    bool silent_;           ///< silence flag (don't play on speakers)
    bool rds_;              ///< RDS flag (whether RDS-like text is to be displayed)
    bool info_;             ///< channel info flag (dumps very specific channel info)
    bool verbose_;          ///< verbosity flag
    const char *graphic_;   ///< directory to save SlideShow graphics
    const char *output_;    ///< file to save audio
};

#endif /* SRC_SDRDAB_GUI_USER_CONF_H_ */
