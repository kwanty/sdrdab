/*
 * abstract_sink.h
 *
 *  Created on: May 24, 2015
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#ifndef SRC_ABSTRACT_SINK_H_
#define SRC_ABSTRACT_SINK_H_

/**
 * @class AbstractSink
 * @brief Abstract class for sinks
 *
 * @author Kacper Patro patro.kacper@gmail.com
 * @copyright Public domain
 * @pre
 */
class AbstractSink {
public:
	AbstractSink();
	virtual ~AbstractSink();

	/**
	 * Initializes sink structures
	 * @param[in,out] other_data Other data pointer, may be pointer to structure being linked
	 */
	virtual void InitSink(void *other_data) = 0;

	/**
	 * Returns element name
	 * @return Element name
	 */
	virtual const char *name() const = 0;

	/**
	 * This one should be called when unlinking sink
	 */
	virtual void Finish() = 0;

	/**
	 * Checks if sink is already linked
	 * @return Current linkage status
	 */
	virtual bool linked() const = 0;

	/**
	 * Equality operator overload, compares sinks names
	 * @return True when elements equal, false otherwise
	 */
	bool operator ==(const AbstractSink &) const;

};

#endif /* SRC_ABSTRACT_SINK_H_ */
