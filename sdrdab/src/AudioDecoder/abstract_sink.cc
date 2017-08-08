/*
 * abstract_sink.cpp
 *
 *  Created on: May 13, 2015
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#include "abstract_sink.h"
#include <string.h>

AbstractSink::~AbstractSink() {
}

AbstractSink::AbstractSink() {
}

bool AbstractSink::operator ==(const AbstractSink &other) const {
	return !strcmp(name(), other.name());
}
