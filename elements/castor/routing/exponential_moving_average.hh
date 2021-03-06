/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLICK_EXPONENTIAL_MOVING_AVERAGE
#define CLICK_EXPONENTIAL_MOVING_AVERAGE

CLICK_DECLS

class ExponentialMovingAverage {
public:
	ExponentialMovingAverage (double delta) : delta(delta), alpha(0), beta(1) { };
	double get() const {
		return alpha / (alpha + beta);
	}
	void increase() {
		alpha = delta * alpha + 1;
		beta  = delta * beta;
	}
	void decrease() {
		alpha = delta * alpha;
		beta  = delta * beta  + 1;
	}
private:
	double delta; // update weight
	double alpha; // running average of packets delivered
	double beta;  // running average of packets not delivered
};

CLICK_ENDDECLS

#endif
