/*
 * null_sink.h
 *
 *  Created on: Jun 3, 2015
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#ifndef AUDIODECODER_SRC_NULL_SINK_H_
#define AUDIODECODER_SRC_NULL_SINK_H_

#include "abstract_sink.h"
#include "player.h"
#include <gst/gst.h>

/**
 * @namespace NullSinkCallbacks
 * @brief This namespace provides necessary for NullSink class callbacks (mostly GStreamer callbacks)
 */
namespace NullSinkCallbacks {

/**
 * GStreamer callback called when unlinking sink from pipeline. Check GStreamer documentation for more
 */
extern "C" GstPadProbeReturn UnlinkCallNullSink(GstPad *, GstPadProbeInfo *, gpointer);

}

/**
 * @class NullSink
 * @brief Base class to keep input buffer empty when not playing
 *
 * @author Kacper Patro patro.kacper@gmail.com
 * @copyright Public domain
 * @pre
 */
class NullSink: public AbstractSink {
public:
	NullSink();
	virtual ~NullSink();

	void InitSink(void *player_data);
	const char *name() const;
	void Finish();
	bool linked() const;

	friend GstPadProbeReturn NullSinkCallbacks::UnlinkCallNullSink(GstPad *, GstPadProbeInfo *, gpointer);

private:
	/**
	 * @struct Data
	 * @brief This struct contains specific for NullSink class elements
	 */
	struct Data {
		NullSink *abstract_sink;	/**< Pointer to "this" sink element */

		GstElement *queue;	/**< Queue element for GStreamer */
		GstElement *sink;	/**< Sink element for GStreamer */
		GstPad *teepad;	/**< TeePad element for GStreamer */

		gboolean removing;	/**< True, when sink is being removed from pipeline */
		bool linked;	/**< True, when sink is linked in pipeline */

		Player::Data *player_data;	/**< Pointer to core Player data */
	};

	Data *data_;	/**< Pointer to internal Data element */

};

#endif /* AUDIODECODER_SRC_NULL_SINK_H_ */
