#include "utils.h"


namespace calm {
    bool is_active_order(OrderStatus status) {
        switch (status) {
            case OrderStatus::pending_submit:
            case OrderStatus::pending_cancel:
            case OrderStatus::pre_submitted:
            case OrderStatus::submitted:
                return true;
            default:
                return false;
        }
    }


}