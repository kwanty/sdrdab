/**
 * @file sdrdab-cli.cc
 * @brief main() function
 *
 * @author Krzysztof Szczęsny, kaszczesny@gmail.com
 * @date Created on: 9 April 2015
 * @version 1.0
 * @copyright Copyright (c) 2015 Krzysztof Szczęsny
 * @pre sdrdab + librtlsdr (dynamic linkage)
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

#include "cli_scheduler.h"

/**
 * Main function - initialization + recreation of CLIScheduler object in a loop
 * if requested.
 * @param[in] argc argc
 * @param[in] argv argv
 * @return EXIT_SUCCESS, CLIScheduler::PTHREAD_CREATE_FAIL,
 * Scheduler::scheduler_error_t or UserInput::ARGP_EXIT_ERR_CODE.
 */
int main(int argc, char **argv) {
	CLIScheduler *cli = new CLIScheduler(argc, argv);
	cli->SuppressStderr();
	cli->RequestedList();

	ReadLine::Init();

	cli->FireSchedulerThread();
	while (cli->Process() == true) { //event loop until user quits
		cli = cli->Revive();
		cli->FireSchedulerThread();
	}

	delete cli;
	return CLIScheduler::errno_;
}
