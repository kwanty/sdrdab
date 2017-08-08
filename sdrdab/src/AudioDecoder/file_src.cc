/*
 * file_src.cpp
 *
 *  Created on: May 13, 2015
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#include "file_src.h"

FileSrc::FileSrc(const char *path):
data_(new Data),
path_(path) {
	data_->abstract_src = this;
	data_->player_data = NULL;
}

FileSrc::~FileSrc() {
	delete data_;
}

void FileSrc::SetSrc(void *player_data) {
	data_->player_data = reinterpret_cast<Player::Data *>(player_data);

	data_->src = gst_element_factory_make("filesrc", "src");
	g_assert(data_->src);

	g_object_set(data_->src, "location", path_, NULL);

	gst_bin_add_many(GST_BIN(data_->player_data->pipeline),
			data_->src,
			NULL
	);
}

const char *FileSrc::name() const {
	return "file_src";
}

void FileSrc::LinkSrc() {
	g_assert(gst_element_link_many(
			data_->src,
			data_->player_data->iddemux,
			NULL)
	);
}

void FileSrc::ResetSrc() {
	return;
}
