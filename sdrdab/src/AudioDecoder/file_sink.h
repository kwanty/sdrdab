/*
 * file_sink.h
 *
 *  Created on: May 21, 2015
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#ifndef SRC_FILE_SINK_H_
#define SRC_FILE_SINK_H_

#include "abstract_sink.h"
#include "player.h"
#include <gst/gst.h>

/**
 * @namespace FileSinkCallbacks
 * @brief This namespace provides necessary for FileSink class callbacks (mostly GStreamer callbacks)
 */
namespace FileSinkCallbacks {

/**
 * GStreamer callback called when unlinking sink from pipeline. Check GStreamer documentation for more
 */
extern "C" GstPadProbeReturn UnlinkCallFileSink(GstPad *, GstPadProbeInfo *, gpointer);

}

/**
 * @class FileSink
 * @brief Class used to save raw audio files
 *
 * @author Kacper Patro patro.kacper@gmail.com
 * @copyright Public domain
 * @pre
 */
class FileSink: public AbstractSink {
public:
	/**
	 * Constructor of FileSink
	 * @param[in] path Path to output file
	 */
	FileSink(const char *path);
	virtual ~FileSink();

	void InitSink(void *player_data);
	const char *name() const;
	void Finish();
	bool linked() const;

	friend GstPadProbeReturn FileSinkCallbacks::UnlinkCallFileSink(GstPad *, GstPadProbeInfo *, gpointer);

private:
	/**
	 * @struct Data
	 * @brief This struct contains specific for FileSink class elements
	 */
	struct Data {
		FileSink *abstract_sink;	/**< Pointer to "this" sink element */

		GstElement *queue;	/**< Queue element for GStreamer */
		GstElement *sink;	/**< Sink element for GStreamer */
		GstPad *teepad;	/**< TeePad element for GStreamer */

		gboolean removing;	/**< True, when sink is being removed from pipeline */
		bool linked;	/**< True, when sink is linked in pipeline */

		Player::Data *player_data;	/**< Pointer to core Player data */
	};

	Data *data_;	/**< Pointer to internal Data element */

	const char *path_;	/**< Path to output file */

};

#endif /* SRC_FILE_SINK_H_ */
