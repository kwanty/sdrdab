/*
 * @class AbstractDataFeeder
 * @brief provides data to the system
 *
 * Abstract class to provide data.
 * Inherited by:
 * - DataFeeder (read data from RTL stick)
 * - FileDataFeeder (read data from file)
 *
 * @author Paweł Szulc <pawel_szulc@onet.pl>    - StartProcessing(), ReadAsync(), SetFC(), SetFS(), HandleDrifts()
 * @author Wojciech Szmyd <wojszmyd@gmail.com>  - Remodulate(), DCRemoval()
 * @author Kacper Patro patro.kacper@gmail.com
 * @date 7 July 2015
 * @version 1.0 beta
 * @copyright Copyright (c) 2015 Wojciech Szmyd, Kacper Patro
 * @par License
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "abstract_data_feeder.h"


void *
StartProcessing (void *data_needed) {
	data_feeder_ctx_t *ptctx = static_cast<data_feeder_ctx_t*> (data_needed);
	AbstractDataFeeder * obj =
			reinterpret_cast<AbstractDataFeeder*> (ptctx->object);
	obj->running = 1;

	obj->ReadAsync (data_needed);

	if (obj->debug)
		fprintf (stderr, "!Stopping thread! \n");
	// Leave mutex unlocked
	pthread_mutex_trylock (&(*ptctx->lock_buffer));
	pthread_mutex_unlock (&(*ptctx->lock_buffer));
	obj->running = 0;
	pthread_exit (NULL);
	return NULL;
}


AbstractDataFeeder::AbstractDataFeeder () {
	running = 0;
	verbose = true;
	debug = false;
	current_fs_offset = 0.0;
	current_fc_offset = 0.0;

	do_remodulate = true;
	do_dcremoval = true;
	do_handle_fs = true;
	do_agc = true;

	write_previous = NULL;


	real_dc_rb = new RingBuffer<float>(DC_LENGTH);
	real_dc_rb->Initialize(0.0);
	imag_dc_rb = new RingBuffer<float>(DC_LENGTH);
	imag_dc_rb->Initialize(0.0);
	inner_buf_num = 0;
	inner_buff_size = 0;

}

AbstractDataFeeder::~AbstractDataFeeder () {
  delete real_dc_rb;
  delete imag_dc_rb;
}

void
AbstractDataFeeder::Remodulate (float* data, size_t size,
		float frequencyShift) {
	if(!do_remodulate)
		return;

	uint32_t fs = this->GetFS ();
	if (frequencyShift == 0.0)
	{
		return;
	}
	float shift_factor = 2.0 * pi * frequencyShift / fs;
	for (unsigned int i = 0, j=1; i < size && j < size; i+=2,j+=2)
	{
		float re = *(data +i);
		float cos_shift_factor = cosf(shift_factor * i / 2);
		float sin_shift_factor = sinf(shift_factor * i / 2);
		*(data +i) = *(data +i) * cos_shift_factor - *(data +j) * sin_shift_factor;
		*(data +j) = re * sin_shift_factor + *(data +j) * cos_shift_factor;
	}
}
