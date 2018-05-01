//
// Created by Uncle Chu on 28/4/18.
//

#include "Python.h"
#include "BATSTradingStatusMsg.hpp"
#include "BATSTradeMsg.hpp"
#include "BATSRetailPriceImproveMsg.hpp"
#include "BATSTradeBreakMsg.hpp"
#include "BATSOrderExecutedMsg.hpp"

BOOST_PYTHON_MODULE(bats_api)
{
    PyEval_InitThreads(); // need this to mess around with GIL

    BATSTradingStatusMsg::export_to_python();
	BATSTradeMsg::export_to_python();
	BATSRetailPriceImproveMsg::export_to_python();
	BATSTradeBreakMsg::export_to_python();
	BATSOrderExecutedMsg::export_to_python();
}