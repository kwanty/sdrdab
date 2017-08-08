/*
 * file_src.h
 *
 *  Created on: May 13, 2015
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#ifndef SRC_FILE_SRC_H_
#define SRC_FILE_SRC_H_

#include "abstract_src.h"
#include "player.h"

/**
 * @class FileSrc
 * @brief Class used to read from audio files
 *
 * @author Kacper Patro patro.kacper@gmail.com
 * @copyright Public domain
 * @pre
 */
class FileSrc: public AbstractSrc {
public:
	/**
	 * Constructor of FileSrc
	 * @param[in] path Path to input file
	 */
	FileSrc(const char *path);
	virtual ~FileSrc();

	void SetSrc(void *player_data);
	void LinkSrc();
	const char *name() const;
	void ResetSrc();

private:
	/**
	 * @struct Data
	 * @brief This struct contains specific for FileSrc class elements
	 */
	struct Data {
		FileSrc *abstract_src;	/**< Pointer to "this" src element */
		GstElement *src;	/**< Src element for GStreamer */
		Player::Data *player_data;	/**< Pointer to core Player data */
	};

	Data *data_;	/**< Pointer to internal Data element */

	const char *path_;	/**< Path to input file */

};

#endif /* SRC_FILE_SRC_H_ */
