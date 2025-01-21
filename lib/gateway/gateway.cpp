//
// Created by wayne on 16/1/25.
//

#include "gateway.h"
#include "utils/logging.h"

calm::IBGateway::IBGateway(EventEngine &event_engine): event_engine{event_engine} {
    logger = init_sub_logger("ib_gateway");
}
