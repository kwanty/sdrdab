/*
 * resampler.cpp
 *
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#include "resampler.h"
#include <stdio.h>

#define CHUNK_LEN 256

long ResamplerCallbacks::WriteCall(void *cb_data, float **data) {
	Resampler::Data *pcb_data;

	if((pcb_data = reinterpret_cast<Resampler::Data *>(cb_data)) == NULL)
		return 0;

	if(data == NULL)
		return 0;

	*(data) = pcb_data->in_data_ptr + (pcb_data->current_count*pcb_data->channels);	//move pointer properly, taking into account read data and number of channels

	long frames;

	if(pcb_data->total - pcb_data->current_count > CHUNK_LEN)	//reading in CHUNK_LEN parts, checking if there is enough data left
		frames = CHUNK_LEN;
	else
		frames = pcb_data->total - pcb_data->current_count;

	pcb_data->current_count += frames;

	return frames;
}

Resampler::Resampler(int conv_type, int chann):
converter_type_(conv_type) {
	data_.current_count = 0;
	data_.total = 0;
	data_.in_data_ptr = NULL;
	data_.channels = chann;

	int error;
	if((src_state_ = src_callback_new(ResamplerCallbacks::WriteCall, conv_type, chann, &error, &data_)) == NULL) {
		printf("Resampler error: src_callback_new() failed: %s", src_strerror(error));
	}
}

Resampler::~Resampler() {
	src_delete(src_state_);
}

void Resampler::SetSourceBuffer(float *source_buffer, size_t length) {
	src_reset(src_state_);

	data_.current_count = 0;
	data_.total = length/data_.channels;
	data_.in_data_ptr = source_buffer;
}

long Resampler::Resample(float *destination_buffer, size_t length, float ratio) {
	return src_callback_read(src_state_, ratio, length/data_.channels, destination_buffer)*data_.channels;
}
