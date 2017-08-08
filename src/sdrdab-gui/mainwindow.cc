#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <pthread.h>
#include <unistd.h> //isatty
#include <cstdlib>
#include <iostream>


using namespace std;

volatile MainWindow::scheduler_error_t MainWindow::errno_ = OK;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),
		Scheduler(),
		ui(new Ui::MainWindow),
		user_conf_(new UserConf()),
		scheduler_thread_(0),
		scheduler_thread_running_(true),
		user_data_(),
		user_data_mutex_(),
		timer(new QTimer(this)){
	pthread_mutex_init(&this->user_data_mutex_, NULL);
	ui->setupUi(this);
	ui->optionWidget->setVisible(false);
	ui->consoleWidget->setVisible(false);
	FindDevices();
	QTimer *timer = new QTimer(this);
}

MainWindow::~MainWindow(){
	delete this->user_conf_;
	pthread_mutex_destroy(&this->user_data_mutex_);
	delete ui;
}

//MARK: Slots

void MainWindow::on_muteCb_clicked(bool checked){
	user_conf_->silent_=true;
}

void MainWindow::on_signalCb_clicked(bool checked){
	if(checked){
		user_conf_->from_tuner_=false;
		ui->deviceList->setDisabled(true);
	}
	else{
		user_conf_->from_tuner_=true;
		ui->deviceList->setDisabled(false);
	}
}

void MainWindow::on_deviceList_currentIndexChanged(int index){
	if(index>0){
		user_conf_->tuner_= index;
		ui->console->append("Tuner selected: "+ QString::number(index));
	}
}

void MainWindow::on_freqEdit_returnPressed(){
	if(ui->freqEdit->text()!= NULL ){
		user_conf_->freq_= static_cast<u_int32_t>(ui->freqEdit->text().toInt());
		ui->console->append("Frequency changed");
	}
}

void MainWindow::on_chooseSignalPath_clicked(){
	QString filename = QFileDialog::getOpenFileName(
			this,
			tr("Choose file to play"),
			("/home/"),
			"Raw samples (*.raw)"
	);
	ui->signalPath->setText(filename);
	ba_ = filename.toUtf8();
	const char *path = ba_.data();
	user_conf_->file_ = path;
	ui->console->append("Chosen file:");
	ui->console->append(user_conf_->file_);
}

void MainWindow::on_stopBtn_clicked(){
}

void MainWindow::on_startBtn_clicked(){
	user_conf_->CompleteEmptyValues();
	if(user_conf_->tuner_ != -1 || user_conf_->file_ != NULL){
		QFuture<void> future1 =  QtConcurrent::run(this,&MainWindow::FireSchedulerThread);
		QFuture<void> future2 =  QtConcurrent::run(this,&MainWindow::RunFindingStations);
		ui->startBtn->setEnabled(false);
		ui->stopBtn->setEnabled(true);
	}
	else{
		ui->console->append("Choose file or tuner");
	}
}

void MainWindow::FillStationList(){
	pthread_mutex_lock(&this->user_data_mutex_);
	std::list<stationInfo> *stations = &this->user_data_.stations;
	ui->stationList->clear();


	if(!stations->empty()){
	for (std::list<stationInfo>::iterator it = stations->begin();
			it != stations->end();
			++it){
		if(it->station_name == "Not Available" || it->station_name == "Packet Mode"){
			break;
		}
		else{
			ui->stationList->addItem(it->station_name.c_str());
		}
		timer->stop();
		pthread_mutex_unlock(&this->user_data_mutex_);
	}
	}
}

void MainWindow::RunFindingStations(void){
	connect(timer, SIGNAL(timeout()), this, SLOT(FillStationList()));
	timer->start(3000);
}

void MainWindow::on_saveFile_returnPressed(){
	QString filepath = ui->saveFile->text();
	ba1_ = filepath.toUtf8();
	const char *file = ba1_.data();
	user_conf_->output_ = file;
	ui->console->append("Output will be save to file:");
	ui->console->append(user_conf_->output_);
}

void MainWindow::on_stationList_currentRowChanged(int currentRow){
	if(timer->isActive())
		timer->stop();
	this->ChangeStation(currentRow+1);
}

