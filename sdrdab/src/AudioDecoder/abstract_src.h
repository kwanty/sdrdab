/*
 * abstract_src.h
 *
 *  Created on: May 24, 2015
 *      Author: Kacper Patro patro.kacper@gmail.com
 */

#ifndef SRC_ABSTRACT_SRC_H_
#define SRC_ABSTRACT_SRC_H_

/**
 * @class AbstractSrc
 * @brief Abstract class for sources
 *
 * @author Kacper Patro patro.kacper@gmail.com
 * @copyright Public domain
 * @pre
 */
class AbstractSrc {
public:
	AbstractSrc();
	virtual ~AbstractSrc();

	/**
	 * Sets source properties, before linking
	 * @param[in,out] other_data Other data pointer, may be pointer to structure being linked
	 */
	virtual void SetSrc(void *other_data) = 0;

	/**
	 * Links source element
	 */
	virtual void LinkSrc() = 0;

	/**
	 * Returns element name
	 * @return Element name
	 */
	virtual const char *name() const = 0;

	/**
	 * Resets source state
	 */
	virtual void ResetSrc() = 0;

};

#endif /* SRC_ABSTRACT_SRC_H_ */
