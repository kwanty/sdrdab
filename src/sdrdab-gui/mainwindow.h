#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "scheduler.h"
#include "user_conf.h"
#include "user_data.h"
#include <QtCore/qtimer.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QtCore/qthread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public Scheduler{
	Q_OBJECT

public:
	/**
	 * Constructor
	 */
	explicit MainWindow(QWidget *parent = 0);
	/**
	 * Descructor
	 */
	virtual ~MainWindow();

	/**
	 * @brief Stores last error value set by
	 * CLIScheduler::ParametersFromSDR(scheduler_error_t).
	 * @note Thread-safety shouldn't be an issue as long as all errors are
	 * fatal, i.e. this isn't overwritten over and over.
	 */
	volatile static scheduler_error_t errno_;

	/**
	 * Calls Scheduler::Start() in separate thread via MainWindow::Start().
	 * @note Calls std::exit if thread creation fails.
	 */
	void FireSchedulerThread(void);

	private slots:

	/**
	 * Find available stations an print them
	 */

	void FillStationList();

	/**
	 * Prints text in MainWindow
	 */
	void printInConsole(std::string);

	/**
	 * After checking 'Mute'' QRadioButton, variable silent_ is set on true
	 */
	void on_muteCb_clicked(bool checked);

	/**
	 * After checking 'Signal from file' QRadioButton, user can choose file to play
	 */
	void on_signalCb_clicked(bool checked);

	/**
	 * User choose device to use
	 */
	void on_deviceList_currentIndexChanged(int index);

	/**
	 * Changing frequency
	 */
	void on_freqEdit_returnPressed();

	/**
	 * After checking 'Save to file' QRadioButton, output will be save to file with given name
	 */
	void on_chooseSignalPath_clicked();

	/**
	 * Stop the signal
	 */
	void on_stopBtn_clicked();

	/**
	 * Start playing
	 */
	void on_startBtn_clicked();

	/**
	 * Name of file to save output
	 */
	void on_saveFile_returnPressed();

	/**
	 * Changing station
	 */
	void on_stationList_currentRowChanged(int currentRow);

	/**
	 * Lists all detected stations on current multiplex.
	 * @note Should be used when user_data_mutex_ is locked.
	 */
	void RunFindingStations(void);

	private:

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
	 * Give a list of devices and put it into QComboButton deviceList.
	 */
	void FindDevices(void);

	/**
	 * Change current station
	 * @param[in] station number
	 */
	void ChangeStation(int);

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
	 * GUIScheduler::errno_
	 * @note Intended only for critical errors
	 */
	void PrintErrorMsg(void);

	Ui::MainWindow *ui;
	QByteArray ba_,ba1_; //variable used when converting QString into const char *
	QTimer *timer;
	UserConf *user_conf_;//informations about configuration
	pthread_t scheduler_thread_; ///< Scheduler::Start() thread
	/// tells if Scheduler thread hasn't stopped operation yet
	bool scheduler_thread_running_;
	UserData user_data_; ///< current information from SDR; protected by mutex
	/// mutex for accessing CLIScheduler::user_data_
	pthread_mutex_t user_data_mutex_;

};

#endif // MAINWINDOW_H