void *MainWindow::Start(void *this_ptr){
	MainWindow *this_ = static_cast<MainWindow *>(this_ptr);
	SchedulerConfig_t config; //invokes default SchedulerConfig_t constructor
	config.sampling_rate  = this_->user_conf_->sampling_rate_;
	config.carrier_frequency = this_->user_conf_->freq_;
	if (this_->user_conf_->from_tuner_)
		config.dongle_nr = static_cast<size_t>(this_->user_conf_->tuner_);
	else {
		config.input_filename = this_->user_conf_->file_;
		config.data_source = Scheduler::DATA_FROM_FILE;
	}
	config.use_speakers = !(this_->user_conf_->silent_);
	config.output_filename = this_->user_conf_->output_;
	if (this_->user_conf_->channel_nr > 0)
		config.start_station_nr = this_->user_conf_->channel_nr;
	this_->Scheduler::Start(config);
	this_->scheduler_thread_running_ = false;
	return NULL;
}

void MainWindow::FireSchedulerThread(){
	if( 0!= pthread_create(&this->scheduler_thread_,
			NULL,
			MainWindow::Start,
			static_cast<void *>(this))){
		printInConsole("FATAL ERROR: Thread creation failed.");
		delete this;
	}
}

void MainWindow::printInConsole(std::string text){
	QString qtext = QString::fromStdString(text);
	ui->console->append(qtext);
}

void MainWindow::FindDevices(void) {
	std::list<std::string> devices;
	this->ListDevices( &devices );
	if (!devices.empty()){
		std::list<std::string>::iterator it;
		size_t i = 1;
		for (it = devices.begin(); it != devices.end(); ++it){
			ui->deviceList->addItem(it->c_str());
		}
	}
}

void MainWindow::ChangeStation(int station){

	pthread_mutex_lock(&this->user_data_mutex_);
	this->ParametersToSDR(STATION_NUMBER, station);
	pthread_mutex_unlock(&this->user_data_mutex_);
}

void MainWindow::ParametersFromSDR(scheduler_error_t error_code){

	switch (error_code) {
	case OK:
		MainWindow::errno_ = OK;
		break;

	case DAB_NOT_DETECTED:
		printInConsole("DAB signal was not detected!\n");
		break;

	case STATION_NOT_FOUND:
		printInConsole("  Unknown station.\n  Type 'list' to get list of "
				"stations available on current multiplex.\n");
		break;

	default:
		MainWindow::errno_ = error_code;
		this->PrintErrorMsg();
	}

}

void MainWindow::ParametersFromSDR(UserFICData_t *user_fic_extra_data) {
	pthread_mutex_lock(&this->user_data_mutex_);
	this->user_data_.Set(user_fic_extra_data);
	pthread_mutex_unlock(&this->user_data_mutex_);
	delete user_fic_extra_data;
}

void MainWindow::ParametersFromSDR(std::string *text) {
	printInConsole("ParametersFromSDR: " + *text + "\n");
}


void MainWindow::PrintErrorMsg(void) {
	cout << "PrintErrorMSG ****************************" << endl;
	switch (MainWindow::errno_) {
	case OK:
		printInConsole("PrintErrorMsg: OK.\n");
		break;
	case DAB_NOT_DETECTED:
		printInConsole("PrintErrorMsg: DAB NOT DETECTED.\n");
		break;
	case STATION_NOT_FOUND:
		printInConsole("PrintErrorMsg: STATION NOT FOUND.\n");
		break;

	case FILE_NOT_FOUND:
		printInConsole("Specified input file wasn't found or couldn't be opened.\n");
		break;

	case DEVICE_NOT_FOUND:
		printInConsole("PrintErrorMsg: device NOT FOUND.\n");
		break;

	case DEVICE_DISCONNECTED:
		printInConsole("RTL tuner has been disconnected from USB.\n");
		break;

	case FILE_END:
		printInConsole("In the supplied sample source file EOF has been reached.\n");
		break;

	default:
		MainWindow::errno_ = ERROR_UNKNOWN;
		printInConsole("Unknown error has occurred.\n");
	}
}

void MainWindow::ParametersFromSDR(float snr){

	if (snr != snr) //NaN
		printInConsole("SNR measurement has failed.\n");
	else{
		std::ostringstream ss;
		ss << snr;
		std::string string = ss.str();
		QString qString = QString::fromStdString(string);
		ui->snrEdit->setText(qString);
		printInConsole(string);
	}
}


