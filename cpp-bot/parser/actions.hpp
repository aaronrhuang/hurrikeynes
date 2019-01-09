#ifndef __PARSER_ACTIONS_HPP__
#define __PARSER_ACTIONS_HPP__

enum ActionType {
    FOLD_ACTION_TYPE       = (1 << 0),
    CHECK_ACTION_TYPE      = (1 << 1),
    CALL_ACTION_TYPE       = (1 << 2),
    EXCHANGE_ACTION_TYPE   = (1 << 3),
    BET_ACTION_TYPE        = (1 << 4),
    RAISE_ACTION_TYPE      = (1 << 5)
};

struct Action {
    ActionType type;
    int amount;
};

Action FoldAction(void);
Action CheckAction(void);
Action CallAction(void);
Action ExchangeAction(void);
Action BetAction(int amount);
Action RaiseAction(int amount);

#endif  // __PARSER_ACTIONS_HPP__
