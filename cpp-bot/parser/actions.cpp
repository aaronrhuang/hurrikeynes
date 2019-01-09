#include "./actions.hpp"

Action FoldAction(void) {
    return (Action) { FOLD_ACTION_TYPE, 0 };
}

Action CheckAction(void) {
    return (Action) { CHECK_ACTION_TYPE, 0 };
}

Action CallAction(void) {
    return (Action) { CALL_ACTION_TYPE, 0 };
}

Action ExchangeAction(void) {
    return (Action) { EXCHANGE_ACTION_TYPE, 0 };
}

Action BetAction(int amount) {
    return (Action) { BET_ACTION_TYPE, amount };
}

Action RaiseAction(int amount) {
    return (Action) { RAISE_ACTION_TYPE, amount };
}
