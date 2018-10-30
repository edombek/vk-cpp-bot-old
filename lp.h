#ifndef LP_H_INCLUDED
#define LP_H_INCLUDED

namespace lp {
	void getServer();
	void loop();
	bool errors(json lp_data);
	void updates(json updates);
}

#endif
