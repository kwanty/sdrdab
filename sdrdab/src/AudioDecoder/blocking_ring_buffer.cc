/*
 * blocking_ring_buffer.cpp
 *
 *  Created on: May 24, 2015
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#include "blocking_ring_buffer.h"

BlockingRingBuffer::BlockingRingBuffer(size_t threshold, size_t length):
threshold_(threshold),
force_read_(false),
last_frame_(false),
buffer_(length) {
	pthread_mutex_init(&count_mutex_, NULL);
	pthread_cond_init(&count_condition_not_empty_, NULL);
}

BlockingRingBuffer::~BlockingRingBuffer() {
	pthread_cond_destroy(&count_condition_not_empty_);
	pthread_mutex_destroy(&count_mutex_);
}

size_t BlockingRingBuffer::ReadFrom(uint8_t *dest_buffer, size_t how_many) {
	pthread_mutex_lock(&count_mutex_);

	while(DataStored() < threshold_ && !last_frame_ && !force_read_)
		pthread_cond_wait(&count_condition_not_empty_, &count_mutex_);

	size_t result = buffer_.sReadFrom(dest_buffer, how_many);

	pthread_mutex_unlock(&count_mutex_);

	return result;
}

size_t BlockingRingBuffer::WriteInto(uint8_t *source_buffer, size_t how_many) {
	pthread_mutex_lock(&count_mutex_);

	size_t result = buffer_.WriteInto(source_buffer, how_many);

	pthread_cond_signal(&count_condition_not_empty_);
	pthread_mutex_unlock(&count_mutex_);

	return result;
}

size_t BlockingRingBuffer::FreeSpace() const {
	return buffer_.FreeSpace();
}

size_t BlockingRingBuffer::DataStored() const {
	return buffer_.DataStored();
}

void BlockingRingBuffer::set_last_frame(bool to) {
	last_frame_ = to;
}

bool BlockingRingBuffer::last_frame() const {
	return last_frame_;
}

void BlockingRingBuffer::Reset() {
	buffer_.Reset();
	set_force_read(false);
	set_last_frame(false);
}

bool BlockingRingBuffer::force_read() const {
	return force_read_;
}

void BlockingRingBuffer::set_force_read(bool to) {
	force_read_ = to;
}

void BlockingRingBuffer::ForceSignal() {
	pthread_mutex_lock(&count_mutex_);
	pthread_cond_signal(&count_condition_not_empty_);
	pthread_mutex_unlock(&count_mutex_);
}
